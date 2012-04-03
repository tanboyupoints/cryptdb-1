#include <parser/CryptoHandlers.hh>
#include <crypto/ope.hh>
#include <util/util.hh>
#include <util/cryptdb_log.hh>


using namespace std;
using namespace NTL;

//TODO: this is duplicated in cdb_rewrite
//moved to util/util
/*static char *
make_thd_string(const string &s, size_t *lenp = 0)
{
    THD *thd = current_thd;
    assert(thd);

    if (lenp)
        *lenp = s.size();
    return thd->strmake(s.data(), s.size());
}

static string
ItemToString(Item * i) {
    String s;
    String *s0 = i->val_str(&s);
    assert(s0 != NULL);
    return string(s0->ptr(), s0->length());
    }*/

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

/****************** RND *********************/

RND_int::RND_int(Create_field * f, PRNG * prng)
    : EncLayer(f),
      key(prng->rand_string(key_bytes)),
      bf(key)
{
}


Create_field *
RND_int::newCreateField() {
    return createFieldHelper(cf, ciph_size, MYSQL_TYPE_LONGLONG);
}

//TODO: may want to do more specialized crypto for lengths
Item *
RND_int::encrypt(Item * ptext, uint64_t IV) {
    //TODO: should have encrypt_SEM work for any length
    uint64_t p = static_cast<Item_int *>(ptext)->value;
    uint64_t c = bf.encrypt(p ^ IV);
    LOG(encl) << "RND_int encrypt " << p << " IV " << IV << "-->" << c;
    return new Item_int((ulonglong) c);
}

Item *
RND_int::decrypt(Item * ctext, uint64_t IV) {
    uint64_t c = static_cast<Item_int*>(ctext)->value;
    uint64_t p = bf.decrypt(c) ^ IV;
    LOG(encl) << "RND_int decrypt " << c << " IV " << IV << " --> " << p;
    return new Item_int((ulonglong) p);
}


static LEX_STRING n_decRNDInt = {
    (char *) "decrypt_int_sem",
    sizeof("decrypt_int_sem"),
};

static udf_func u_decRNDInt = {
    n_decRNDInt,
    INT_RESULT,
    UDFTYPE_FUNCTION,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0L,
};


Item * 
RND_int::decryptUDF(Item * col, Item * ivcol) {
    List<Item> l;
    l.push_back(col);
    
    Item * keyI = new Item_string(make_thd_string(key),
				  key.length(), &my_charset_bin);
    keyI->name = NULL;
    l.push_back(keyI);
    
    l.push_back(ivcol);
    
    Item * udfdec = new Item_func_udf_int(&u_decRNDInt, l);
    udfdec->name = NULL; //no alias

    //add encompassing CAST for unsigned
    Item * udf = new Item_func_unsigned(udfdec);
    udf->name = NULL;

    return udf;
}


RND_str::RND_str(Create_field * f, PRNG * key)
    : EncLayer(f),
      rawkey(key->rand_string(key_bytes))
{
    enckey = get_AES_enc_key(rawkey);
    deckey = get_AES_dec_key(rawkey);
}

Create_field *
RND_str::newCreateField() {
//TODO: use more precise sizes and types
    return createFieldHelper(cf, -1, MYSQL_TYPE_BLOB);
}

Item *
RND_str::encrypt(Item * ptext, uint64_t IV) {
    string enc = CryptoManager::encrypt_SEM(
	ItemToString(static_cast<Item_string *>(ptext)),
	enckey, IV);
    LOG(encl) << "RND_str encrypt " << ItemToString(ptext) << " IV " << IV << "--->" << enc;
    return new Item_string(make_thd_string(enc), enc.length(), &my_charset_bin);
}

Item *
RND_str::decrypt(Item * ctext, uint64_t IV) {
    string dec = CryptoManager::decrypt_SEM(
	ItemToString(static_cast<Item_string *>(ctext)),
	deckey, IV);
    LOG(encl) << "RND_str decrypt " << ItemToString(ctext) << " IV " << IV << "-->" << dec;
    return new Item_string(make_thd_string(dec), dec.length(), &my_charset_bin);
}


//TODO; make edb.cc udf naming consistent with these handlers
static LEX_STRING n_decRNDString = {
    (char *) "decrypt_text_sem",
    sizeof("decrypt_text_sem"),
};

static udf_func u_decRNDString = {
    n_decRNDString,
    STRING_RESULT,
    UDFTYPE_FUNCTION,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0L,
};


Item *
RND_str::decryptUDF(Item * col, Item * ivcol) {
    List<Item> l;
    l.push_back(col);
    l.push_back(new Item_string(make_thd_string(rawkey), rawkey.length(), &my_charset_bin));
    l.push_back(ivcol);
    
    return new Item_func_udf_str(&u_decRNDString, l);	
}


/********** DET ************************/



DET_int::DET_int(Create_field * f, PRNG * prng)
    : EncLayer(f),
      key(prng->rand_string(bf_key_size)),
      bf(key)
{
}

Create_field *
DET_int::newCreateField() {
    return createFieldHelper(cf, ciph_size, MYSQL_TYPE_LONGLONG);
}

//TODO: may want to do more specialized crypto for lengths
Item *
DET_int::encrypt(Item * ptext, uint64_t IV) {
    ulonglong val = static_cast<Item_int *>(ptext)->value;
    ulonglong res = (ulonglong) bf.encrypt(val);
    LOG(encl) << "DET_int encrypt " << val << "--->" << res;
    return new Item_int(res);
}

Item *
DET_int::decrypt(Item * ctext, uint64_t IV) {
    ulonglong val = static_cast<Item_int*>(ctext)->value;
    ulonglong res = (ulonglong) bf.decrypt(val);
    LOG(encl) << "DET_int decrypt " << val << "-->" << res;
    Item * ni = new Item_int(res);
    cerr << "det int " << ni << "\n";
    return ni;
}


static LEX_STRING n_decDETInt = {
    (char *) "decrypt_int_det",
    sizeof("decrypt_int_det"),
};

static udf_func u_decDETInt = {
    n_decDETInt,
    INT_RESULT,
    UDFTYPE_FUNCTION,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0L,
};


Item *
DET_int::decryptUDF(Item * col, Item * ivcol) {
    List<Item> l;
    l.push_back(col);

    Item * keyI = new Item_string(make_thd_string(key), key.length(), &my_charset_bin);
    keyI->name = NULL;
    l.push_back(keyI);
    
    Item * udfdec = new Item_func_udf_int(&u_decDETInt, l);
    udfdec->name = NULL;
    
    //add encompassing CAST for unsigned
    Item * udf = new Item_func_unsigned(udfdec);
    udf->name = NULL;

    return udf;
}

DET_str::DET_str(Create_field * f, PRNG * key)
    : EncLayer(f)
{
    rawkey = key->rand_string(key_bytes);
    enckey = get_AES_enc_key(rawkey);
    deckey = get_AES_dec_key(rawkey);
}


Create_field *
DET_str::newCreateField() {
//TODO: use more precise sizes and types
    return createFieldHelper(cf, -1, MYSQL_TYPE_BLOB);
}

Item *
DET_str::encrypt(Item * ptext, uint64_t IV) {
    string enc = encrypt_AES_CMC(
	ItemToString(static_cast<Item_string *>(ptext)),
	enckey, false);
    return new Item_string(make_thd_string(enc), enc.length(), &my_charset_bin);
}

Item *
DET_str::decrypt(Item * ctext, uint64_t IV) {
    string dec = decrypt_AES_CMC(
	ItemToString(static_cast<Item_string *>(ctext)),
	deckey, false);
    return new Item_string(make_thd_string(dec), dec.length(), &my_charset_bin);
}



static LEX_STRING n_decDETStr = {
    (char *) "decrypt_text_det",
    sizeof("decrypt_text_det"),
};

static udf_func u_decDETStr = {
    n_decDETStr,
    STRING_RESULT,
    UDFTYPE_FUNCTION,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0L,
};



Item *
DET_str::decryptUDF(Item * col, Item * ivcol) {
    List<Item> l;
    l.push_back(col);
    l.push_back(new Item_string(make_thd_string(rawkey), rawkey.length(), &my_charset_bin));
    
    return new Item_func_udf_str(&u_decDETStr, l);

}

/*************** DETJOIN *********************/

Item *
DETJOIN::encrypt(Item * p, uint64_t IV) {
    ulonglong val = static_cast<Item_int *>(p)->value;
    return new Item_int(val);
}

Item *
DETJOIN::decrypt(Item * c, uint64_t IV) {
    ulonglong val = static_cast<Item_int *>(c)->value;
    return new Item_int(val);
}
  

/**************** OPE **************************/

OPE_int::OPE_int(Create_field * f, PRNG * prng)
    : EncLayer(f),
      key(prng->rand_string(key_bytes)),
      ope(key, plain_size * 8, ciph_size * 8)
{
}

Create_field *
OPE_int::newCreateField() {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG);
}

Item *
OPE_int::encrypt(Item * ptext, uint64_t IV) {
    ulong pval =  (ulong)static_cast<Item_int *>(ptext)->value;
    ulonglong enc = uint64FromZZ(ope.encrypt(to_ZZ(pval)));
    LOG(encl) << "OPE_int encrypt " << pval << " IV " << IV << "--->" << enc;
    return new Item_int(enc);
}

Item *
OPE_int::decrypt(Item * ctext, uint64_t IV) {
    ulong cval = (ulong) static_cast<Item_int*>(ctext)->value;
    ulonglong dec = uint64FromZZ(ope.decrypt(to_ZZ(cval)));
    LOG(encl) << "OPE_int decrypt " << cval << " IV " << IV << "--->" << dec; 
    return new Item_int(dec);
}


OPE_str::OPE_str(Create_field * f, PRNG * prng)
    : EncLayer(f),
      key(prng->rand_string(key_bytes)),
      ope(key, plain_size * 8, ciph_size * 8)
{
}

Create_field *
OPE_str::newCreateField() {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG);
}

Item *
OPE_str::encrypt(Item * ptext, uint64_t IV) {
    string ps = ItemToString(ptext);
    if (ps.size() < plain_size)
        ps = string(plain_size - ps.size(), 0) + ps;
    uint32_t pv;
    memcpy(&pv, ps.data(), plain_size);
    ZZ enc = ope.encrypt(to_ZZ(ntohl(pv)));
    return new Item_int((ulonglong) trunc_long(enc, ciph_size));
}

Item *
OPE_str::decrypt(Item * ctext, uint64_t IV) {
    thrower() << "cannot decrypt string from OPE";
}

/**************** HOM ***************************/

HOM::HOM(Create_field * f, PRNG * key)
    : EncLayer(f),
      sk(Paillier_priv::keygen(key, nbits))
{
}

Create_field *
HOM::newCreateField() {
    return createFieldHelper(cf, 2*nbits/8, MYSQL_TYPE_VARCHAR, &my_charset_bin);
}

static ZZ
ItemIntToZZ(Item * ptext) {
    ulonglong val = ((Item_int*) ptext)->value;
    return to_ZZ((unsigned long)val);
}

static Item *
ZZToItemInt(const ZZ & val) {
    ulonglong v = to_ulong(val);
    return new Item_int(v);
}

static Item *
ZZToItemStr(const ZZ & val) {
    string str = StringFromZZ(val);
    cerr << "n2 len is " << str.length() << " first byte " << (int)str[0] << "\n";
    Item * newit = new Item_string(make_thd_string(str), str.length(), &my_charset_bin);
    newit->name = NULL; //no alias

    return newit;
}

static ZZ
ItemStrToZZ(Item* i) {
    string res = ItemToString(i);
    return ZZFromString(res);
}

Item *
HOM::encrypt(Item * ptext, uint64_t IV) {
    ZZ enc = sk.encrypt(ItemIntToZZ(ptext));
    return ZZToItemStr(enc);
}

Item *
HOM::decrypt(Item * ctext, uint64_t IV) {
    ZZ enc = ItemStrToZZ(ctext);
    ZZ dec = sk.decrypt(enc);
    LOG(encl) << "HOM ciph " << enc << "---->" << dec; 
    return ZZToItemInt(dec);
}


static LEX_STRING n_sum = {
    (char*)"agg",
    sizeof("agg"),
};

static udf_func u_sum = {
    n_sum,
    STRING_RESULT,
    UDFTYPE_AGGREGATE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0L,
};


Item *
HOM::sumUDF(Item * expr) {
         
    List<Item> l;
    l.push_back(expr);
    l.push_back(ZZToItemStr(sk.hompubkey()));
        
    return new Item_func_udf_str(&u_sum, l);
}

/******* SEARCH **************************/

Search::Search(Create_field * f, PRNG * key)
    : EncLayer(f)
{
    rawkey = key->rand_string(key_bytes);
    this->key = Binary(key_bytes, (unsigned char *)rawkey.data());
}

Create_field *
Search::newCreateField() {
    return createFieldHelper(cf, -1, MYSQL_TYPE_BLOB);
}


//this function should in fact be provided by the programmer
//currently, we split by whitespaces
// only consider words at least 3 chars in len
// discard not unique objects
static list<Binary> *
tokenize(string text)
{

    list<string> tokens = split(text, " ,;:.");

    std::set<string> search_tokens;

    list<Binary> * res = new list<Binary>();

    for (list<string>::iterator it = tokens.begin(); it != tokens.end();
         it++) {
        if ((it->length() >= 3) &&
            (search_tokens.find(*it) == search_tokens.end())) {
            string token = toLowerCase(*it);
            search_tokens.insert(token);
            res->push_back(Binary((uint) it->length(),
                                  (unsigned char *) token.data()));
        }
    }

    search_tokens.clear();
    return res;

}

Item *
Search::encrypt(Item * ptext, uint64_t IV) {
    string plainstr = ItemToString(ptext);
    //TODO: remove Binary, string serves this purpose now..
    list<Binary> * tokens = tokenize(plainstr);
    Binary ciph = CryptoManager::encryptSWP(key, *tokens);
    
    return new Item_string((const char *)ciph.content, ciph.len, &my_charset_bin);
}

Item *
Search::decrypt(Item * ctext, uint64_t IV) {
    thrower() << "decryption from SWP not supported \n";
}


static LEX_STRING n_search = {
    (char*)"searchSWP",
    sizeof("searchSWP"),
};

static udf_func u_search = {
    n_search,
    INT_RESULT,
    UDFTYPE_FUNCTION,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0L,
};

Item *
Search::searchUDF(Item * expr) {
    Token t = CryptoManager::token(key, Binary(ItemToString(expr)));
    
    List<Item> l;
    l.push_back(expr);
    l.push_back(new Item_string((const char *)t.ciph.content, t.ciph.len, &my_charset_bin));
    l.push_back(new Item_string((const char *)t.wordKey.content, t.wordKey.len, & my_charset_bin));
    
    return new Item_func_udf_int(&u_sum, l);
}

/************ EncLayer factory creation  ********/

EncLayer *
EncLayerFactory::encLayer(SECLEVEL sl, Create_field * cf, PRNG * key) {
    switch (sl) {
    case SECLEVEL::RND: {
	if (IsMySQLTypeNumeric(cf->sql_type)) {
	    return new RND_int(cf, key);
	} else {
	    return new RND_str(cf, key);
	}
    }
    case SECLEVEL::DET: {
	if (IsMySQLTypeNumeric(cf->sql_type)) {
	    return new DET_int(cf, key);
	} else {
	    return new DET_str(cf, key);
	}
    }
    case SECLEVEL::DETJOIN: {
	return new DETJOIN(cf, key);
    }
    case SECLEVEL::OPE: {
	if (IsMySQLTypeNumeric(cf->sql_type)) {
	    return new OPE_int(cf, key);
	} else {
	    return new OPE_str(cf, key);
	}
    }
    case SECLEVEL::HOM: {
	return new HOM(cf, key);
    }
    case SECLEVEL::SEARCH: {
	return new Search(cf, key);
    }
    default:{
	
    }
    }
    thrower() << "unknown or unimplemented security level \n";
}
