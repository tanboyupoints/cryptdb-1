#pragma once

#include <functional>

#include <main/Connect.hh>

/*
 * Table Layout
 * > Join table format <objectTableName>_<parentObjectTableName>
 * > Other Data
 *   > EncLayerz.
 *   > Index mappings.
 * 
 *                 Delta
 * id | action | serial_object | parent_id
 * ---------------------------------------
 *    |        |               |
 * ---------------------------------------

 *   TableMeta
 * id | serial |
 * -------------
 *    |        |
 * -------------
 *
 *   FieldMeta
 * id | serial | 
 * -------------
 *    |        |
 * -------------
 *
 *   OnionMeta
 * id | serial |
 * -------------
 *    |        |
 * -------------
 *  
 *      TableMeta_SchemaInfo
 * id | object_id | parent_id | key
 * --------------------------------
 *    |           |   NULL    |
 * --------------------------------
 *
 *       FieldMeta_TableMeta
 * id | object_id | parent_id | key
 * --------------------------------
 *    |           |           |
 * --------------------------------
 *
 *       OnionMeta_FieldMeta
 * id | object_id | parent_id | key
 * --------------------------------
 *    |           |           |
 * --------------------------------
 *
 */

// FIXME: Maybe should inherit from DBObject.
class AbstractMetaKey {
public:
    AbstractMetaKey() {;}
    virtual ~AbstractMetaKey() {;}
    virtual bool operator <(const AbstractMetaKey &rhs) const = 0;
    virtual bool operator ==(const AbstractMetaKey &rhs) const = 0;
    virtual std::string serialize() const = 0;
    virtual std::string toString() const = 0;
};

template <typename KeyType>
class MetaKey : public AbstractMetaKey {
    typedef std::function<std::string(KeyType)> serializer;
    typedef std::function<KeyType(std::string)> deserializer;
    
    // Build MetaKey from serialized MetaKey.
    MetaKey(int dummy, std::string serial, deserializer toKeyType)
        : key_data(toKeyType(serial)), serial(serial) {}

public:
    const KeyType key_data;
    const std::string serial;

    // Build MetaKey from 'actual' key value.
    MetaKey(KeyType key_data, serializer toStr)
        : key_data(key_data), serial(toStr(key_data)) {}
    bool operator <(const AbstractMetaKey &rhs) const
    {
        MetaKey rhs_key = static_cast<const MetaKey &>(rhs);
        return key_data < rhs_key.key_data;
    }

    bool operator ==(const AbstractMetaKey &rhs) const
    {
        MetaKey rhs_key = static_cast<const MetaKey &>(rhs);
        return key_data == rhs_key.key_data;
    }

    static MetaKey<KeyType> *deserialize(std::string serial,
                                         deserializer toKeyType)
    {
        static const int dummy = 1;
        return new MetaKey<KeyType>(dummy, serial, toKeyType);
    }

    std::string serialize() const
    {
        return serial;
    }

    // FIXME.
    std::string toString() const
    {
        std::ostringstream s;
        s << key_data;
        return s.str();
    }
    
    static std::string identity(std::string s) {
        return s;
    }
};

// A string key is most common and this class will allow us to clean up
// the instantiaion syntax significantly.
class IdentityMetaKey : public MetaKey<std::string> {
public:
    IdentityMetaKey(std::string key_data)
        : MetaKey<std::string>(key_data, MetaKey<std::string>::identity) {}
};

class DBObject {
    // FIXME: Make const.
    unsigned int id;

public:
    // Building new objects.
    explicit DBObject(unsigned int id) : id(id) {}
    // Unserializing old objects.
    explicit DBObject(const std::string &serial)
    {
        throw "Cannot unserialize into DBObject";
    }
    // 0 indicates that the object does not have a database id.
    // This is the state of the object before it is written to the database
    // for the first time.
    DBObject() : id(0) {}
    virtual ~DBObject() {;}
    unsigned int getDatabaseID() const {return id;}
    // FIXME: This should possibly be a part of DBMeta.
    // > Parent will definitely be DBMeta.
    // > Parent-Child semantics aren't really added until DBMeta.
    virtual std::string serialize(const DBObject &parent) const = 0;
};

class DBMeta : public DBObject {
public:
    DBMeta() {}
    virtual ~DBMeta() {}

    virtual bool addChild(AbstractMetaKey *key, DBMeta *meta);
    virtual bool replaceChild(AbstractMetaKey *key, DBMeta *meta);
    virtual bool destroyChild(AbstractMetaKey *key);

    // Helpers.
    std::map<AbstractMetaKey *, DBMeta *>::const_iterator
        findChild(AbstractMetaKey *key) const;
    bool childExists(AbstractMetaKey * key) const;
    DBMeta *getChild(AbstractMetaKey * key) const;
    AbstractMetaKey *getKey(const DBMeta *const child) const;

    // FIXME: Use rtti.
    virtual std::string typeName() const = 0;
    virtual std::vector<DBMeta *> fetchChildren(Connect *e_conn) = 0;

    std::map<AbstractMetaKey *, DBMeta *> children;
};

// > TODO: Make getDatabaseID() protected by templating on the Concrete type
//   and making it a friend.
// > TODO: Use static deserialization functions for the derived types so we
//   can get rid of the <Constructor>(std::string serial) functions and put
//   'const' back on the members.
template <typename ChildType, typename KeyType>
class AbstractMeta : public DBMeta {
public:
    // TODO: Remove default constructor.
    AbstractMeta() {}
    virtual ~AbstractMeta()
    {
        auto cp = children;
        children.clear();

        for (auto it : cp) {
            delete it.second;
        }
    }
    // Virtual constructor to deserialize from embedded database.
    template <typename ConcreteMeta>
        static ConcreteMeta *deserialize(std::string serial);
    std::vector<DBMeta *> fetchChildren(Connect *e_conn);
    // FIXME: If this is too tightly coupled with MetaKey, implement it
    // as a function pointer passed to the constructor.
    virtual KeyType deserializeKey(std::string serialized_key) const = 0;
};

class LeafDBMeta : public DBMeta {
public:
    virtual std::vector<DBMeta *> fetchChildren(Connect *e_conn)
    {
        return std::vector<DBMeta *>();
    }

private:
    bool addChild(AbstractMetaKey *key, DBMeta *meta)
    {
        throw CryptDBError("LeafMeta!");
    }
    bool replaceChild(AbstractMetaKey *key, DBMeta *obj)
    {
        throw CryptDBError("LeafMeta!");
    }
    bool destroyChild(AbstractMetaKey *key)
    {
        throw CryptDBError("LeafMeta!");
    }
};

inline std::string
serialize_string(std::string str)
{
    return std::string(std::to_string(str.length()) + "_" + str);
}

// TESTME.
// Must perserve order.
inline std::vector<std::string>
unserialize_string(std::string serial)
{
    std::vector<std::string> output;
    std::size_t start = 0;
    std::size_t under_pos = serial.find_first_of("_");
    while (under_pos != std::string::npos) {
        std::size_t length =
            atoi(serial.substr(start, under_pos-start).c_str());
        output.push_back(serial.substr(under_pos+1, length)); 
        start = under_pos + 1 + length;
        under_pos = serial.find_first_of("_", start);
    }

    // TODO: Sanity check no leftover characters.

    return output;
}

