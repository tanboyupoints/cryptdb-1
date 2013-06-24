#pragma once

using namespace std;

class XMLParser 
{
    public:
    XMLParser(); 
    ~XMLParser(){xmlCleanupParser();};

    void printNamesDebug(xmlNode *node);
    void setDoc(xmlDoc *document)
    {
        this->doc = document;
    }
    xmlDoc *getDoc(void) {return this->doc;}

    private:
        xmlDoc *doc; 

        // dbname<tablename<tablefields>>
        map<string, map<string, vector<string>>>dbmap;
};

