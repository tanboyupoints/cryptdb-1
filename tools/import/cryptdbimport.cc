/*
 * Author: ccarvalho
 * Jun 25 19:44:52 BRT 2013
 *
 * TODO/Missing impl. list:
 *
 * - Parse ALL fields in STRUCTURE. Some of then are missing.
 * - Get rid of descriptive text that mysqldump prints.
 * - Implement query creation.
 * - Simplify loadXmlStructure(): Way too far obfuscate.
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
 * Structure fields write out.
 */
int
XMLParser::writeRIWO(string& dbname, string& tablename, 
        tsPacket_t ts)
{
    //TODO: implement this
    assert(dbname.size() > 0);
    assert(tablename.size() > 0);
    assert(ts.size() > 0);

    for(tsIt it = ts.begin(); it != ts.end(); ++it)
    {
        switch(it->first)
        {
            case FIELD:
                {
                    cout << "[TABLE_STRUCTURE] " << "db: " << dbname 
                        << " table: " << tablename << " <field> " << "prop: " << 
                        it->second.first << " value: " << it->second.second << endl;
                }
                break;
            case OPTIONS:
                {
                    cout << "[TABLE_STRUCTURE] " << "db: " << dbname 
                        << " table: " << tablename << " <options> " << "prop: " << 
                        it->second.first << " value: " << it->second.second << endl;
                }
                break;
            default:
                throw runtime_error(string("Parsing error: ") + 
                    string(__PRETTY_FUNCTION__));
                abort();
        }
    }
    return 0;
}


/**
 * Data fields write out.
 */
int
XMLParser::writeRIWO(string& dbname, string& tablename, 
        dataPacket_t td)
{
    //TODO: implement this
    assert(dbname.size() > 0);
    assert(tablename.size() > 0);
    assert(td.size() > 0);
    
    for(dataIt it = td.begin(); it != td.end(); ++it)
    {
        cout << "[TABLE_DATA] " << "db: " << dbname
            << " table: " << tablename << " field: " << it->first << " Value: " << it->second << endl;
    }
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
                // DATABASE
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
                        throw runtime_error(string("Error creating empty database: ") + 
                                string(__PRETTY_FUNCTION__));
                    }
                }           

                while(ch != NULL)
                {
                    string tablename("");

                    // table_structure
                    tsPacket_t ts; 

                    // table_data
                    dataPacket_t td; 

                    // table_structure
                    if ((!xmlStrcmp(ch->name, (const xmlChar *)"table_structure"))) 
                    {

                        // Here we create if not exists
                        xmlAttrPtr attr2 = ch->properties;
                        tablename = (char*)attr2->children->content;
                        xmlNode *ch2 = ch->xmlChildrenNode;
                        while(ch2 != NULL)
                        {
                            string type = (char*)ch2->name; 
                            while(ch2->properties != NULL)
                            {
                                //prop
                                string prop = (char*)ch2->properties->name;

                                xmlAttrPtr attr3 = ch2->properties;
                                assert(attr3 != NULL);

                                string value = (char*)attr3->children->content;
                                ts.push_back( make_pair(type == "field" ? FIELD : OPTIONS, make_pair(prop, value)));

                                ch2->properties = ch2->properties->next;
                            }
                            ch2 = ch2->next;
                        }
                        // Write out 
                        xml.writeRIWO(dbname, tablename, ts); 

                        // Empty vector, shall not forget.
                        ts.clear();

                    // table_data
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
                                        unsigned char *_val = xmlNodeListGetString(xml.getDoc(), ch3->xmlChildrenNode, 1);
                                        string value("");
                                        if(_val)
                                            value = (char*)_val;
                                        string field = (char*)attr3->children->content;

                                        // Write out on-demand
                                        //assert(xml.writeRIWO(dbname, tablename, field, value) != 1);
                                        td.push_back(make_pair(field, value));

                                        ch3->properties = ch3->properties->next;
                                    }
                                    ch3 = ch3->next;
                                }
                            }
                            ch2 = ch2->next;
                        }
                        // Write out 
                        xml.writeRIWO(dbname, tablename, td); 

                        // Empty vector, shall not forget.
                        td.clear();
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
    // under development
#if 0
    cout << "CryptDBImport - CryptDB's MySQL import database tool" << endl;
    cout << "Use: " << arg << " [OPTIONS]" << endl; 
    cout << "OPTIONS are:" << endl;
    cout << "-u<username>: MySQL server username" << endl;
    cout << "-p<password>: MySQL server password" << endl;
    cout << "-s<file>: MySQL's .sql dump file, originated from \"mysqldump\" tool." << endl;
    cout << "To generate DB's dump file use mysqldump, e.g.:" << endl;
    cout << "$ mysqldump -u root -pletmein --all-databases --xml  --no-tablespaces --skip-comments --complete-insert" << endl;
#endif
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
