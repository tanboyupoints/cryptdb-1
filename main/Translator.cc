/*
 * Translator.cpp
 *
 *  Created on: Aug 13, 2010
 *      Author: raluca
 */

#include <main/Translator.hh>
#include <util/cryptdb_log.hh>

using namespace std;

string
anonymizeFieldName(unsigned int index, onion o, string origname)
{

    switch (o) {
    case oPLAIN:
	return origname;
    case oDET: {
        return string("field") + strFromVal(index) + "DET";
    }
    case oOPE: {
        return string("field") + strFromVal(index) + "OPE";
    }
    case oAGG: {
        return string("field") + strFromVal(index) + "AGG";
    }
    case oSWP: {
        return string("field") + strFromVal(index) + "SWP";
    }
    default: {assert_s(false, "invalid onion in anonymizeFieldName"); }
    }

    assert_s(false, "invalid control path in anonymizeFieldName");
    return "";
}


string
nextAutoInc(map<string, unsigned int > & autoInc, string fullname)
{
    string val;
    if (autoInc.find(fullname) == autoInc.end()) {
        val = "1";
        autoInc[fullname] = 1;
    } else {
        autoInc[fullname] += 1;
        val = strFromVal(autoInc[fullname]);
    }

    return val;
}

string
getTableSalt(string anonTableName) {
    return BASE_SALT_NAME + "_t_" + anonTableName;
}

bool
isSalt(string id, bool & isTableSalt)
{
    if (id.find(BASE_SALT_NAME) == 0 || (isTableField(id) && (getField(id).find(BASE_SALT_NAME) == 0))) {
        if (id.find(BASE_SALT_NAME+"_t_") == 0) {
            isTableSalt = true;
        } else {
            isTableSalt = false;
        }
        return true;
    }

    return false;
}

string
getTableOfSalt(string salt_name) {

    return salt_name.substr(BASE_SALT_NAME.length() + 3, salt_name.length() - 3 - BASE_SALT_NAME.length());
}



string
getFieldsItSelect(list<string> & words, list<string>::iterator & it)
{
    it = words.begin();
    it++;
    string res = "SELECT ";

    if (equalsIgnoreCase(*it, "distinct")) {
        LOG(edb_v) << "has distinct!";
        it++;
        res += "DISTINCT ";
    }

    return res;
}

/*
 * The following functions return field name and table name.
 * Require the data to be in the format table.field or field.
 *
 */
string
getField(string tablefield)
{
    if (isTableField(tablefield)) {
        size_t pos = tablefield.find(".");
        return tablefield.substr(pos+1, tablefield.length() - pos - 1);
    } else {
        return tablefield;
    }
}

string
getTable(string tablefield)
{
    if (isTableField(tablefield)) {
        size_t pos = tablefield.find(".");
        return tablefield.substr(0, pos);
    } else {
        return "";
    }
}

