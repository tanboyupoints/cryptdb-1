#pragma once

#include <util/util.hh>
#include <crypto/prng.hh>
#include <crypto-old/BasicCrypto.hh>
#include <crypto-old/CryptoManager.hh>
#include <crypto/paillier.hh>

#include <sql_select.h>
#include <sql_delete.h>
#include <sql_insert.h>
#include <sql_update.h>



class OnionTypeHandler {
private:
    int                   field_length;
    enum enum_field_types type;
    CHARSET_INFO *        charset;
public:
    OnionTypeHandler(enum enum_field_types t) :
        field_length(-1), type(t), charset(NULL) {}
    OnionTypeHandler(enum enum_field_types t, size_t f) :
        field_length((int)f), type(t), charset(NULL) {}
    OnionTypeHandler(enum enum_field_types t,
                      size_t f,
                      CHARSET_INFO *charset) :
        field_length((int)f), type(t), charset(charset) {}

    Create_field*
    newOnionCreateField(const char * anon_name,
                        const Create_field *f) const;
    
    Item*
    createItem(const std::string & data);
    
};

// One possible plan: an onion layer key is logically a PRNG that
// generates a suitable key.
//
// In practice, the PRNG would be something like blockrng<AES> based
// on the master AES key, or based on a multiprinc key.
//
// Each onion layer reads the necessary number of bytes from the
// PRNG to generate its key (AES, blowfish, Paillier, SWP, ...)
//
// TODO: The naive version of this is likely inefficient (it will require
// performing key setup each time), but it should be possible to
// retrofit a suitable caching scheme once everything else works.



//TODO: call enclayer? 
//TODO: currently, we have one such object per onion layer per field
// optimize storage by sharing these handlers among objects with same type

//TODO: need to writeup cleanup & destructors

class EncLayer {
public:
    virtual Create_field * newCreateField() = 0;
    
    virtual Item * encrypt(Item * ptext, uint64_t IV = 0) = 0;
    virtual Item * decrypt(Item * i, uint64_t IV = 0) = 0;
    virtual Item * decryptUDF(Item * col, Item * ivcol = NULL) {
        thrower() << "decryptUDF not supported";
    }
};

class RND_int : public EncLayer {
public:
    RND_int(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    Item * decryptUDF(Item * col, Item * ivcol);

private:
    Create_field * cf;
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * key;
    
    static const int ciph_size = 8;
};

class RND_str : public EncLayer {
public:
    RND_str(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    Item * decryptUDF(Item * col, Item * ivcol);

private:
    Create_field * cf;
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;
};

class DET_int : public EncLayer {
public:
    DET_int(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * ivcol = NULL);

private:
    Create_field * cf;
    std::string rawkey;
    blowfish * key;
    static const int bf_key_size = 16;
    static const int ciph_size = 8;
};


class DET_str : public EncLayer {
public:
    DET_str(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * = NULL);

private:
    Create_field * cf;
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;
};



class OPE_int : public EncLayer {
public:
    OPE_int(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);

private:
    Create_field * cf;
    std::string rawkey;
    OPE * key;
    static const int key_bytes = 16;
    static const int plain_size = 4;
    static const int ciph_size = 8;
};


class OPE_str : public EncLayer {
public:
    OPE_str(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0)__attribute__((noreturn));
  
private:
    Create_field * cf;
    std::string rawkey;
    OPE * key;
    static const int key_bytes = 16;
    static const int plain_size = 4;
    static const int ciph_size = 8;
};


class HOM : public EncLayer {
public:
    HOM(Create_field * cf, PRNG * key);

    Create_field * newCreateField();

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);

    //expr is the expression (e.g. a field) over which to sum
    Item * sumUDF(Item * expr);
    
private:
    Create_field * cf;
    static const uint nbits = 1024;
    std::vector<NTL::ZZ> sk;
    Paillier_priv * skP;
    std::vector<NTL::ZZ> pk;
    Paillier * pkP;
};

class Search : public EncLayer {
public:
    Search(Create_field * cf, PRNG * key);
    
    Create_field * newCreateField();

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0)__attribute__((noreturn));

    //expr is the expression (e.g. a field) over which to sum
    Item * searchUDF(Item * expr);
    
private:
    Create_field * cf;
    static const uint key_bytes = 16;
    std::string rawkey;
    Binary key;
};

class EncLayerFactory {
    static EncLayer * encLayer(SECLEVEL sl, Create_field * cf, PRNG * key);
};

/*

class EncLayerSearch : public EncLayer {
public:
    Create_field * createField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV);
    Item * seachUDF(cdb_key * key, Item *columnref, std::string keyword);

private:    
    cdb_key * getKey(PRNG * prng);

};

    cdb_key * getKey(PRNG * prng);
*/
