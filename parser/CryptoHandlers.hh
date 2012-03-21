#pragma once

#include <util/util.hh>
#include <crypto/prng.hh>
#include <crypto-old/BasicCrypto.hh>

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
    virtual std::string decryptUDF(const std::string & col, const std::string & ivcol = "") {
        thrower() << "decryptUDF not supported";
    }
};

class RND_int : public EncLayer {
public:
    RND_int(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    std::string decryptUDF(const std::string & col, const std::string & ivcol);

private:
    Create_field * cf;
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * key;
    
    static const int ciph_size = 8;
};

class RND_string : public EncLayer {
public:
    RND_string(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    std::string decryptUDF(const std::string & col, const std::string & ivcol);

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
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    std::string decryptUDF(const std::string & col, const std::string & ivcol);

private:
    Create_field * cf;
    std::string rawkey;
    blowfish * key;
    static const int bf_key_size = 16;
    static const int ciph_size = 8;
};

class DET_string : public EncLayer {
public:
    DET_string(Create_field *, PRNG * key);

    Create_field * newCreateField();
    
    Item * encrypt(Item * ptext, uint64_t IV);
    Item * decrypt(Item * ctext, uint64_t IV);
    std::string decryptUDF(const std::string & col, const std::string & ivcol);

private:
    Create_field * cf;
    std::string rawkey;
    static const int key_bytes = 16;
    AES_KEY * enckey;
    AES_KEY * deckey;
};

class EncLayerFactory {
    static EncLayer * encLayer(SECLEVEL sl, Create_field * cf, PRNG * key);
};

/*

class EncLayerHOM : public EncLayer {
public:
    Create_field * createField(Create_field *);

    cdb_key * getKey(PRNG * prng);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV);
    Item * sumUDF(cdb_key * key, Item *columnref);

private:    
    cdb_key * getKey(PRNG * prng);

};

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
