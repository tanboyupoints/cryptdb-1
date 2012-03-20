#include <parser/OnionHandlers.hh>


using namespace std;

//TODO: this is duplicated in cdb_rewrite
static char *
make_thd_string(const string &s, size_t *lenp = 0)
{
    THD *thd = current_thd;
    assert(thd);

    if (lenp)
        *lenp = s.size();
    return thd->strmake(s.data(), s.size());
}


Create_field*
OnionTypeHandler::newOnionCreateField(const char * anon_name,
		    const Create_field *f) const {
    THD *thd = current_thd;
    Create_field *f0 = f->clone(thd->mem_root);
    f0->field_name = thd->strdup(anon_name);
    if (field_length != -1) {
	f0->length = field_length;
    }
    f0->sql_type = type;
    
    if (charset != NULL) {
	f0->charset = charset;
    } else {
	//encryption is always unsigned
	f0->flags = f0->flags | UNSIGNED_FLAG; 
    }
    return f0;
}

Item*
OnionTypeHandler::createItem(const string & data) {
    switch (type) {
    case MYSQL_TYPE_LONGLONG: {
	return new Item_int((ulonglong)valFromStr(data));
    }
    case MYSQL_TYPE_BLOB: 
    case MYSQL_TYPE_VARCHAR: {
	return new Item_string(make_thd_string(data), data.length(),
			       &my_charset_bin);
    }
    default: {}
    }

    cerr << "incorrect mysql_type " << type << "\n";
    exit(-1);
}
