#pragma once

using namespace std;

/*
typedef enum _tableDataType_e
{
    FIELDS = 0,
    OPTIONS,
    UNDEFINED
}tableDataType_e;
*/

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
                string& fo);
        int writeRIWO(string& dbname, string& tablename, 
                string& fo, string& value);

    private:
        xmlDoc *doc; 
        string username;
        string password;
        Connect *conn;
};

