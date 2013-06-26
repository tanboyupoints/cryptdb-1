#pragma once

using namespace std;

typedef vector<string> FieldsVec_t;
typedef vector<pair<string, string>> ValuesPair_t;

/**
 * Map holding chain of tables->fields.
 *
 */
typedef map<string , map<string, FieldsVec_t>> StructDBMap_t;

/**
 * Map holding chain of tables->fields->values.
 *
 */
typedef map<string , map<string, ValuesPair_t>> DataDBMap_t; 


typedef map<string, vector<string>> TableMap_t;
typedef map<string, ValuesPair_t> DataTableMap_t;


typedef enum _tableDataType_e
{
    FIELDS = 0,
    OPTIONS,
    UNDEFINED
}tableDataType_e;

typedef struct _readInWriteOut_st
{
    tableDataType_e type;

    //Structure
    FieldsVec_t fieldVec;
    StructDBMap_t tablesStructMap;

    //Data
    ValuesPair_t dataVecPair;
    DataDBMap_t tablesDataMap;

}readInWriteOut_st;

/**
 * Import database tool main class.
 */
class XMLParser 
{
    public:
        XMLParser() {
            RWO = new readInWriteOut_st[sizeof(readInWriteOut_st)]();
            assert(RWO != NULL);
        }
        ~XMLParser(){
            xmlCleanupParser();
            delete [] RWO;
        }

        void setDoc(xmlDoc *document){
            this->doc = document;
        }

        xmlDoc *getDoc(void) {return this->doc;}

        void setPassword(const char *password) { 
            this->password = password;
        }

        string getPassword(void){return this->password;}

        void setUsername(const char *username) { 
            this->username = username;
        }

        string getUsername(void) {return this->username;}

        void setConn(Connect & conn){this->conn = &conn;}
        Connect *getConn(void){return this->conn;}

        vector<string>loadXmlDatabaseName(xmlNode *node);

        void fillRWO(string& dbname, string& tablename, 
                string& fo, tableDataType_e type);  

        void fillRWO(string& dbname, string& tablename, 
                string& fo, string& value, tableDataType_e type);  

        int writeStructureRWO(void);  
        int writeDataRWO(void);

    private:
        readInWriteOut_st *RWO;
        xmlDoc *doc; 
        string username;
        string password;
        Connect *conn;
};

