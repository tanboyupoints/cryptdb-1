#pragma once

#include <util/util.hh>
#include <crypto/prng.hh>
#include <crypto-old/BasicCrypto.hh>
#include <crypto-old/CryptoManager.hh>
#include <crypto/paillier.hh>
#include <crypto/ope.hh>
#include <crypto/blowfish.hh>
#include <edb/sql_utils.hh>

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

class EncLayer {
 public:
    EncLayer(Create_field * f) : cf(f) {}

    virtual SECLEVEL level() = 0;
    virtual Create_field * newCreateField() = 0;
    
    virtual Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "") = 0;
    virtual Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "") = 0;
    virtual Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "decryptUDF not supported";
    }

 protected:
    Create_field *cf;

    virtual void setKey(const std::string &k) = 0;
    virtual void unSetKey(const std::string &k) = 0;
};


class EncLayerFactory {
public:
    static EncLayer * encLayer(SECLEVEL sl, Create_field * cf, PRNG * key);
};


class RND_int : public EncLayer {
public:
    RND_int(Create_field *, PRNG * key);

    SECLEVEL level() {return SECLEVEL::RND;}
    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol);

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

    SECLEVEL level() {return SECLEVEL::RND;}
    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol);

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

    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField();

    Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol = NULL);

private:
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

    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * = NULL);

private:
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);    
};


class DETJOIN : public EncLayer {
    //TODO: this layer currently does not do encryption
public:
    DETJOIN(Create_field * cf, PRNG * key) : EncLayer(cf) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}
    Create_field * newCreateField() {return cf;}
    
    //TODO: DETJOIN for multi
    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "");
    Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "should not decrypt from joindet\n";
    }

private:
    void setKey(const std::string &k) {};
    void unSetKey(const std::string &k) {};
};


class OPE_int : public EncLayer {
public:
    OPE_int(Create_field *, PRNG * key);

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField();

    Item * encrypt(Item * p, uint64_t IV, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV, const std::string &k = "");

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

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField();

    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "")__attribute__((noreturn));
  
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

    SECLEVEL level() {return SECLEVEL::HOM;}
    Create_field * newCreateField();

    //TODO needs multi encrypt and decrypt
    Item * encrypt(Item * p, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * c, uint64_t IV = 0, const std::string &k = "");

    //expr is the expression (e.g. a field) over which to sum
    Item * sumUDA(Item * expr, const std::string &k = "");
    Item * sumUDF(Item * expr, const std::string &k = "");
private:
    static const uint nbits = 1024;
    Paillier_priv sk;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);
};

class Search : public EncLayer {
public:
    Search(Create_field * cf, PRNG * key);

    SECLEVEL level() {return SECLEVEL::SEARCH;}
    Create_field * newCreateField();

    Item * encrypt(Item * ptext, uint64_t IV = 0, const std::string &k = "");
    Item * decrypt(Item * ctext, uint64_t IV = 0, const std::string &k = "")__attribute__((noreturn));

    //expr is the expression (e.g. a field) over which to sum
    Item * searchUDF(Item * expr);
    
private:
    static const uint key_bytes = 16;
    std::string rawkey;
    Binary key;

    void setKey(const std::string &key);
    void unSetKey(const std::string &key);
};

// XXX these should probably be special methods in HOM above.
extern udf_func s_HomAddUdfFunc;
extern udf_func s_HomSubUdfFunc;

extern const std::vector<udf_func*> udf_list;

