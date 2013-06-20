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
    //  EncLayer(Create_field * f, std::string key) : cf(f) {}

    virtual SECLEVEL level() = 0;

    // returns a rewritten create field to include in rewritten query
    virtual Create_field * newCreateField(std::string anonname = "") = 0;
    
    virtual Item * encrypt(Item * ptext, uint64_t IV = 0) = 0;
    virtual Item * decrypt(Item * ctext, uint64_t IV = 0) = 0;

    // returns the decryptUDF to remove the onion layer 
    virtual Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "decryptUDF not supported";
    }

    virtual std::string serialize() = 0;
    virtual static EncLayer * deserialize() = 0; 

 protected:
    Create_field *cf;
};

class RND_int : public EncLayer {
public:
    RND_int(Create_field *cf, std::string seed_key);

    SECLEVEL level() {return SECLEVEL::RND;}
    Create_field * newCreateField(std::string anonname = "");
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    Item * decryptUDF(Item * col, Item * ivcol);

    virtual std::string serialize() {return ""; //just key}
    virtual static EncLayer* deserialize() {return NULL;}

private:
    std::string key;
    blowfish bf;
    static const int key_bytes = 16;    
    static const int ciph_size = 8;

};

class RND_str : public EncLayer {
public:
    RND_str(Create_field *, std::string seed_key);

    SECLEVEL level() {return SECLEVEL::RND;}
    Create_field * newCreateField(std::string anonname = "");
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    Item * decryptUDF(Item * col, Item * ivcol);


    std::string serialize() {return ""; //just rawkey}
    static EncLayer* deserialize() {return NULL;}


private:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

};

class DET_int : public EncLayer {
public:
    DET_int(Create_field *, std::string seed_key);

    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * ivcol = NULL);

    std::string serialize() {return ""; /*just key*/ }
    static EncLayer* deserialize() {return NULL;}
    
protected:
    std::string key;
    blowfish bf;
    static const int bf_key_size = 16;
    static const int ciph_size = 8;

};

class DET_str : public EncLayer {
public:
    DET_str(Create_field *, PRNG * key);
    DET_str(Create_field *cf, std::string key) : EncLayer(cf) {setKey(key);}

    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField(std::string anonname = "");
    
    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * = NULL);
    
    std::string serialize() {return ""; /* just rawkey */}
    static EncLayer* deserialize() {return NULL;}


protected:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

};


class DETJOIN_int : public DET_int {
    //TODO
public:
    DETJOIN_int(Create_field * cf, std::string seed_key) : DET_int(cf, seed_key) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}
};


class DETJOIN_str : public DET_str {
    //TODO
public:
    DETJOIN_str(Create_field * cf, std::string seed_key) : DET_str(cf, seed_key) {}    }

    SECLEVEL level() {return SECLEVEL::DETJOIN;}

};


class OPE_int : public EncLayer {
public:
    OPE_int(Create_field *, std::string seed_key);
  

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV);
    Item * decrypt(Item * c, uint64_t IV);

    std::string serialize() {return ""; /* just key */}
    static EncLayer* deserialize() {return NULL;}

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

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV = 0);
    Item * decrypt(Item * c, uint64_t IV = 0)__attribute__((noreturn));
  
    std::string serialize() {return ""; /* just key */}
    static EncLayer* deserialize() {return NULL;}

private:
    std::string key;
    OPE ope;
    static const size_t key_bytes = 16;
    static const size_t plain_size = 4;
    static const size_t ciph_size = 8;
};


class HOM : public EncLayer {
public:
    HOM(Create_field * cf, std::string key);
    
    SECLEVEL level() {return SECLEVEL::HOM;}
    Create_field * newCreateField(std::string anonname = "");

    //TODO needs multi encrypt and decrypt
    Item * encrypt(Item * p, uint64_t IV = 0);
    Item * decrypt(Item * c, uint64_t IV = 0);
    
    std::string getKey() {throw "Does not support getKey()";};

    //expr is the expression (e.g. a field) over which to sum
    Item * sumUDA(Item * expr);
    Item * sumUDF(Item * i1, Item * i2);

private:
    static const uint nbits = 1024;
    PRNG *dummy_prng;
    Paillier_priv sk;

};

class Search : public EncLayer {
public:
    Search(Create_field * cf, PRNG * key);
    Search(Create_field *cf, std::string key) : EncLayer(cf) {setKey(key);}

    SECLEVEL level() {return SECLEVEL::SEARCH;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0)__attribute__((noreturn));

    //expr is the expression (e.g. a field) over which to sum
    Item * searchUDF(Item * field, Item * expr);
    
    // FIXME(burrows): rawkey or key?
    std::string getKey() {return rawkey;}

private:
    static const uint key_bytes = 16;
    std::string rawkey;
    std::string key;

};


extern const std::vector<udf_func*> udf_list;

template <typename type>
class EncLayerFactory {
public:
    static EncLayer * encLayer(onion o, SECLEVEL sl, Create_field *cf,
                               type key);
};


