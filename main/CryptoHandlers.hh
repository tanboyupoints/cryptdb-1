#pragma once

#include <util/util.hh>
#include <crypto/prng.hh>
#include <crypto/BasicCrypto.hh>
#include <crypto/paillier.hh>
#include <crypto/ope.hh>
#include <crypto/blowfish.hh>
#include <parser/sql_utils.hh>
#include <crypto/SWPSearch.hh>

#include <sql_select.h>
#include <sql_delete.h>
#include <sql_insert.h>
#include <sql_update.h>


// An enc layer encrypts and decrypts data for a certain onion layer. It also 
// knows how to transform the data type of some plain data to the data type of
// encrypted data in the DBMS.
//TODO: currently, we have one such object per onion layer per field
// optimize storage by sharing these handlers among objects with same type
//TODO: need to implement cleanup & destructors
//TODO(burrows): Make getKey virtual.


class EncLayer {
 public:
    EncLayer(Create_field * f) : cf(f) {}
    EncLayer(Create_field * f, std::string key) : cf(f) {}

    virtual SECLEVEL level() = 0;
    virtual Create_field * newCreateField(std::string anonname = "") = 0;
    
    virtual Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "") = 0;
    virtual Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "") = 0;
    virtual Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "decryptUDF not supported";
    }

    virtual std::string getKey() {return std::string("unsupported");}

 protected:
    Create_field *cf;
    virtual void setKey(const std::string &k) = 0;
    virtual void unSetKey(const std::string &k) = 0;
};

class RND_int : public EncLayer {
public:
    RND_int(Create_field *, PRNG * key);
    RND_int(Create_field *cf, std::string key) : EncLayer(cf), key(key), bf(key) {}

    SECLEVEL level() {return SECLEVEL::RND;}
    Create_field * newCreateField(std::string anonname = "");
    
    Item * encrypt(Item * ptext, uint64_t IV, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol);

    std::string getKey() {return key;}

private:
    std::string key;
    blowfish bf;
    static const int key_bytes = 16;    
    static const int ciph_size = 8;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};

class RND_str : public EncLayer {
public:
    RND_str(Create_field *, PRNG * key);
    RND_str(Create_field *cf, std::string key) : EncLayer(cf) {setKey(key);}

    SECLEVEL level() {return SECLEVEL::RND;}
    Create_field * newCreateField(std::string anonname = "");
    
    Item * encrypt(Item * ptext, uint64_t IV, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol);

    std::string getKey() {return rawkey;}

private:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};

class DET_int : public EncLayer {
public:
    DET_int(Create_field *, PRNG * key);
    DET_int(Create_field *cf, std::string key) : EncLayer(cf), key(key), bf(key) {}

    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol = NULL);

    std::string getKey() {return key;}

protected:
    std::string key;
    blowfish bf;
    static const int bf_key_size = 16;
    static const int ciph_size = 8;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};

class DET_str : public EncLayer {
public:
    DET_str(Create_field *, PRNG * key);
    DET_str(Create_field *cf, std::string key) : EncLayer(cf) {setKey(key);}

    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField(std::string anonname = "");
    
    Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * = NULL);

    std::string getKey() {return rawkey;}

protected:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};


class DETJOIN_int : public DET_int {
    //TODO: this layer currently does not do encryption
public:
    DETJOIN_int(Create_field * cf, PRNG * key) : DET_int(cf, key) {
	this->key="joinjoinjoinjoin";
	setKey(this->key);}
    DETJOIN_int(Create_field *cf, std::string key) : DET_int(cf, key) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}
    
    std::string getKey() {return key;}

/*    Create_field * newCreateField(std::string anonname = "") {return cf;}
    
    //TODO: DETJOIN for multi
    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "should not decrypt from joindet\n";
    }

private:
    void setKey(const std::string &k) {};
    void unSetKey(const std::string &k) {};

*/
};


class DETJOIN_str : public DET_str {
    //TODO: this layer currently does not do encryption
public:
    DETJOIN_str(Create_field * cf, PRNG * key) : DET_str(cf, key) {
	this->rawkey="joinjoinjoinjoin";
	setKey(this->rawkey);
    }

    DETJOIN_str(Create_field *cf, std::string key) : DET_str(cf, key) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}

    std::string getKey() {return rawkey;}

/*    Create_field * newCreateField(std::string anonname = "") {return cf;}
    
    //TODO: DETJOIN for multi
    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "should not decrypt from joindet\n";
    }

private:
    void setKey(const std::string &k) {};
    void unSetKey(const std::string &k) {};

*/
};


class OPE_int : public EncLayer {
public:
    OPE_int(Create_field *, PRNG * key);
    OPE_int(Create_field *cf, std::string key) : EncLayer(cf), key(key), ope(key, plain_size*8, ciph_size*8) {}

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV, const std::string &k = "");

    std::string getKey() {return key;}

private:
    std::string key;
    OPE ope;
    static const size_t key_bytes = 16;
    static const size_t plain_size = 4;
    static const size_t ciph_size = 8;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};


class OPE_str : public EncLayer {
public:
    OPE_str(Create_field *, PRNG * key);
    OPE_str(Create_field *cf, std::string key) : EncLayer(cf), key(key), ope(key, plain_size*8, ciph_size*8) {setKey(key);}

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "")__attribute__((noreturn));
  
    std::string getKey() {return key;}

private:
    std::string key;
    OPE ope;
    static const size_t key_bytes = 16;
    static const size_t plain_size = 4;
    static const size_t ciph_size = 8;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};


class HOM : public EncLayer {
public:
    HOM(Create_field * cf, PRNG * key);
    // HACK(burrows).
    HOM(Create_field *cf, std::string key) : EncLayer(cf), dummy_prng(new urandom()), sk(Paillier_priv::keygen(dummy_prng, nbits)) {throw "HOM does not support persistence!";}

    SECLEVEL level() {return SECLEVEL::HOM;}
    Create_field * newCreateField(std::string anonname = "");

    //TODO needs multi encrypt and decrypt
    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "");

    //expr is the expression (e.g. a field) over which to sum
    Item * sumUDA(Item * expr, const std::string &k = "");
    Item * sumUDF(Item * i1, Item * i2, const std::string &k = "");

private:
    static const uint nbits = 1024;
    PRNG *dummy_prng;
    Paillier_priv sk;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);
};

class Search : public EncLayer {
public:
    Search(Create_field * cf, PRNG * key);
    Search(Create_field *cf, std::string key) : EncLayer(cf) {setKey(key);}

    SECLEVEL level() {return SECLEVEL::SEARCH;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "")__attribute__((noreturn));

    //expr is the expression (e.g. a field) over which to sum
    Item * searchUDF(Item * field, Item * expr);
    
    // FIXME(burrows): rawkey or key?
    std::string getKey() {return rawkey;}

private:
    static const uint key_bytes = 16;
    std::string rawkey;
    std::string key;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);
};


extern const std::vector<udf_func*> udf_list;

template <typename type>
class EncLayerFactory {
public:
    static EncLayer * encLayer(onion o, SECLEVEL sl, Create_field * cf,
                               type key)
    {
        switch (sl) {
            case SECLEVEL::RND: {
                if (IsMySQLTypeNumeric(cf->sql_type) || (o == oOPE)) {
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
                if (IsMySQLTypeNumeric(cf->sql_type)) {
                    return new DETJOIN_int(cf, key);
                } else {
                    return new DETJOIN_str(cf, key);
                }
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

    // static EncLayer * encLayer(onion o, SECLEVEL sl, Create_field * cf, std::string key);
};


