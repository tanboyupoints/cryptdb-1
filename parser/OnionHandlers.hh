#pragma once

#include <util/util.hh>
#include <crypto/prng.hh>

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
// The naive version of this is likely inefficient (it will require
// performing key setup each time), but it should be possible to
// retrofit a suitable caching scheme once everything else works.
typedef PRNG cdb_key;

//TODO: currently, we have one such object per onion layer per field
// optimize storage by sharing these handlers among objects with same type
class OnionLayer {
public:
    virtual Create_field * layerCreateField(Create_field *) = 0;
    
    virtual Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV = 0) = 0;
    virtual Item * decrypt(cdb_key * key, Item * i, uint64_t IV = 0) = 0;
    virtual Item * decryptUDF(cdb_key * key, Item *columnref, Item *ivcolumnref = 0) {
        thrower() << "decryptUDF not supported";
    }
};

class OnionLayerRND : public OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV);
    Item * decryptUDF(cdb_key *key, Item *columnref, Item *ivcolumnref);
};

class OnionLayerDET : public OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV);
    Item * decryptUDF(cdb_key *key, Item *columnref, Item *ivcolumnref);
};

class OnionLayerHOM : public OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV);
    Item * sumUDF(cdb_key * key, Item *columnref);
};

class OnionLayerSearch : public OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV);
    Item * seachUDF(cdb_key * key, Item *columnref, std::string keyword);
};
