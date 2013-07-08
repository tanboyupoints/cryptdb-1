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


/*
 * Requirements & TODO:
 *  -- want to have RND_int and RND_str specialized
 *  -- only see RND and others in interface
 *  -- need to call HOM->sumUDF and UDA with specific arguments
 *  -- chain create fields
 *  -- anon name should not be in EncLayers
 *  -- RNDFactory are part of implementation
 */

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

    // can be deserialized by corresponding factory
    virtual std::string serialize() = 0;

 protected:
    Create_field *cf;
};


class DET_int : public EncLayer {
public:
    DET_int(Create_field *,  const std::string & seed_key);

    std::string serialize() {return key; }
    // create object from serialized contents
    DET_int(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * ivcol = NULL);


protected:
    std::string key;
    blowfish bf;
    static const int bf_key_size = 16;
    static const int ciph_size = 8;

};

class DET_str : public EncLayer {
public:
    DET_str(Create_field *cf, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return rawkey;}
    DET_str(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::DET;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0);
    Item * decryptUDF(Item * col, Item * = NULL);

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

    // serialize from parent;  unserialize:
    DETJOIN_int(std::string serial) : DET_int(serial) {}


    SECLEVEL level() {return SECLEVEL::DETJOIN;}


};


class DETJOIN_str : public DET_str {
    //TODO
public:
    DETJOIN_str(Create_field * cf, std::string seed_key) : DET_str(cf, seed_key) {}

    // serialize from parent; unserialize:
    DETJOIN_str(std::string serial) : DET_str(serial) {}

    SECLEVEL level() {return SECLEVEL::DETJOIN;}

};


class OPE_int : public EncLayer {
public:
    OPE_int(Create_field *, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return key;}
    OPE_int(const std::string & serial);

    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField(std::string anonname = "");

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
    std::string serialize() {return key;}
    OPE_str(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::OPE;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * p, uint64_t IV = 0);
    Item * decrypt(Item * c, uint64_t IV = 0)__attribute__((noreturn));

private:
    std::string key;
    OPE ope;
    static const size_t key_bytes = 16;
    static const size_t plain_size = 4;
    static const size_t ciph_size = 8;
};


class HOM : public EncLayer {
public:
    HOM(Create_field * cf, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return seed_key;}
    HOM(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::HOM;}
    Create_field * newCreateField(std::string anonname = "");

    //TODO needs multi encrypt and decrypt
    Item * encrypt(Item * p, uint64_t IV = 0);
    Item * decrypt(Item * c, uint64_t IV = 0);

    //expr is the expression (e.g. a field) over which to sum
    Item * sumUDA(Item * expr);
    Item * sumUDF(Item * i1, Item * i2);

private:
    std::string seed_key;
    static const uint nbits = 1024;
    Paillier_priv * sk;

    ~HOM();
};

class Search : public EncLayer {
public:
    Search(Create_field * cf, std::string seed_key);

    // serialize and deserialize
    std::string serialize() {return key;}
    Search(const std::string & serial);


    SECLEVEL level() {return SECLEVEL::SEARCH;}
    Create_field * newCreateField(std::string anonname = "");

    Item * encrypt(Item * ptext, uint64_t IV = 0);
    Item * decrypt(Item * ctext, uint64_t IV = 0)__attribute__((noreturn));

    //expr is the expression (e.g. a field) over which to sum
    Item * searchUDF(Item * field, Item * expr);


private:
    static const uint key_bytes = 16;
    std::string key;

};



extern const std::vector<udf_func*> udf_list;

class EncLayerFactory {
public:
    static EncLayer * encLayer(onion o, SECLEVEL sl, Create_field *cf,
                               std::string key);

    // creates EncLayer from its serialization
    static EncLayer * encLayerFromSerial(onion o, SECLEVEL sl,
					 enum enum_field_types sql_type, const std::string & serial);
};


