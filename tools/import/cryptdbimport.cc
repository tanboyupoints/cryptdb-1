/*
 * Author: ccarvalho
 * Jun 25 19:44:52 BRT 2013
 *
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


/*
 * TODO:FIXME:
 *
 * The next two static functions 'format_create_database_query' and
 *  'format_insert_table_query' are inconsistent. Both are parsing queries
 *  based on unnecessary too much complex schemas.
 *
 * - 'format_create_database_query' is using a "heavy" std::map to
 *   locate some of its _own_ fields. It should instead be called from
 *   XML loop and execute query by query and not keeping track of data.
 *
 * - 'format_insert_table_query' is the same case though its map is lighter.
 *
 *   Both funcions are error prone and some queries are not being well
 *   formatted and some SQL syntax mistakes made by me.
 *
 *   To execute this program only to stdout queries and not actually executing them:
 *
 *   ./obj/tools/import/cryptdbimport -u root -p letmein -n -sqlfile_name(dump sql file)
 *
 *   Help:  ./obj/tools/import/cryptdbimport -h
 */

static pthread_mutex_t __attribute__((unused))
    _mutex = PTHREAD_MUTEX_INITIALIZER;

static string
format_create_database_query(const string dbname,
        const string tablename, table_structure ts, tsMap_t& tsmap)
{
    assert(tsmap.size() > 0);

    ostringstream q;
    q << "CREATE TABLE IF NOT EXISTS " << dbname << "." << tablename << " (";

    vector<string>fieldVec;
    vector<string>optionsVec;

    int i = 0;
    for(tsIt_t it = tsmap.begin(); it != tsmap.end(); ++it, ++i)
    {
        pairKeys_t p0 = it->first;
        size_t vpsiz = it->second.size();

        if(p0.first == "field")
        {
            string s = p0.second + " ";
            fieldVec.push_back(s);

            // TODO:FIXME: Weird logic, check these if's later;
            if(vpsiz > 0)
            {
                //Type
                fieldVec.push_back(it->second.at(1).second);
                fieldVec.push_back("_fg_");
            }

            if(vpsiz < 2)
            {
                //Null
                if(it->second.at(2).second == "NO")
                {
                    fieldVec.push_back(" NOT NULL");
                    fieldVec.push_back("_fg_");
                }
            }

            if(vpsiz < 3)
            {
                //Key
                if(s == "PRI")
                {
                    s = " PRIMARY KEY(" + p0.second + ")";
                    fieldVec.push_back(it->second.at(3).second);
                    fieldVec.push_back("_fg_");
                }else if(s == "MUL")
                {
                    s = " MULTIPLE KEY(" + p0.second + ")";
                    fieldVec.push_back(it->second.at(3).second);
                    fieldVec.push_back("_fg_");
                }else if(s == "UNI")
                {
                    s = " UNIQUE KEY(" + p0.second + ")";
                    fieldVec.push_back(it->second.at(3).second);
                    fieldVec.push_back("_fg_");
                }
            }
            if(vpsiz < 4)
            {
                //Default
                string tmp = "DEFAULT '" + it->second.at(4).second + "'";
                fieldVec.push_back(tmp);
                fieldVec.push_back("_fg_");
            }

            if(vpsiz < 5)
            {
                //Extra
                fieldVec.push_back(it->second.at(5).second);
                fieldVec.push_back("_fg_");
            }

            if(vpsiz < 6)
            {
                //Comment
                fieldVec.push_back(it->second.at(6).second );
                fieldVec.push_back("_fg_");
            }

        } else if(p0.first == "key")
        {
        //TODO: Extend to get all keys
        } else if(p0.first == "options")
        {
            // Engine
            assert(vpsiz > 0);
            string s = ") ENGINE=" + it->second.at(1).second + ";";
            optionsVec.push_back(s);
            //TODO: Extend to get all options
        }
    }

    for(uint i = 0; i < fieldVec.size(); ++i)
    {
        if(fieldVec.at(i) == "_fg_" && i != fieldVec.size()-1)
        {
            q << ", ";
            continue;
        }
        if(fieldVec.at(i) != "_fg_")
            q << fieldVec.at(i) << " ";

        if(i < fieldVec.size()-1 && i > 0)
            if(fieldVec.at(i) == "_fg_")
                q << ", ";
    }

    for(uint i = 0; i < optionsVec.size(); ++i)
    {
        q << optionsVec.at(i);
    }

    fieldVec.clear();
    optionsVec.clear();
    return q.str();
}

static string
format_insert_table_query(const string dbname,
        const string tablename, table_structure ts, table_data td)
{
    tdVec_t data = td.get_data();

    ostringstream q;
    q << "INSERT INTO " << dbname << "." << tablename << " VALUES(";
    for(tdVec_t::iterator it = data.begin(); it != data.end(); ++it)
    {
        char *p;
        strtol(it->second.c_str(), &p, 10);

        if(it->second.size() == 0)
        {
            q << "NULL";
        }else
        {
            if(*p)
                q << "'" << it->second << "'";
            else
                q << it->second;
        }
        if (it+1 == data.end())
            break;

        q << ", ";

    }
    q << ");";

    return q.str();
}
static int
createEmptyDB(XMLParser& xml, Connect & conn, const string dbname)
{
    string q = "CREATE DATABASE IF NOT EXISTS " + dbname + ";";
    DBResult * dbres;

    //TODO/FIXME: Use executeQuery() instead.
    // I am not using it because of 'Unexpected Error: unhandled sql command 36'
    assert(conn.execute(q, dbres));
    if(!dbres)
        return 1;

    return 0;
}


/**
 * Structure fields write out.
 */
bool
XMLParser::writeRIWO(const string& dbname, const string& tablename,
        Rewriter& r, Connect& conn, bool exec, table_structure& ts)
{
    assert(dbname.size() > 0);
    assert(tablename.size() > 0);

    tsMap_t tsmap = ts.get_data();
    string q = format_create_database_query(dbname, tablename, ts, tsmap);
    cout << q << endl;

    if(exec == true)
        return (bool)executeQuery(conn, r, q, true);

    return true;
}


/**
 * Data fields write out.
 */
bool
XMLParser::writeRIWO(const string& dbname, const string& tablename,
        Rewriter& r, Connect& conn, bool exec, table_structure& ts, table_data& td)
{
    assert(dbname.size() != 0);
    assert(tablename.size() != 0);

    if(td.get_size() == 0)
        return false;

    string q = format_insert_table_query(dbname, tablename, ts, td);
    td.clear();

    cout << q << endl;

    //DBResult * dbres;
    if(exec == true)
        return (bool)executeQuery(conn, r, q, true);

    return true;
}

static void
loadXmlStructure(XMLParser& xml, Connect & conn, Rewriter& r, bool exec, xmlNode *node)
{
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            // TODO: use XML native types to avoid casting
            if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"database")))
            {
                // DATABASE
                xmlAttrPtr attr = cur_node->properties;
                xmlNode *ch = cur_node->xmlChildrenNode;
                string dbname = (char*)attr->children->content;

                // create database if not exists
                if(!ch->next && exec == true)
                {
                    if(createEmptyDB(xml, conn, dbname) != 0)
                    {
                        throw runtime_error(string("Error creating empty database: ") +
                                string(__PRETTY_FUNCTION__));
                    }
                }

                while(ch != NULL)
                {
                    table_structure ts;
                    table_data td;

                    // table_structure
                    if ((!xmlStrcmp(ch->name, (const xmlChar *)"table_structure")))
                    {
                        // Here we create if not exists
                        xmlAttrPtr attr2 = ch->properties;
                        string tablename = (char*)attr2->children->content;
                        xmlNode *ch2 = ch->xmlChildrenNode;
                        while(ch2 != NULL)
                        {
                            keyMNG mng;
                            ident_t ident = (char*)ch2->name;
                            while(ch2->properties != NULL)
                            {
                                prop_t prop = (char*)ch2->properties->name;

                                xmlAttrPtr attr3 = ch2->properties;
                                assert(attr3 != NULL);

                                value_t value = (char*)attr3->children->content;
                                ts.add(make_pair(ident, mng.getKey(value)), make_pair(prop, value));
                                ch2->properties = ch2->properties->next;
                            }
                            mng.unsetKey();
                            ch2 = ch2->next;
                        }
                        // Write out
                        if(xml.writeRIWO(dbname, tablename, r, conn, exec, ts) == false)
                        {
                            //TODO/FIXME: ignoring this error for while.
                            // throw is here in case we find such case.
                            //throw runtime_error(string("Parsing error ?! ") +
                            //    string(__PRETTY_FUNCTION__));
                        }
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
                                        unsigned char *_val = xmlNodeListGetString(xml.doc, ch3->xmlChildrenNode, 1);

                                        value_t value("");
                                        if(_val)
                                            value = (char*)_val;
                                        ident_t ident = (char*)attr3->children->content;
                                        td.add(ident, value);

                                        ch3->properties = ch3->properties->next;
                                    }
                                    ch3 = ch3->next;
                                }
                            }
                            ch2 = ch2->next;
                            // Write out
                            if(xml.writeRIWO(dbname, tablename, r, conn, exec, ts, td) == false)
                            {
                                //cout << "Info: " << dbname << "::" << tablename
                                //    << " has table_structure but table_data is empty." << endl;
                            }
                        }
                    }
                    ch = ch->next;
                }
            }
        }
        loadXmlStructure(xml, conn, r, exec, cur_node->children);
    }
}

static void do_display_help(const char *arg)
{
    cout << "CryptDBImport" << endl;
    cout << "Use: " << arg << " [OPTIONS]" << endl;
    cout << "OPTIONS are:" << endl;
    cout << "-u<username>: MySQL server username" << endl;
    cout << "-p<password>: MySQL server password" << endl;
    cout << "-n: Do not execute queries. Only show stdout." << endl;
    cout << "-s<file>: MySQL's .sql dump file, originated from \"mysqldump\" tool." << endl;
    cout << "To generate DB's dump file use mysqldump, e.g.:" << endl;
    cout << "$ mysqldump -u root -pletmein --all-databases --xml  --no-tablespaces --skip-comments --complete-insert" << endl;
    exit(0);
}

static void
do_init(XMLParser & xml, Connect & conn, Rewriter& r, bool exec, const char *filename)
{
    xmlDoc *doc = NULL;
    xmlNode *node = NULL;

    assert(filename != NULL);
    doc = xmlReadFile(filename, NULL, 0);
    assert(doc != NULL);


    xml.doc = doc;
    node = xmlDocGetRootElement(xml.doc);
    loadXmlStructure(xml, conn, r, exec, node);
    xmlFreeDoc(xml.doc);
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
        {"noexec", required_argument, 0, 'n'},
        {"threads", required_argument, 0, 't'},
        {NULL, 0, 0, 0},
    };

    string username("");
    string password("");
    bool exec = true;
    while(1)
    {
        c = getopt_long(argc, argv, "hs:p:u:t:n", long_options, &optind);
        if(c == -1)
            break;

        switch(c)
        {
            case 'h':
                do_display_help(argv[0]);
            case 's':
                {
                    ConnectionInfo ci("localhost", username, password);
                    Rewriter r(ci, "/var/lib/shadow-mysql", false, true);
                    Connect conn("localhost", username, password, "cryptdbtest");
                    do_init(xml, conn, r, exec, optarg);
                }
                break;
            case 'p':
                password = optarg;
                break;
            case 'u':
                username = optarg;
                break;
            case 't':
                threads = atoi(optarg);
                (void)threads;
                break;
            case 'n':
                exec = false;
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    return 0;
}
