#pragma once
/*
 * Author: ccarvalho
 * Jun 25 19:44:52 BRT 2013
 */

using namespace std;

/**
 * Identifier enum
 */
typedef enum _tableIdentifier_e
{
    FIELD = 0,
    OPTIONS,
    UNDEFINED
}tableIdentifier_e;

/** 
 * tsPacket_t is a vector (with pairs) of property + 
 *      field OR options (identifiers) + value.
 */
typedef vector< pair<tableIdentifier_e, 
        pair<string, string>>>tsPacket_t; 
typedef tsPacket_t::iterator tsIt; 

/**
 * dataPacket_t is a vector or pairs 
 *      of field + value.
 */
typedef vector< pair<string, string>>dataPacket_t;
typedef dataPacket_t::iterator dataIt;

/**
 * Import database tool main class.
 */
class XMLParser 
{
    public:
        XMLParser(){
        }
        ~XMLParser(){
            xmlCleanupParser();
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

        int writeRIWO(string& dbname, string& tablename, 
            tsPacket_t ts);
        int writeRIWO(string& dbname, string& tablename, 
            dataPacket_t td);

    private:
        xmlDoc *doc; 
        string username;
        string password;
        Connect *conn;
};

