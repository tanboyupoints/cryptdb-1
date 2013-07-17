#pragma once

#include <functional>

#include <main/Connect.hh>
#include <main/enum_text.hh>

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
    virtual std::string toString() const = 0;
    virtual std::string getSerial() const = 0;
    template <typename ConcreteKey>
        static ConcreteKey *factory(std::string serial)
    {
        return new ConcreteKey(serial);
    }


protected:
    virtual void serialize() = 0;
    virtual void unserialize() = 0;
};

// TODO: Could use pointer hack so key_data and serial are const.
template <typename KeyType>
class MetaKey : public AbstractMetaKey {
protected:
    // Build MetaKey from serialized MetaKey.
    MetaKey(int dummy, std::string serial)
        : serial(serial)
    {
        unserialize();
    }

    KeyType key_data;
    std::string serial;

public:
    // Build MetaKey from 'actual' key value.
    MetaKey(KeyType key_data)
        : key_data(key_data)
    {
        serialize();
    }
    bool operator <(const AbstractMetaKey &rhs) const
    {
        const MetaKey &rhs_key = static_cast<const MetaKey &>(rhs);
        return key_data < rhs_key.key_data;
    }

    bool operator ==(const AbstractMetaKey &rhs) const
    {
        const MetaKey &rhs_key = static_cast<const MetaKey &>(rhs);
        return key_data == rhs_key.key_data;
    }

    KeyType getValue() const {return key_data;}
    std::string getSerial() const {return serial;}

    // FIXME.
    std::string toString() const
    {
        std::ostringstream s;
        s << key_data;
        return s.str();
    }
    
};

// A string key is most common and this class will allow us to clean up
// the instantiaion syntax significantly.
class IdentityMetaKey : public MetaKey<std::string> {
public:
    IdentityMetaKey(std::string key_data)
        : MetaKey<std::string>(key_data) {}

private:
    virtual void serialize()
    {
        serial = key_data;
    }

    virtual void unserialize()
    {
        key_data = serial;
    }
};

class OnionMetaKey : public MetaKey<onion> {
public:
    OnionMetaKey(std::string serial)
        : MetaKey<onion>(1, serial) {}
    OnionMetaKey(onion key_data)
        : MetaKey<onion>(key_data) {}

private:
    virtual void serialize()
    {
        serial = TypeText<onion>::toText(key_data);
    }

    virtual void unserialize()
    {
        key_data = TypeText<onion>::toType(serial);
    }
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

/*
 * DBBasicMeta is also a design choice about how we use Deltas.
 * i) Read SchemaInfo from database, read Deltaz from database then
 *  apply Deltaz to in memory SchemaInfo.
 *  > How/When do we get an ID the first time we put something into the
 *    SchemaInfo?
 *  > Would likely require a function DBMeta::applyDelta(Delta *)
 *    because we don't have singluarly available interfaces to change
 *    a DBMeta from the outside, ie addChild/replaceChild/destroyChild.
 * ii) Apply Deltaz to SchemaInfo while all is still in database, then
 *  read SchemaInfo from database.
 *  > Logic is in SQL.
 */

class DBWriter;

class DBMeta : public DBObject {
public:
    DBMeta() {}
    virtual ~DBMeta() {;}
    // FIXME: Use rtti.
    virtual std::string typeName() const = 0;
    virtual std::vector<DBMeta *> fetchChildren(Connect *e_conn) = 0;

protected:
    std::vector<DBMeta *>
        doFetchChildren(Connect *e_conn, DBWriter dbw,
                        std::function<DBMeta *(std::string, std::string)>
                          deserialHandler);
};

class LeafDBMeta : public DBMeta {
public:
    virtual std::vector<DBMeta *> fetchChildren(Connect *e_conn)
    {
        return std::vector<DBMeta *>();
    }
};

class KeyedDBMeta : public DBMeta {
public:
    KeyedDBMeta() {}
    virtual ~KeyedDBMeta() {;}

    virtual bool addChild(AbstractMetaKey *key, DBMeta *meta);
    virtual bool replaceChild(AbstractMetaKey *key, DBMeta *meta);
    virtual bool destroyChild(AbstractMetaKey *key);
    virtual bool childExists(AbstractMetaKey * key) const;
    virtual DBMeta *getChild(AbstractMetaKey * key) const;

    std::map<AbstractMetaKey *, DBMeta *> children;

private:
    // Helpers.
    std::map<AbstractMetaKey *, DBMeta *>::const_iterator
        findChild(AbstractMetaKey *key) const;
    AbstractMetaKey *getKey(const DBMeta *const child) const;
};

// > TODO: Make getDatabaseID() protected by templating on the Concrete type
//   and making it a friend.
// > TODO: Use static deserialization functions for the derived types so we
//   can get rid of the <Constructor>(std::string serial) functions and put
//   'const' back on the members.
template <typename ChildType, typename KeyType>
class AbstractMeta : public KeyedDBMeta {
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
    virtual std::vector<DBMeta *> fetchChildren(Connect *e_conn);
};

class DBWriter {
    const std::string child_table;
    const std::string parent_table;

public:
    DBWriter(std::string child_name, std::string parent_name) :
        child_table(child_name), parent_table(parent_name) {}
    DBWriter(DBMeta *child, DBMeta *parent)
        : child_table(child->typeName()), parent_table(parent->typeName())
        {}

    template <typename ChildType>
        static DBWriter factory(DBMeta *parent) {
            auto getChildTypeName = ChildType::instanceTypeName;
            return DBWriter(getChildTypeName(), parent->typeName());
        }

    std::string table_name() {return child_table;}
    std::string join_table_name() {return child_table + "_" + parent_table;}
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

