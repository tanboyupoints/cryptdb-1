#include <edb/sql_utils.hh>



using namespace std;

bool
isTableField(string token)
{
    size_t pos = token.find(".");

    if (pos == string::npos) {
        return false;
    } else {
        return true;
    }
}

string
fullName(string field, string table)
{
    if (isTableField(field)) {
        return field;
    } else {
        return table + "." + field;
    }
}

char *
make_thd_string(const string &s, size_t *lenp)
{
    THD *thd = current_thd;
    assert(thd);
    if (lenp)
        *lenp = s.size();
    return thd->strmake(s.data(), s.size());
}

string
ItemToString(Item * i) {
    assert(i);
    String s;
    String *s0 = i->val_str(&s);
    assert(s0 != NULL);
    string ret = string(s0->ptr(), s0->length());
    return ret;
}
