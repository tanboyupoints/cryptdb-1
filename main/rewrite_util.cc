#include <main/rewrite_util.hh>

using namespace std;

string
anonymize_table_name(const string &tname,
                     Analysis & a)
{
    TableMeta *tm = a.getTableMeta(tname);
    assert(tm);

    return tm->anonTableName;
}

