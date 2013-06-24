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
#include <field.h>
#include <cryptdbimport.hh>
    
XMLParser::XMLParser(void)
{
    //TODO: implement this
} 

//TODO: rename this function and make it create the objects.
void
XMLParser::printNamesDebug(xmlNode *node)
{
    xmlNode *cur_node = NULL;
    //xmlChar *key;
    for (cur_node = node; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
            //cout << "Element name: " << cur_node->name << endl;
                
            if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"table_structure"))) 
            {
                xmlAttrPtr attr = cur_node->properties;
                cout << "table: " << attr->children->content << endl;
#if 0
                xmlNode *children = cur_node->children;
                while(children != NULL)
                {
                    if ((!xmlStrcmp(children->name, (const xmlChar *)"field"))) {
                        key = xmlNodeListGetString(doc, children->xmlChildrenNode, 1);
                        //printf("type: %d field: %s\n", children->type, key);
                        xmlFree(key);
                    } else if((!xmlStrcmp(children->content, (const xmlChar *)"row")))
                    {
                        key = xmlNodeListGetString(doc, children->xmlChildrenNode, 1);
                        //printf("type: %d row: %s\n", children->type, key);
                        xmlFree(key);
                    }
                    children = children->next;
                }
#endif

            }
            




        }

        printNamesDebug(cur_node->children);
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
    assert(node != NULL);

    xml.printNamesDebug(node);
    xmlFreeDoc(xml.getDoc());
}


int main(int argc, char **argv)
{
    int c, threads = 1, optind = 0;
    string username("");
    string password("");

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
                password = optarg;
                break;
            case 'u':
                username = optarg;
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
