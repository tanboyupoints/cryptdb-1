#pragma once

#include <util/util.hh>

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

typedef struct cdb_key {
    
} cdb_key;

//TODO: currently, we have one such object per onion layer per field
// optimize storage by sharing these handlers among objects with same type
class OnionLayer {
public:
    virtual Create_field * layerCreateField(Create_field *) = 0;
    
    virtual Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV = 0) = 0;
    virtual Item * decrypt(cdb_key * key, Item * i, uint64_t IV = 0) = 0;

protected:

};

class OnionLayerRND : OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV = 0);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV = 0);

    std::string decryptUDF(std::string field);
};

class OnionLayerDET : OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV = 0);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV = 0);

    std::string decryptUDF(std::string field);
};

class OnionLayerHOM : OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV = 0);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV = 0);

    std::string sumUDF(cdb_key * key, std::string field);
    
};

class OnionLayerSearch : OnionLayer {
public:
    Create_field * layerCreateField(Create_field *);
    
    Item * encrypt(cdb_key * key, Item * ptext, uint64_t IV = 0);
    Item * decrypt(cdb_key * key, Item * i, uint64_t IV = 0);

    std::string seachUDF(cdb_key * key, std::string field, std::string token);
};

    

  



