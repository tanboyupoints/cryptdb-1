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




class OnionLayerHandler {
public:
    Create_field * layerCreateField(Create_field *);
    
    virtual Item * encrypt(Item * i);
    virtual Item * decrypt(Item * i);
private:
    int              field_length;
};




