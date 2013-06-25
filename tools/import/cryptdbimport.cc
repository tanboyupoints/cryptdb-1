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
    
XMLParser::XMLParser(void)
{
    //TODO: implement this
} 


void
XMLParser::loadXmlStructure(xmlNode *node)
{
    xmlNode *cur_node = NULL;
    //xmlChar *key;
    for (cur_node = node; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
            if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"database"))) 
            {
                xmlAttrPtr attr = cur_node->properties;
                cout << "db_name: " << attr->children->content << endl;
                xmlNode *ch = cur_node->xmlChildrenNode;
                if(!ch->properties)
                {
                    //TODO: Table is empty so we just create it
                    cout << "\tdb: " << attr->children->content << " has no properties" << endl;
                }

                while(ch != NULL)
                {
                    // TABLE_STRUCTURE
                    if ((!xmlStrcmp(ch->name, (const xmlChar *)"table_structure"))) 
                    {
                        // Here we create if not exists
                        xmlAttrPtr attr2 = ch->properties;
                        cout << "\ttable_name: " << attr2->children->content << endl;
                        xmlNode *ch2 = ch->xmlChildrenNode;
                        while(ch2 != NULL)
                        {
                            while(ch2->properties != NULL)
                            {
                                xmlAttrPtr attr3 = ch2->properties;
                                if(strlen((char*)attr3->children->content) > 0)
                                {
                                    cout << "\t\ttable_name_field: " << attr3->children->content << endl;
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
                        cout << "\ttable_data: " << attr2->children->content << endl;

                        xmlNode *ch2 = ch->xmlChildrenNode;
                        while(ch2 != NULL)
                        {
                            if ((!xmlStrcmp(ch2->name, (const xmlChar *)"row"))) 
                            {
                                xmlNode *ch3 = ch2->xmlChildrenNode;
                                while(ch3 != NULL)
                                {
                                    while(ch3->properties != NULL)
                                    {
                                        xmlAttrPtr attr3 = ch3->properties;
                                        cout << "\t\ttable_data_row: " << attr3->children->content << endl;
                                        
                                        // ROW_VALUE
                                        unsigned char *name = xmlNodeListGetString(this->getDoc(), ch3->xmlChildrenNode, 1);
                                        if(name)
                                            cout << "\t\t\ttable_data_row_value: " << name << endl;
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
        loadXmlStructure(cur_node->children);
    }
}

static void do_display_help(const char *arg)
{
    exit(0);
}

static void 
do_display_fields(const char *filename)
{
    xmlDoc *doc = NULL;
    xmlNode *node = NULL;

    assert(filename != NULL);

    doc = xmlReadFile(filename, NULL, 0);
    assert(doc != NULL);

    XMLParser xml;
    xml.setDoc(doc);

    node = xmlDocGetRootElement(xml.getDoc());

    xml.loadXmlStructure(node);
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
                do_display_fields(optarg);
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
    //Connect conn("localhost" /* will use this as default for while */, xml.getUsername(), xml.getPassword(), 
    //        "cryptdbtest" /* get this one in run time or NULL if possible */);

    return 0;
}
