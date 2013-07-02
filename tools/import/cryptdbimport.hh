#pragma once
/*
 * Author: ccarvalho
 * Jun 25 19:44:52 BRT 2013
 */

using namespace std;


namespace {

typedef string myKey_t;
typedef string myValue_t;
typedef string myTsIdent_t;
typedef string myTsId_t;
typedef string myTdId_t;
typedef string myTsValue_t;
typedef string myTdValue_t;

/**
 * General containers typedef's.
 *
 */
typedef pair<string, string>pairKeys_t;
typedef pair<string, string>tsPairValues_t;
typedef vector<tsPairValues_t>tsVecPairValues_t;


/**
 * table_structure 
 */
typedef myKey_t ident_t;
typedef myKey_t value_t;
typedef myKey_t prop_t;

typedef map<pairKeys_t, tsVecPairValues_t> tsMap_t;
typedef tsMap_t::iterator tsIt_t;
typedef tsVecPairValues_t::iterator tsVpIt_t;

/**
 * TODO/FIXME: Both structures 'table_structure' and 'table_data'
 * are under a serious risk of becaming deprected. We should get _all_
 * fields from XML parser, currently some are missing.
 *
 * table_structure in special is too heavy, later will change the parser's approach
 * and avoid to keep XML's fields, just write'em out e that's it.
 */
struct table_structure
{
    private:
        tsMap_t m_ts;

    public:
        inline void add(pairKeys_t pkeys, tsPairValues_t pvalues)
        {
            m_ts[pkeys].push_back(pvalues);
        }

        inline const string get_field(pairKeys_t pkeys, string prop, tsMap_t ts)
        {
            if(ts.find(pkeys) != ts.end())
            {
                for(tsVpIt_t it = ts[pkeys].begin(); 
                        it != ts[pkeys].end(); ++it)
                {
                    if(it->first == prop)
                    {
                        return it->second;
                    }
                }
            }
            return "";
        }

        inline const tsMap_t get_data(void) {return m_ts;}

        inline void clear(void)
        {
            m_ts.clear();
        }
};

/**
 * table_data
 */
typedef pair<myTdId_t, myTdValue_t>tdPair_t;
typedef vector<tdPair_t>tdVec_t; 
struct table_data
{
    private:
        tdVec_t m_td;
        
    public:
        inline void add(ident_t ident, value_t value)
        {
            m_td.push_back(make_pair(ident, value));
        }
            
        inline void clear(void)
        {
            m_td.clear();
        }

        inline const tdVec_t get_data(void){return m_td;}
        
        inline size_t get_size(void) { return m_td.size();}
};


typedef vector< pair<string, string>>dataPacket_t;

class keyMNG 
{
    public:

        keyMNG() 
            : m_flag(false) 
        {}

        const myKey_t getKey(string key)
        {
            if(!m_flag)
            {
                m_flag = true;
                m_key = key;
            }

            return m_key;
        }
        void unsetKey(void) { m_flag = false;}
        bool m_flag;

    private:
        myKey_t m_key;
};


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

        xmlDoc *doc; 

        vector<string>loadXmlDatabaseName(xmlNode *node);

        int writeRIWO(const string& dbname, const string& tablename, 
            Rewriter& r, Connect& conn, bool exec, table_structure& _ts);
        int writeRIWO(const string& dbname, const string& tablename, 
            Rewriter& r, Connect& conn, bool exec, table_structure& ts, table_data& _td);

    private:
        //string username;
        //string password;
        Connect *conn;
};
};

