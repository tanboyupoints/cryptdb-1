#include <parser/CryptoHandlers.hh>
#include <util/util.hh>

using namespace std;

//TODO: this is duplicated in cdb_rewrite
static char *
make_thd_string(const string &s, size_t *lenp = 0)
{
    THD *thd = current_thd;
    assert(thd);

    if (lenp)
        *lenp = s.size();
    return thd->strmake(s.data(), s.size());
}

//TODO: OnionTypeHandler not needed any more
Create_field*
OnionTypeHandler::newOnionCreateField(const char * anon_name,
		    const Create_field *f) const {
    THD *thd = current_thd;
    Create_field *f0 = f->clone(thd->mem_root);
    f0->field_name = thd->strdup(anon_name);
    if (field_length != -1) {
	f0->length = field_length;
    }
    f0->sql_type = type;
    
    if (charset != NULL) {
	f0->charset = charset;
    } else {
	//encryption is always unsigned
	f0->flags = f0->flags | UNSIGNED_FLAG; 
    }
    return f0;
}

Item*
OnionTypeHandler::createItem(const string & data) {
    switch (type) {
    case MYSQL_TYPE_LONGLONG: {
	return new Item_int((ulonglong)valFromStr(data));
    }
    case MYSQL_TYPE_BLOB: 
    case MYSQL_TYPE_VARCHAR: {
	return new Item_string(make_thd_string(data), data.length(),
			       &my_charset_bin);
    }
    default: {}
    }

    cerr << "incorrect mysql_type " << type << "\n";
    exit(-1);
}

//TODO: remove above newcreatefield
static Create_field*
createFieldHelper(const Create_field *f, int field_length,
		  enum enum_field_types type,
		  CHARSET_INFO * charset = NULL) {
    THD *thd = current_thd;
    Create_field *f0 = f->clone(thd->mem_root);
    if (field_length != -1) {
	f0->length = field_length;
    }
    f0->sql_type = type;
    
    if (charset != NULL) {
	f0->charset = charset;
    } else {
	//encryption is always unsigned
	f0->flags = f0->flags | UNSIGNED_FLAG; 
    }
    return f0;
}

//TODO: remove duplication of this func from cdb_rewrite
static inline bool
IsMySQLTypeNumeric(enum_field_types t) {
    switch (t) {
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_TINY:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_FLOAT:
        case MYSQL_TYPE_DOUBLE:
        case MYSQL_TYPE_LONGLONG:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_NEWDECIMAL:

        // numeric also includes dates for now,
        // since it makes sense to do +/- on date types
        case MYSQL_TYPE_TIMESTAMP:
        case MYSQL_TYPE_DATE:
        case MYSQL_TYPE_TIME:
        case MYSQL_TYPE_DATETIME:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_NEWDATE:
            return true;
        default: return false;
    }
}


RND_int::RND_int(Create_field * f, PRNG * key) {
    cf = f;
    rawkey = key->rand_string(key_bytes);
    this->key = CryptoManager::get_key_SEM(rawkey);
}


Create_field *
RND_int::newCreateField() {
    return createFieldHelper(cf, ciph_size, MYSQL_TYPE_LONGLONG);
}

//TODO: may want to do more specialized crypto for lengths
Item *
RND_int::encrypt(Item * ptext, uint64_t IV) {
    //TODO: should have encrypt_SEM work for any length
    return new Item_int(CryptoManager::encrypt_SEM((uint64_t)static_cast<Item_int *>(ptext)->value, key, IV));
}

Item *
RND_int::decrypt(Item * ctext, uint64_t IV) {
    return new Item_int(CryptoManager::decrypt_SEM((uint64_t)static_cast<Item_int*>(ctext)->value, key, IV));
}

std::string
RND_int::decryptUDF(const string & col, const string & ivcol) {
    cerr << "udf expects key represented in different manner, fix udf";
    std::string query = "UPDATE columnref SET col = decrypt_int_sem(" \
	+ col + ", " + \
	rawkey + "," + \
	ivcol + ");";

    return query;
	
}

EncLayer *
EncLayerFactory::encLayer(SECLEVEL sl, Create_field * cf, PRNG * key) {
    switch (sl) {
    case SECLEVEL::SEMANTIC_DET:
    case SECLEVEL::SEMANTIC_OPE: {
	if (IsMySQLTypeNumeric(cf->sql_type)) {
	    return new RND_int(cf, key);
	} else {
	    return new RND_string(cf, key);
	}
    }
    default:{
	
    }
    }
    thrower() << "unknown or unimplemented security level \n";
}
    
static string
ItemToString(Item * i) {
    String s;
    String *s0 = i->val_str(&s);
    assert(s0 != NULL);
    return string(s0->ptr(), s0->length());
}



RND_string::RND_string(Create_field * f, PRNG * key) {
    cf = f;
    rawkey = key->rand_string(key_bytes);
    enckey = get_AES_enc_key(rawkey);
    deckey = get_AES_dec_key(rawkey);
}

Create_field *
RND_string::newCreateField() {
//TODO: use more precise sizes and types
    return createFieldHelper(cf, -1, MYSQL_TYPE_BLOB);
}

Item *
RND_string::encrypt(Item * ptext, uint64_t IV) {
    string enc = CryptoManager::encrypt_SEM(
	ItemToString(static_cast<Item_string *>(ptext)),
	enckey, IV);
    return new Item_string(make_thd_string(enc), enc.length(), &my_charset_bin);
}

Item *
RND_string::decrypt(Item * ctext, uint64_t IV) {
    string dec = CryptoManager::decrypt_SEM(
	ItemToString(static_cast<Item_string *>(ctext)),
	deckey, IV);
    return new Item_string(make_thd_string(dec), dec.length(), &my_charset_bin);
}

std::string
RND_string::decryptUDF(const std::string & col, const std::string & ivcol) {
    cerr << "udf expects key represented in different manner, fix udf";
    std::string query = "UPDATE columnref SET col = decrypt_text_sem(" + col + ", " +
	rawkey + "," +
	ivcol + ");";
    
    return query;

}


DET_int::DET_int(Create_field * f, PRNG * key) {
    cf = f;
    rawkey = key->rand_string(bf_key_size);
    this->key = new blowfish(rawkey);
}

Create_field *
DET_int::newCreateField() {
    return createFieldHelper(cf, ciph_size, MYSQL_TYPE_LONGLONG);
}

//TODO: may want to do more specialized crypto for lengths
Item *
DET_int::encrypt(Item * ptext, uint64_t IV) {
    return new Item_int(key->encrypt(static_cast<Item_int *>(ptext)->value));
}

Item *
DET_int::decrypt(Item * ctext, uint64_t IV) {
    return new Item_int(key->decrypt(static_cast<Item_int*>(ctext)->value));
}

std::string
DET_int::decryptUDF(const string & col, const string & ivcol) {
    cerr << "udf expects key represented in different manner, fix udf";
    std::string query = "UPDATE columnref SET col = decrypt_int_det(" \
	+ col + ", " + \
	rawkey + ");";

    return query;
	
}

DET_string::DET_string(Create_field * f, PRNG * key) {
    cf = f;
    rawkey = key->rand_string(key_bytes);
    enckey = get_AES_enc_key(rawkey);
    deckey = get_AES_dec_key(rawkey);
}


Create_field *
DET_string::newCreateField() {
//TODO: use more precise sizes and types
    return createFieldHelper(cf, -1, MYSQL_TYPE_BLOB);
}

Item *
DET_string::encrypt(Item * ptext, uint64_t IV) {
    string enc = encrypt_AES_CMC(
	ItemToString(static_cast<Item_string *>(ptext)),
	enckey, false);
    return new Item_string(make_thd_string(enc), enc.length(), &my_charset_bin);
}

Item *
DET_string::decrypt(Item * ctext, uint64_t IV) {
    string dec = decrypt_AES_CMC(
	ItemToString(static_cast<Item_string *>(ctext)),
	deckey, false);
    return new Item_string(make_thd_string(dec), dec.length(), &my_charset_bin);
}

std::string
DET_string::decryptUDF(const std::string & col, const std::string & ivcol) {
    cerr << "udf expects key represented in different manner, fix udf";
    std::string query = "UPDATE columnref SET col = decrypt_text_det(" + col + ", " +
	rawkey + ");";
    
    return query;

}


OPE_int::OPE_int(Create_field * cf, PRNG *key) {
    this->cf = cf;
    rawkey = key->rand_string(key_bytes);
    this->key = CryptoManager::get_key_OPE(rawkey, plain_size, ciph_size);
}

Create_field *
OPE_int::newCreateField() {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG);
}

Item *
OPE_int::encrypt(Item * ptext, uint64_t IV) {
    uint64_t enc = CryptoManager::encrypt_OPE(static_cast<Item_int *>(ptext)->value, key);
    return new Item_int(enc);
}

Item *
OPE_int::decrypt(Item * ctext, uint64_t IV) {
    uint32_t dec = CryptoManager::decrypt_OPE(static_cast<Item_int*>(ctext)->value, key);
    return new Item_int((ulonglong)dec);
}


OPE_string::OPE_string(Create_field * cf, PRNG *key) {
    this->cf = cf;
    rawkey = key->rand_string(key_bytes);
    this->key = CryptoManager::get_key_OPE(rawkey, plain_size, ciph_size);
}

Create_field *
OPE_string::newCreateField() {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG);
}

Item *
OPE_string::encrypt(Item * ptext, uint64_t IV) {
    uint64_t enc = CryptoManager::encrypt_OPE_text_wrapper(ItemToString(ptext), key);
    return new Item_int(enc);
}

Item *
OPE_string::decrypt(Item * ctext, uint64_t IV)   {
    thrower() << "should not decrypt string from OPE \n";
}

