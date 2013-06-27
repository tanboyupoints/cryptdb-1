/*
 * Author: ccarvalho
 * Jun 25 19:44:52 BRT 2013
 *
 * TODO/Missing impl. list:
 *
 * - Parse ALL fields in STRUCTURE. Some of then are missing.
 * - Get rid of descriptive text that mysqldump prints.
 * - Implement query creation.
 * - Cleanup code and interfaces, get rid of some of then.
 * - Simplify loadXmlStructure(): Way too far obfuscate.
 * - Improve code reuse: fill*() & write*() functions.
 * - Re-think the whole interface, too many functions.
 * - Re-check xml parser for missing fields.
 * - Error handling mechanism.
 * - Memory leak check.
 * - Use Rewriter.
 * - Use Connect. 
 * - Test, test and test.
 */
#include <string>
#include <stdio.h>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <map>
#include <stdlib.h>
#include <typeinfo>
#include <pthread.h>
#include <getopt.h>

#include <list>
#include <algorithm>

#include <cdb_rewrite.hh>
#include <Connect.hh>
#include <field.h>
#include <cryptdbimport.hh>
    

static int 
createEmptyDB(XMLParser& xml, Connect & conn, string dbname)
{
    string q = "CREATE DATABASE IF NOT EXISTS " + dbname + ";";
    DBResult * dbres;
    
    assert(conn.execute(q, dbres));
    if(!dbres)
        return 1;

    return 0;
}

/**
 * Structure fields setup.
 */
void    
XMLParser::fillRIWO(string& dbname, string& tablename, 
            string& fo, tableDataType_e type)
{
    //RIWO->type = type;
    RIWO->fieldVec.push_back(fo);
    RIWO->tablesStructMap[dbname].insert(make_pair(tablename, RIWO->fieldVec));
}

/**
 * Data fields setup.
 */
void 
XMLParser::fillRIWO(string& dbname, string& tablename, 
            string& fo, string& value, tableDataType_e type)
{
    //RIWO->type = type;
    RIWO->dataVecPair.push_back(make_pair(value, fo));
    RIWO->tablesDataMap[dbname].insert(make_pair(tablename, RIWO->dataVecPair));
}


/**
 * Structure fields write out.
 */
int
XMLParser::writeStructureRIWO(void)
{
    // TODO: Format query string, rewrite query, execute query
    // 
    
    //databases
    for(StructDBMap_t::iterator itdb = RIWO->tablesStructMap.begin(); 
            itdb != RIWO->tablesStructMap.end(); ++itdb)
    {
        //tables by database
        for(TableMap_t::iterator ittables = itdb->second.begin(); 
                ittables != itdb->second.end(); ++ittables)
        {

            //fields by table
            for(FieldsVec_t::iterator itdata = ittables->second.begin(); 
                    itdata != ittables->second.end(); ++itdata)
            {
                cout << "[TABLE_STRUCTURE] " << "db: " << itdb->first 
                    << " table: " << ittables->first << " field: " << *itdata << endl;
                //TODO: Ok, we've got almost all necessary fields, now let's create the query,
                //rewrite it and write encrypted query in backend database.
            }
        }
    }

    RIWO->fieldVec.clear();
    RIWO->tablesStructMap.clear();

    return 0;

}


/**
 * Data fields write out.
 */
int
XMLParser::writeDataRIWO(void)
{
    //databases
    for(DataDBMap_t::iterator itdb = RIWO->tablesDataMap.begin(); itdb != RIWO->tablesDataMap.end();
            ++itdb)
    {
        //tables by database
        for(DataTableMap_t::iterator ittables = itdb->second.begin(); 
                ittables != itdb->second.end(); ++ittables)
        {
            // field/value pair by table
            for(ValuesPair_t::iterator itvalues = ittables->second.begin(); itvalues != ittables->second.end();
                    ++itvalues)
            {
                cout << "[TABLE_DATA] " << "db: " << itdb->first 
                    << " table: " << ittables->first << " field: " << itvalues->first << " Value: " << itvalues->second << endl;
                //TODO: Ok, we've got almost all necessary fields, now let's create the query,
                //rewrite it and write encrypted query in backend database.
            }
        }
    }

    RIWO->dataVecPair.clear();
    RIWO->tablesDataMap.clear();

    return 0;
}

// TODO: add sanity checks make it more OO
static void
loadXmlStructure(XMLParser& xml, Connect & conn, Rewriter& r, xmlNode *node)
{
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
            if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"database"))) 
            {
                string dbname("");
                xmlAttrPtr attr = cur_node->properties;
                cout << "[" << attr->children->content << "]" << endl;
                xmlNode *ch = cur_node->xmlChildrenNode;
                dbname = (char*)attr->children->content; 
                
                // create database if not exists
                if(!ch->next) 
                {
                    if(createEmptyDB(xml, conn, dbname) != 0)
                    {
                        // Better error handling in TODO list.
                        cout << "ERROR creating database " << dbname << endl;
                    }
                }           

                while(ch != NULL)
                {
                    // TABLE_STRUCTURE
                    string tablename("");
                    if ((!xmlStrcmp(ch->name, (const xmlChar *)"table_structure"))) 
                    {
                        // Here we create if not exists
                        string tablename("");
                        xmlAttrPtr attr2 = ch->properties;
                        tablename = (char*)attr2->children->content;
                        //cout << "\t" << "[" << dbname << "::" << tablename << "](structure)" << endl;
                        xmlNode *ch2 = ch->xmlChildrenNode;
                        while(ch2 != NULL)
                        {
                            while(ch2->properties != NULL)
                            {
                                xmlAttrPtr attr3 = ch2->properties;
                                string options = (char*)attr3->children->content;
                                if(options.size() > 0)
                                {
                                    //TODO/FIXME: Identify when we are working on filds, keys and options 
                                    string fo = (char*)attr3->children->content;
                                    xml.fillRIWO(dbname, tablename, fo, UNDEFINED);
                                    assert(xml.writeStructureRIWO() != 1);
                                }
                                ch2->properties = ch2->properties->next;
                            }
                            ch2 = ch2->next;
                        }

                    // TABLE_DATA
                    } else if ((!xmlStrcmp(ch->name, (const xmlChar *)"table_data"))) 
                    {
                        // No need to create table here
                        xmlAttrPtr attr2 = ch->properties;
                        string tablename = (char*)attr2->children->content;

                        xmlNode *ch2 = ch->xmlChildrenNode;
                        while(ch2 != NULL)
                        {
                            if((!xmlStrcmp(ch2->name, (const xmlChar *)"row"))) 
                            {
                                xmlNode *ch3 = ch2->xmlChildrenNode;
                                while(ch3 != NULL)
                                {
                                    while(ch3->properties != NULL)
                                    {
                                        xmlAttrPtr attr3 = ch3->properties;

                                        // ROW_VALUE
                                        unsigned char *name = xmlNodeListGetString(xml.getDoc(), ch3->xmlChildrenNode, 1);
                                        string fo("");
                                        if(name)
                                            fo = (char*)name;
                                        string value = (char*)attr3->children->content;
                                        xml.fillRIWO(dbname, tablename, fo, value  , UNDEFINED);
                                        assert(xml.writeDataRIWO() != 1);

                                        ch3->properties = ch3->properties->next;
                                    }
                                    ch3 = ch3->next;
                                }
                            }
                            ch2 = ch2->next;
                        }
                    }
                    ch = ch->next;
                }
            }
        }
        // Recursive
        loadXmlStructure(xml, conn, r, cur_node->children);
    }
}

static void do_display_help(const char *arg)
{
    exit(0);
}

static void 
do_init(XMLParser & xml, Connect & conn, Rewriter& r, const char *filename)
{
    xmlDoc *doc = NULL;
    xmlNode *node = NULL;

    assert(filename != NULL);

    doc = xmlReadFile(filename, NULL, 0);
    assert(doc != NULL);

    xml.setDoc(doc);

    node = xmlDocGetRootElement(xml.getDoc());

    loadXmlStructure(xml, conn, r, node);
    
    //assert(xml.writeStructureRIWO() != 1);

    xmlFreeDoc(xml.getDoc());
}


int main(int argc, char **argv)
{
    int c, threads = 1, optind = 0;
    XMLParser xml;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"show", required_argument, 0, 's'},
        {"password", required_argument, 0, 'p'},
        {"user", required_argument, 0, 'u'},
        {"threads", required_argument, 0, 't'},
        {NULL, 0, 0, 0},
    };

    while(1)
    {
        c = getopt_long(argc, argv, "hs:p:u:t:", long_options, &optind);
        if(c == -1)
            break;

        switch(c)
        {
            //TODO: implement this
            case 'h': 
                do_display_help(argv[0]);
            case 's':
                {
                    ConnectionInfo ci("localhost", xml.getUsername(), xml.getPassword());
                    Rewriter r(ci, "/var/lib/shadow-mysql", false, true);
                    Connect conn("localhost", xml.getUsername(), xml.getPassword(), "");
                    do_init(xml, conn, r, optarg);
                }
                break;
            case 'p':
                xml.setPassword(optarg);
                break;
            case 'u':
                xml.setUsername(optarg);
                break;
            case 't':
                threads = atoi(optarg);
                (void)threads;
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    return 0;
}
