#include <main/CryptoHandlers.hh>
#include <crypto/ope.hh>
#include <crypto/BasicCrypto.hh>
#include <crypto/SWPSearch.hh>
#include <util/util.hh>
#include <util/cryptdb_log.hh>
#include <crypto/arc4.hh>

#include <cmath>

#define LEXSTRING(cstr) { (char*) cstr, sizeof(cstr) }


using namespace std;
using namespace NTL;

#include <utility>


/* Implementation class hierarchy is as in .hh file plus:

   - We have a set of implementation EncLayer-s: each implementation layer
   is tied to some specific encryption scheme and key/block size:
   RND_int, RND_str, DET_int, DET_str, OPE_int, OPE_str

   - LayerFactory: creates an implementation EncLayer for a certain security
   level and field type:

   - RNDFactory: outputs a RND layer
         - RND layers: RND_int for blowfish, RND_str for AES

    -DETFactory: outputs a DET layer
         - DET layers: DET_int, DET_str

    -OPEFactory: outputs a OPE layer
         - OPE layers: OPE_int, OPE_str
  
 */


//============= FACTORIES ==========================//

class LayerFactory {
public:
    static EncLayer * create(Create_field * cf, std::string key) {
	throw "needs to be inherited";
    };
    static EncLayer * deserialize(const std::string & serial) {
	throw "needs to be inherited";
    };
};

class RNDFactory : public LayerFactory {
public:
    static EncLayer * create(Create_field * cf, std::string key);
    static EncLayer * deserialize(const std::string & serial);
};


class DETFactory : public LayerFactory {
public:
    static EncLayer * create(Create_field * cf, std::string key);
    static EncLayer * deserialize(const std::string & serial);
};

class OPEFactory : public LayerFactory {
public:
    static EncLayer * create(Create_field * cf, std::string key);
    static EncLayer * deserialize(const std::string & serial);
};


EncLayer *
EncLayerFactory::encLayer(onion o, SECLEVEL sl, Create_field * cf,
                                string key)
{
    switch (sl) {
    case SECLEVEL::RND: {
	return RNDFactory::create(cf, key);
    }
    case SECLEVEL::DET:
    case SECLEVEL::DETJOIN: {
	return DETFactory::create(cf, key);
    }
	
    case SECLEVEL::OPE: {
	return OPEFactory::create(cf, key);
    }
    case SECLEVEL::HOM: {
	return new HOM(cf, key);
    }
    case SECLEVEL::SEARCH: {
	return new Search(cf, key);
    }
    default:{}
    }
    thrower() << "unknown or unimplemented security level \n";
}

EncLayer *
EncLayerFactory::encLayerFromSerial(onion o, SECLEVEL sl,
                                    const string & serial)
{
    switch (sl) {
    case SECLEVEL::RND: 
	return RNDFactory::deserialize(serial);
	
    case SECLEVEL::DET: 
    case SECLEVEL::DETJOIN: 
	return DETFactory::deserialize(serial);
	
    case SECLEVEL::OPE: 
	return OPEFactory::deserialize(serial);
	
    case SECLEVEL::HOM: 
	return new HOM(serial);
	
    case SECLEVEL::SEARCH: 
	return new Search(serial);
	
    default:{}
    }
    thrower() << "unknown or unimplemented security level \n";
}



/*===================== helpers =============================*/


/*
 * EncLayer->serialize outputs a string of the form [LEN SECLEVEL LAYER_IMPL_TYPE
 * LAYER_INFO]
 * LEN indicates the length of LAYER_INFO
 * LAYER_IMPL_TYPE is a readable string
 * LAYER_INFO could be binary
 * LayerFactory->deserialize gets as input such a string
 */
static string
serial_pack(SECLEVEL l, const string & layer_impl_type, const string & layer_info) {
    stringstream ss;
    ss.clear();

    ss << layer_info.length() << " " << levelnames[(uint)l] << " " << layer_impl_type << " " << layer_info;

    return ss.str();
}

static void
serial_unpack(const string & serial, SECLEVEL & l, string & layer_impl_type, string & layer_info) {
    stringstream ss(serial);
    uint len;
    ss >> len;
    string levelname;
    ss >> levelname;
    l = string_to_sec_level(levelname);
    ss >> layer_impl_type;
    layer_info = serial.substr(serial.size()-len, len);
}


static string
get_layer_info(const string & serial, SECLEVEL l, const string & name) {
    string impl_type, layer_info;
    SECLEVEL ll;
    serial_unpack(serial, ll, impl_type, layer_info);

    assert_s(ll == l && impl_type == name, "inconsistency in RND_int");

    return layer_info;
}

static string
get_impl_type(const string & serial) {
    string impl_type, layer_info;
    SECLEVEL ll;
    serial_unpack(serial, ll, impl_type, layer_info);

    return impl_type;
}


static
string prng_expand(string seed_key, uint key_bytes) {
    streamrng<arc4> prng(seed_key);
    return prng.rand_string(key_bytes);
}

// returns the length of output by AES encryption of a string of given type
// and len
static
pair<enum enum_field_types, int>
type_len_for_AES_str(enum enum_field_types type, int len, bool pad) {

    int res_len = -1;
    enum enum_field_types res_type = type;
    
    switch (type) {
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
    case MYSQL_TYPE_BLOB:
	break;
    case MYSQL_TYPE_VARCHAR:
	res_len = rounded_len(len, AES_BLOCK_BYTES, pad);
	break;
    default: {
	assert_s(false, "unexpected sql_type");
    }
    }

    return make_pair(res_type, res_len);
  
}

//TODO: remove above newcreatefield
static Create_field*
createFieldHelper(const Create_field *f, int field_length,
		  enum enum_field_types type, string anonname = "",
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

    if (anonname.size() > 0) {
        f0->field_name = make_thd_string(anonname);
    }

    return f0;

}

static Item *
get_key_item(const string & key) {
    Item_string * keyI = new Item_string(make_thd_string(key),
					 key.length(), &my_charset_bin);
    keyI->name = NULL; // no alias
    return keyI;
}



/*********************** RND ************************************************/

class RND_int : public EncLayer {
public:
    RND_int(Create_field *cf, const std::string & seed_key);

    // serialize and deserialize
    virtual std::string serialize() {return serial_pack(level(), name(), key);}
    RND_int(const std::string & serial);

    SECLEVEL level() {return SECLEVEL::RND;}
    string name() {return "RND_int";}
    
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    Item * decryptUDF(Item * col, Item * ivcol);

private:
    std::string key;
    blowfish bf;
    static const int key_bytes = 16;
    static const int ciph_size = 8;

};

class RND_str : public EncLayer {
public:
    RND_str(Create_field *, const std::string & seed_key);

    // serialize and deserialize
    std::string serialize() {return serial_pack(level(), name(), rawkey); }
    RND_str(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::RND;}
    string name() {return "RND_str";}
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    Item * decryptUDF(Item * col, Item * ivcol);

private:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

};

EncLayer *
RNDFactory::create(Create_field * cf, std::string key) {
    if (IsMySQLTypeNumeric(cf->sql_type)) { // the ope case as well 
	 return new RND_int(cf, key);
     } else {
	 return new RND_str(cf, key);
     }
}

EncLayer *
RNDFactory::deserialize(const std::string & serial) {
    if (get_impl_type(serial) == "RND_int") {
	return new RND_int(serial);
    } else {
	return new RND_str(serial);
    }
}


RND_int::RND_int(Create_field * f, const string & seed_key)
    : key(prng_expand(seed_key, key_bytes)),
      bf(key)
{}

RND_int::RND_int(const std::string & serial)
    : key(get_layer_info(serial, level(), name())),
      bf(key)
{}

Create_field *
RND_int::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, ciph_size, MYSQL_TYPE_LONGLONG, anonname);
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

static udf_func u_decRNDInt = {
    LEXSTRING("decrypt_int_sem"),
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

    l.push_back(get_key_item(key));

    l.push_back(ivcol);

    Item * udfdec = new Item_func_udf_int(&u_decRNDInt, l);
    udfdec->name = NULL; //no alias

    //add encompassing CAST for unsigned
    Item * udf = new Item_func_unsigned(udfdec);
    udf->name = NULL;

    return udf;
}

///////////////////////////////////////////////

RND_str::RND_str(Create_field * f,  const string & seed_key)
{
    rawkey = prng_expand(seed_key, key_bytes);
    enckey = get_AES_enc_key(rawkey);
    deckey = get_AES_dec_key(rawkey);
}

RND_str::RND_str(const std::string & serial)
  : rawkey(get_layer_info(serial, level(), name())),
    enckey(get_AES_enc_key(rawkey)),
    deckey(get_AES_dec_key(rawkey))
 {}



Create_field *
RND_str::newCreateField(Create_field * cf, string anonname) {
    auto typelen = type_len_for_AES_str(cf->sql_type, cf->length, false);
  
    return createFieldHelper(cf, typelen.second, typelen.first, anonname, &my_charset_bin);
}


Item *
RND_str::encrypt(Item * ptext, uint64_t IV) {
    string enc = encrypt_AES_CBC(
                 ItemToString(ptext),
                 enckey,
		 BytesFromInt(IV, SALT_LEN_BYTES),
		 false);
    
    LOG(encl) << "RND_str encrypt " << ItemToString(ptext) << " IV " << IV << "--->"
	      << "len of enc " << enc.length() << " enc " << enc;

    return new Item_string(make_thd_string(enc), enc.length(), &my_charset_bin);
}

Item *
RND_str::decrypt(Item * ctext, uint64_t IV) {
    string dec = decrypt_AES_CBC(
	ItemToString(ctext),
	deckey,
	BytesFromInt(IV, SALT_LEN_BYTES),
	false);
    LOG(encl) << "RND_str decrypt " << ItemToString(ctext) << " IV " << IV << "-->"
	      << "len of dec " << dec.length() << " dec: " << dec;

    return new Item_string(make_thd_string(dec), dec.length(), &my_charset_bin);
}


//TODO; make edb.cc udf naming consistent with these handlers
static udf_func u_decRNDString = {
    LEXSTRING("decrypt_text_sem"),
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
    l.push_back(get_key_item(rawkey));
    l.push_back(ivcol);

    return new Item_func_udf_str(&u_decRNDString, l);
}


/********** DET ************************/


class DET_int : public EncLayer {
public:
    DET_int(Create_field *,  const std::string & seed_key);
    DET_int() : key("x"), bf(key) {}; // HACK(raluca)
    
    std::string serialize() {return serial_pack(level(), name(), key); }
    // create object from serialized contents
    DET_int(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::DET;}
    string name() {return "DET_int";}
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * ivcol = NULL);


protected:
    std::string key;
    blowfish bf;
    static const int bf_key_size = 16;
    static const int ciph_size = 8;

};



class DET_dec : public DET_int {
public:
    DET_dec(Create_field *,  const std::string & seed_key);

    std::string serialize();
    // create object from serialized contents
    DET_dec(const std::string & serial);


    string name() {return "DET_dec";}
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * ivcol = NULL);


protected:
    uint decimals; // number of decimals
    ulonglong shift;
    
};

class DET_str : public EncLayer {
public:
    DET_str(Create_field *cf, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return serial_pack(level(), name(), rawkey);}
    DET_str(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::DET;}
    string name() {return "DET_str";}
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * = NULL);

protected:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

};


EncLayer *
DETFactory::create(Create_field * cf, std::string key) {
    if (IsMySQLTypeNumeric(cf->sql_type)) { 
	if (cf->sql_type == MYSQL_TYPE_DECIMAL) {
	    return new DET_dec(cf, key);
	} else {
	    return new DET_int(cf, key);
	}
     } else {
	 return new DET_str(cf, key);
     }
}

EncLayer *
DETFactory::deserialize(const std::string & serial) {
    if (get_impl_type(serial) == "DET_int") {
	return new DET_int(serial);
    } else {
	return new DET_str(serial);
    }
}


DET_int::DET_int(Create_field * f, const string & seed_key)
    : key(prng_expand(seed_key, bf_key_size)),
      bf(key)
{}

DET_int::DET_int(const string & serial) : 
    key(get_layer_info(serial, level(), name())),
    bf(key)
{}

Create_field *
DET_int::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, ciph_size, MYSQL_TYPE_LONGLONG, anonname);
}


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

    return ni;
}


static udf_func u_decDETInt = {
    LEXSTRING("decrypt_int_det"),
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

    l.push_back(get_key_item(key));

    Item * udfdec = new Item_func_udf_int(&u_decDETInt, l);
    udfdec->name = NULL;

    //add encompassing CAST for unsigned
    Item * udf = new Item_func_unsigned(udfdec);
    udf->name = NULL;

    return udf;
}

DET_dec::DET_dec(Create_field * cf, const string & seed_key) : DET_int(cf, seed_key) {
    assert_s(cf->length <= 8, " this type of decimal not supported ");

    decimals = cf->decimals;
    shift = pow(10, decimals);
}

string
DET_dec::serialize() {
    stringstream layerinfo;
    layerinfo.clear();
    layerinfo << decimals << " " << key;
    return serial_pack(level(), name(), layerinfo.str());
}

DET_dec::DET_dec(const string & serial) {
    string layerinfo = get_layer_info(serial, level(), name());
    stringstream ss(layerinfo);

    ss >> decimals;
    shift = pow(10, decimals);

    uint len = layerinfo.length();
    key = layerinfo.substr(len - bf_key_size, bf_key_size);
    bf = blowfish(key);
}

static Item_int *
decimal_to_int(Item_decimal * v, uint decimals, const ulonglong & shift) {
    assert_s(v->decimal_precision() <= decimals, "value has more precision that specified decimals");
    
    ulonglong res = (v->val_int() + v->val_real()) * shift;
    
    return new Item_int(res);
}
Item *
DET_dec::encrypt(Item *ptext, uint64_t IV) {
    Item_decimal * ptext_dec = (Item_decimal *) ptext;
    Item_int * ptext_int = decimal_to_int(ptext_dec, decimals, shift);
    Item * result = DET_int::encrypt(ptext_int, IV);
    delete ptext_int;
    
    return result;
}

Item *
DET_dec::decrypt(Item *ctext, uint64_t IV) {
    Item_int * res_int = static_cast<Item_int*>(DET_int::decrypt(ctext, IV));

    Item_decimal * res = new Item_decimal(res_int->value*1.0/shift, decimals, decimals);

    delete res_int;

    return res;
}

Item *
DET_dec::decryptUDF(Item * col, Item * ivcol) {
    assert_s(false, "should not decrypt decimal");
    return NULL;
}

Create_field *
DET_dec::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG, anonname);
}

DET_str::DET_str(Create_field * f, string seed_key)
{
    rawkey = prng_expand(seed_key, key_bytes);
    enckey = get_AES_enc_key(rawkey);
    deckey = get_AES_dec_key(rawkey);

    
}

DET_str::DET_str(const std::string & serial): 
    rawkey(get_layer_info(serial, level(), name())),
    enckey(get_AES_enc_key(rawkey)),
    deckey(get_AES_dec_key(rawkey))
{}


Create_field *
DET_str::newCreateField(Create_field * cf, string anonname) {
    auto typelen = type_len_for_AES_str(cf->sql_type, cf->length, true);
    
    return createFieldHelper(cf, typelen.second, typelen.first, anonname, &my_charset_bin);
}

Item *
DET_str::encrypt(Item * ptext, uint64_t IV) {
    string plain =  ItemToString(ptext);
    string enc = encrypt_AES_CMC(plain,enckey, true);
    LOG(encl) << " DET_str encrypt " << plain  << " IV " << IV << " ---> "
	      << " enc len " << enc.length() << " enc " << enc;

    return new Item_string(make_thd_string(enc), enc.length(), &my_charset_bin);
}

Item *
DET_str::decrypt(Item * ctext, uint64_t IV) {
    string enc = ItemToString(ctext);
    string dec = decrypt_AES_CMC(enc, deckey, true);
    LOG(encl) << " DET_str decrypt enc len " << enc.length() << " enc " << enc
	      << " IV " << IV << " ---> " << " dec len " << dec.length() << " dec " << dec;

    return new Item_string(make_thd_string(dec), dec.length(), &my_charset_bin);
}

static udf_func u_decDETStr = {
    LEXSTRING("decrypt_text_det"),
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
    l.push_back(get_key_item(rawkey));
    return new Item_func_udf_str(&u_decDETStr, l);

}

/*************** DETJOIN *********************/


class DETJOIN_int : public DET_int {
    //TODO
public:
    DETJOIN_int(Create_field * cf, std::string seed_key) : DET_int(cf, seed_key) {}

    // serialize from parent;  unserialize:
    DETJOIN_int(const std::string & serial) : DET_int(serial) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}
};


class DETJOIN_str : public DET_str {
public:
    DETJOIN_str(Create_field * cf, std::string seed_key) : DET_str(cf, seed_key) {}

    // serialize from parent; unserialize:
    DETJOIN_str(const std::string & serial) : DET_str(serial) {};

    SECLEVEL level() {return SECLEVEL::DETJOIN;}
};




class DETJOIN_dec: public DET_dec {
    //TODO
public:
    DETJOIN_dec(Create_field * cf, std::string seed_key) : DET_dec(cf, seed_key) {}

    // serialize from parent;  unserialize:
    DETJOIN_dec(const std::string & serial) : DET_dec(serial) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}
};


/**************** OPE **************************/


class OPE_int : public EncLayer {
public:
    OPE_int(Create_field *, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return serial_pack(level(), name(), key);}
    OPE_int(const std::string & serial);

    SECLEVEL level() {return SECLEVEL::OPE;}
    string name() {return "OPE_int";}
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV);
    Item * decrypt(Item * c, uint64_t IV);


private:
    std::string key;
    OPE ope;
    static const size_t key_bytes = 16;
    static const size_t plain_size = 4;
    static const size_t ciph_size = 8;

};


class OPE_str : public EncLayer {
public:
    OPE_str(Create_field *, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return serial_pack(level(), name(), key);}
    OPE_str(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::OPE;}
    string name() {return "OPE_str";}
    Create_field * newCreateField(Create_field * cf, std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV = 0);
    Item * decrypt(Item * c, uint64_t IV = 0)__attribute__((noreturn));

private:
    std::string key;
    OPE ope;
    static const size_t key_bytes = 16;
    static const size_t plain_size = 4;
    static const size_t ciph_size = 8;
};


EncLayer *
OPEFactory::create(Create_field * cf, std::string key) {
    if (IsMySQLTypeNumeric(cf->sql_type)) { // the ope case as well 
	 return new OPE_int(cf, key);
     } else {
	 return new OPE_str(cf, key);
     }
}

EncLayer *
OPEFactory::deserialize(const std::string & serial) {
    if (get_impl_type(serial) == "OPE_int") {
	return new OPE_int(serial);
    } else {
	return new OPE_str(serial);
    }
}


OPE_int::OPE_int(Create_field * f, string seed_key)
    : key(prng_expand(seed_key, key_bytes)),
      ope(OPE(key, plain_size * 8, ciph_size * 8))
{}

OPE_int::OPE_int(const std::string & serial) :
    key(get_layer_info(serial, level(), name())), ope(OPE(key, plain_size * 8, ciph_size * 8))
{}

Create_field *
OPE_int::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG, anonname);
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
    ulonglong cval = (ulonglong) static_cast<Item_int*>(ctext)->value;
    ulonglong dec = uint64FromZZ(ope.decrypt(ZZFromUint64(cval)));
    LOG(encl) << "OPE_int decrypt " << cval << " IV " << IV << "--->" << dec;

    return new Item_int(dec);
}


OPE_str::OPE_str(Create_field * f, string seed_key)
    : key(prng_expand(seed_key, key_bytes)),
      ope(OPE(key, plain_size * 8, ciph_size * 8))
{}

OPE_str::OPE_str(const std::string & serial) : 
    key(get_layer_info(serial, level(), name())),
    ope(OPE(key, plain_size * 8, ciph_size * 8))
{}

Create_field *
OPE_str::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, -1, MYSQL_TYPE_LONGLONG, anonname, &my_charset_bin);
}

Item *
OPE_str::encrypt(Item * ptext, uint64_t IV) {
    string ps = ItemToString(ptext);
    if (ps.size() < plain_size)
        ps = ps + string(plain_size - ps.size(), 0);

    uint32_t pv = 0;

    for (uint i = 0; i < plain_size; i++) {
	pv = pv * 256 + (int)ps[i];
    }

    ZZ enc = ope.encrypt(to_ZZ(pv));

    return new Item_int((ulonglong) uint64FromZZ(enc));
}

Item *
OPE_str::decrypt(Item * ctext, uint64_t IV) {
    thrower() << "cannot decrypt string from OPE";
}



/**************** HOM ***************************/



HOM::HOM(Create_field * f, string seed_key) :
    seed_key(seed_key)
{
    streamrng<arc4> * prng = new streamrng<arc4>(seed_key);
    sk =  new Paillier_priv(Paillier_priv::keygen(prng, nbits));
    delete prng;
}

HOM::HOM(const std::string & serial): seed_key(serial)
{
    streamrng<arc4> * prng = new streamrng<arc4>(seed_key);
    sk = new Paillier_priv(Paillier_priv::keygen(prng, nbits));
    delete prng;
}

Create_field *
HOM::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, 2*nbits/8, MYSQL_TYPE_BLOB, anonname, &my_charset_bin);
}

static ZZ
ItemIntToZZ(Item * ptext) {
    ulonglong val = static_cast<Item_int *>(ptext)->value;
    return ZZFromUint64(val);
}

static Item *
ZZToItemInt(const ZZ & val) {
    ulonglong v = uint64FromZZ(val);
    return new Item_int(v);
}

static Item *
ZZToItemStr(const ZZ & val) {
    string str = StringFromZZ(val);
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
    ZZ enc = sk->encrypt(ItemIntToZZ(ptext));
    return ZZToItemStr(enc);
}

Item *
HOM::decrypt(Item * ctext, uint64_t IV) {
    ZZ enc = ItemStrToZZ(ctext);
    ZZ dec = sk->decrypt(enc);
    LOG(encl) << "HOM ciph " << enc << "---->" << dec;
    return ZZToItemInt(dec);
}

static udf_func u_sum_a = {
    LEXSTRING("agg"),
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

static udf_func u_sum_f = {
    LEXSTRING("func_add_set"),
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
HOM::sumUDA(Item * expr) {
    List<Item> l;
    l.push_back(expr);
    l.push_back(ZZToItemStr(sk->hompubkey()));
    return new Item_func_udf_str(&u_sum_a, l);
}

Item *
HOM::sumUDF(Item * i1, Item * i2) {
    List<Item> l;
    l.push_back(i1);
    l.push_back(i2);
    l.push_back(ZZToItemStr(sk->hompubkey()));

    return new Item_func_udf_str(&u_sum_f, l);
}

HOM::~HOM() {
    delete sk;
}

/******* SEARCH **************************/

Search::Search(Create_field * f, string seed_key)
{
    key = prng_expand(seed_key, key_bytes);
}

Search::Search(const std::string & serial) {
    key = prng_expand(serial, key_bytes);
}

Create_field *
Search::newCreateField(Create_field * cf, string anonname) {
    return createFieldHelper(cf, -1, MYSQL_TYPE_BLOB, anonname, &my_charset_bin);
}


//returns the concatenation of all words in the given list
static string
assembleWords(list<string> * words)
{
    string res = "";

    for (list<string>::iterator it = words->begin(); it != words->end();
         it++) {
        res = res + *it;
    }

    return res;
}

static string
encryptSWP(const string & key, const list<string> & words)
{
    auto l = SWP::encrypt(key, words);
    string r = assembleWords(l);
    delete l;
    return r;
}

static Token
token(const string & key, const string & word)
{
    return SWP::token(key, word);
}

//this function should in fact be provided by the programmer
//currently, we split by whitespaces
// only consider words at least 3 chars in len
// discard not unique objects
static list<string> *
tokenize(string text)
{

    list<string> tokens = split(text, " ,;:.");

    std::set<string> search_tokens;

    list<string> * res = new list<string>();

    for (list<string>::iterator it = tokens.begin(); it != tokens.end();
         it++) {
        if ((it->length() >= 3) &&
            (search_tokens.find(*it) == search_tokens.end())) {
            string token = toLowerCase(*it);
            search_tokens.insert(token);
            res->push_back(token);
        }
    }

    search_tokens.clear();
    return res;

}

static char *
newmem(const string & a) {
    unsigned int len = a.length();
    char * res = new char[len];
    memcpy(res, a.c_str(), len);
    return res;
}

Item *
Search::encrypt(Item * ptext, uint64_t IV) {
    string plainstr = ItemToString(ptext);
    //TODO: remove string, string serves this purpose now..
    list<string> * tokens = tokenize(plainstr);
    string ciph = encryptSWP(key, *tokens);

    LOG(encl) << "SEARCH encrypt " << plainstr << " --> " << ciph;

    return new Item_string(newmem(ciph), ciph.length(), &my_charset_bin);
}

Item *
Search::decrypt(Item * ctext, uint64_t IV) {
    thrower() << "decryption from SWP not supported \n";
}

static udf_func u_search = {
    LEXSTRING("searchSWP"),
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


static string
searchstrip(string s) {
    cerr << "searchstrip input " << s << "\n";

    if (s[0] == '%') {
	s = s.substr(1, s.length() - 1);
    }
    uint len = s.length();
    if (s[len-1] == '%') {
	s = s.substr(0, len-1);
    }
    cerr << "searchstrip output " << s << "\n";
    return s;
}

Item *
Search::searchUDF(Item * field, Item * expr) {
    List<Item> l = List<Item>();

    l.push_back(field);

    // Add token

    Token t = token(key, string(searchstrip(ItemToString(expr))));
    Item_string * t1 =  new Item_string(newmem(t.ciph),
					t.ciph.length(), &my_charset_bin);
    t1->name = NULL; //no alias
    l.push_back(t1);

    Item_string * t2 = new Item_string(newmem(t.wordKey),
				       t.wordKey.length(), &my_charset_bin);
    t2->name = NULL;
    l.push_back(t2);

    return new Item_func_udf_int(&u_search, l);
}

const std::vector<udf_func*> udf_list = {
    &u_decRNDInt,
    &u_decRNDString,
    &u_decDETInt,
    &u_decDETStr,
    &u_sum_f,
    &u_sum_a,
    &u_search
};
