#pragma once

using namespace std;

class XMLParser 
{
    public:
    XMLParser(); 
    ~XMLParser(){xmlCleanupParser();};

    void loadXmlStructure(xmlNode *node);
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

    vector<string>loadXmlDatabaseName(xmlNode *node);

    private:
        xmlDoc *doc; 
        string username;
        string password;

        // dbname<tablename<tablefields>>
        map<string, map<string, vector<string>>>dbmap;
};

