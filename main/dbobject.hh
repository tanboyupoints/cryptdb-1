#pragma once

/*
 * Table Layout
 * > Join table format <objectTableName>_<parentObjectTableName>
 * > Other Data
 *   > EncLayerz.
 *   > Index mappings.
 * 
 *      TableMeta
 * id | serial | delta_id
 * ----------------------
 *    |        |     
 * ----------------------
 *
 *      FieldMeta
 * id | serial | delta_id
 * ----------------------
 *    |        |     
 * ----------------------
 *
 *      OnionMeta
 * id | serial | delta_id
 * ----------------------
 *    |        |     
 * ----------------------
 *  
 *    Delta
 * id | serial
 * -----------
 *    |
 * -----------
 *
 *      TableMeta_SchemaInfo?
 * id | object_id | parent_id | key
 * --------------------------------
 *    |           |           |
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
    // FIXME: Remove default constructor.
    DBObject() {;}
    unsigned int getDatabaseID() const {return id;}
    virtual std::string serialize(const DBObject &parent) const = 0;
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

