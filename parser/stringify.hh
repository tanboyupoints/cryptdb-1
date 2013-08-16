#pragma once

#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <functional>

template<class T>
std::string stringify_ptr(T * x) {
    if (x == NULL ) {
	return "NULL";
    } else {
	return x->stringify();
    }
}

static inline std::ostream&
operator<<(std::ostream &out, String &s)
{
    return out << std::string(s.ptr(), s.length());
}

static inline std::ostream&
operator<<(std::ostream &out, Item &i)
{
    String s;
    i.print(&s, QT_ORDINARY);

    return out << s;
}

static inline std::ostream&
operator<<(std::ostream &out, Alter_drop &adrop)
{
    // FIXME: Needs to support escapes.
    return out << adrop.name;
}

template<class T>
class List_noparen: public List<T> {};

template<class T>
static inline List_noparen<T>&
noparen(List<T> &l)
{
    return *(List_noparen<T>*) (&l);
}

template<class T>
static inline std::ostream&
operator<<(std::ostream &out, List_noparen<T> &l)
{
    bool first = true;
    for (auto it = List_iterator<T>(l);;) {
        T *i = it++;
        if (!i)
            break;

        if (!first)
            out << ", ";
        out << *i;
        first = false;
    }
    return out;
}

//Strings needs apostrophes
//template<>
static inline std::ostream&
operator<<(std::ostream &out, List_noparen<String> &l)
{
    bool first = true;
    for (auto it = List_iterator<String>(l);;) {
        String *i = it++;
        if (!i)
            break;

        if (!first) {
            out << ", ";
        }
        out << "'" << *i << "'";
        first = false;
    }
    return out;
}

template<class T>
static inline std::ostream&
operator<<(std::ostream &out, List<T> &l)
{
    return out << "(" << noparen(l) << ")";
}

static inline std::ostream&
operator<<(std::ostream &out, SELECT_LEX &select_lex)
{
    // TODO(stephentu): mysql's select print is
    // missing some parts, like procedure, into outfile,
    // for update, and lock in share mode
    String s;
    THD *t = current_thd;
    select_lex.print(t, &s, QT_ORDINARY);
    return out << s;
}

static inline std::ostream&
operator<<(std::ostream &out, SELECT_LEX_UNIT &select_lex_unit)
{
    String s;
    select_lex_unit.print(&s, QT_ORDINARY);
    return out << s;
}

// FIXME: Combine with vector_join.
template <typename T>
std::string ListJoin(List<T> lst, std::string delim,
                     std::function<std::string(T)> finalize)
{
    std::ostringstream accum;

    auto it = List_iterator<T>(lst);
    T *element = it++;
    for (;element; element = it++) {
        std::string finalized_element = finalize(*element);
        accum << finalized_element;
        accum << delim;
    }

    std::string output, str_accum = accum.str();
    if (str_accum.length() > 0) {
        output = str_accum.substr(0, str_accum.length() - delim.length());
    } else {
        output = str_accum;
    }

    return output;
}

static const char *
sql_type_to_string(enum_field_types tpe, CHARSET_INFO *charset)
{
#define ASSERT_NOT_REACHED() \
    do { \
        assert(false); \
        return ""; \
    } while (0)

    switch (tpe) {
    case MYSQL_TYPE_DECIMAL     : return "DECIMAL";
    case MYSQL_TYPE_TINY        : return "TINYINT";
    case MYSQL_TYPE_SHORT       : return "SMALLINT";
    case MYSQL_TYPE_LONG        : return "INT";
    case MYSQL_TYPE_FLOAT       : return "FLOAT";
    case MYSQL_TYPE_DOUBLE      : return "DOUBLE";
    case MYSQL_TYPE_NULL        : ASSERT_NOT_REACHED();
    case MYSQL_TYPE_TIMESTAMP   : return "TIMESTAMP";
    case MYSQL_TYPE_LONGLONG    : return "BIGINT";
    case MYSQL_TYPE_INT24       : return "MEDIUMINT";
    case MYSQL_TYPE_DATE        : return "DATE";
    case MYSQL_TYPE_TIME        : return "TIME";
    case MYSQL_TYPE_DATETIME    : return "DATETIME";
    case MYSQL_TYPE_YEAR        : return "YEAR";
    case MYSQL_TYPE_NEWDATE     : ASSERT_NOT_REACHED();
    case MYSQL_TYPE_VARCHAR     :
	if (charset == &my_charset_bin) {
	    return "VARBINARY";
	} else {
	    return "VARCHAR";
	}
    case MYSQL_TYPE_BIT         : return "BIT";
    case MYSQL_TYPE_NEWDECIMAL  : return "DECIMAL";
    case MYSQL_TYPE_ENUM        : return "ENUM";
    case MYSQL_TYPE_SET         : return "SET";
    case MYSQL_TYPE_TINY_BLOB   :
	if (charset == &my_charset_bin) {
            return "TINYBLOB";
        } else {
            return "TINYTEXT";
        }
    case MYSQL_TYPE_MEDIUM_BLOB :
	if (charset == &my_charset_bin) {
            return "MEDIUMBLOB";
        } else {
            return "MEDIUMTEXT";
        }
    case MYSQL_TYPE_LONG_BLOB   :
	if (charset == &my_charset_bin) {
            return "LONGBLOB";
        } else {
            return "LONGTEXT";
        }
    case MYSQL_TYPE_BLOB        :
        if (charset == &my_charset_bin) {
            return "BLOB";
        } else {
            return "TEXT";
        }
    case MYSQL_TYPE_VAR_STRING  : ASSERT_NOT_REACHED();
    case MYSQL_TYPE_STRING      : return "CHAR";

    /* don't bother to support */
    case MYSQL_TYPE_GEOMETRY    : ASSERT_NOT_REACHED();
    }

    ASSERT_NOT_REACHED();
}

static std::ostream&
operator<<(std::ostream &out, CHARSET_INFO & ci) {
    out << ci.csname;
    return out;
}

static std::ostream&
operator<<(std::ostream &out, Create_field &f)
{

    // emit field name + type definition
    out << f.field_name << " " << sql_type_to_string(f.sql_type, f.charset);

    // emit extra length info if necessary
    switch (f.sql_type) {

    // optional (length) cases
    case MYSQL_TYPE_BIT:
    case MYSQL_TYPE_TINY:
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_LONGLONG:
    case MYSQL_TYPE_STRING:
        if (f.length)
            out << "(" << f.length << ")";
        break;

    // optional (length, decimal) cases
    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DOUBLE:
        if (f.length && f.decimals) {
            out << "(" << f.length << ", " << f.decimals << ")";
        }
        break;

    // mandatory (length) cases
    case MYSQL_TYPE_VARCHAR:
    case MYSQL_TYPE_VAR_STRING:
        assert(f.length);
        out << "(" << f.length << ")";
        break;

    // optional (length [, decimal]) cases
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
        if (f.length) {
            out << "(" << f.length;
            if (f.decimals) {
                out << ", " << f.decimals << ")";
            } else {
                out << ")";
            }
        }
        break;

    // (val1, val2, ...) cases
    case MYSQL_TYPE_ENUM:
    case MYSQL_TYPE_SET:
        out << f.interval_list;
        break;

    default:
        break;
    }

    // emit extra metadata
    switch (f.sql_type) {
    // optional unsigned zerofill cases
    case MYSQL_TYPE_TINY:
    case MYSQL_TYPE_SHORT:
    case MYSQL_TYPE_INT24:
    case MYSQL_TYPE_LONG:
    case MYSQL_TYPE_LONGLONG:
    case MYSQL_TYPE_DOUBLE:
    case MYSQL_TYPE_FLOAT:
    case MYSQL_TYPE_DECIMAL:
    case MYSQL_TYPE_NEWDECIMAL:
        if (f.flags & UNSIGNED_FLAG) {
            out << " unsigned";
            if (f.flags & ZEROFILL_FLAG) {
                out << " zerofill";
            }
        }
        break;

    // optional character set and collate parameters
    case MYSQL_TYPE_STRING:
    case MYSQL_TYPE_VARCHAR:
    case MYSQL_TYPE_TINY_BLOB:
    case MYSQL_TYPE_BLOB:
    case MYSQL_TYPE_MEDIUM_BLOB:
    case MYSQL_TYPE_LONG_BLOB:
        break;
    case MYSQL_TYPE_ENUM:
    case MYSQL_TYPE_SET:
        if (f.charset) {
            if (f.charset->csname) {
                out << " character set " << f.charset->csname;
            }
            if (f.charset->name) {
                out << " collate '" << f.charset->name << "'";
            }
        }

        break;

    default:
        break;
    }

    // not null or null
    if (f.flags & NOT_NULL_FLAG) {
        out << " not null";
    }

    // default value
    if (f.def) {
        out << " default " << *f.def;
    }

    // auto increment
    if (f.flags & AUTO_INCREMENT_FLAG) {
        out << " auto_increment";
    }

    // primary key
    /*
    if (f.flags & PRI_KEY_FLAG) {
        out << " primary key";
    } else if (f.flags & UNIQUE_FLAG) { // unique
        out << " unique";
    } else if (f.flags & UNIQUE_KEY_FLAG) { // unique key
        out << " unique key";
    }
    */

    // ignore comments

    // TODO(stephentu): column_format?

    // reference_definition

    return out;
}

static std::ostream&
operator<<(std::ostream &out, Key_part_spec &k)
{
    // field name
    std::string field_name(k.field_name.str, k.field_name.length);

    out << field_name;

    // length
    if (k.length) {
        out << " (" << k.length << ")";
    }

    // TODO: asc/desc

    return out;
}

inline std::string
convert_lex_str(const LEX_STRING &l)
{
    return std::string(l.str, l.length);
}

inline LEX_STRING
string_to_lex_str(const std::string &s)
{
    char *cstr = new char[s.length() + 1];
    strncpy(cstr, s.c_str(), s.length());
    return (LEX_STRING){cstr, s.length()};
}

static std::ostream&
operator<<(std::ostream &out, enum legacy_db_type db_type) {
    switch (db_type) {
    case DB_TYPE_INNODB: {out << "InnoDB"; break;}
    case DB_TYPE_ISAM: {out << "ISAM"; break;}
    case DB_TYPE_MYISAM: {out << "MYISAM"; break;}
    case DB_TYPE_CSV_DB: {out << "CSV"; break;}
    default:
	assert_s(false,
		 "stringify does not know how to print db_type "
		 + strFromVal((uint) db_type));
    }

    return out;
}
static std::ostream&
operator<<(std::ostream &out, Key &k)
{
    // TODO: constraint

    // key type
    const char *kname = NULL;
    switch (k.type) {
    case Key::PRIMARY     : kname = "PRIMARY KEY"; break;
    case Key::UNIQUE      : kname = "UNIQUE KEY";      break;
    case Key::MULTIPLE    : kname = "INDEX";       break;
    case Key::FULLTEXT    : kname = "FULLTEXT";    break;
    case Key::SPATIAL     : kname = "SPATIAL";     break;
    case Key::FOREIGN_KEY : kname = "FOREIGN KEY"; break;
    default:
        assert(false);
        break;
    }
    out << kname;

    // index_name
    std::string key_name(k.name.str, k.name.length);
    if (!key_name.empty()) {
        out << " " << key_name;
    }

    // column list
    out << " " << k.columns;

    // TODO: index_option

    // foreign key references
    if (k.type == Key::FOREIGN_KEY) {
        auto fk = static_cast< Foreign_key& >(k);
        out << " references ";
        const std::string &db_str(convert_lex_str(fk.ref_table->db));
        const std::string &tl_str(convert_lex_str(fk.ref_table->table));
        if (!db_str.empty()) {
            out << "" << db_str << "." << tl_str << "";
        } else {
            out << "" << tl_str << "";
        }
        out << " " << fk.ref_columns;
    }

    return out;
}

static void
do_create_table(std::ostream &out, LEX &lex)
{
    assert(lex.sql_command == SQLCOM_CREATE_TABLE);

    THD *t = current_thd;

    // table name
    TABLE_LIST *tl = lex.select_lex.table_list.first;
    {
        String s;
        tl->print(t, &s, QT_ORDINARY);
        out << "create ";

        // temporary
        if (lex.create_info.options & HA_LEX_CREATE_TMP_TABLE) {
            out << "temporary ";
        }

        out << "table ";

        // if not exists
        if (lex.create_info.options & HA_LEX_CREATE_IF_NOT_EXISTS) {
            out << "if not exists ";
        }

        out << tl->table_name << " ";
    }

    if (lex.create_info.options & HA_LEX_CREATE_TABLE_LIKE) {
        // create table ... like tbl_name
        out << " like ";
        TABLE_LIST *select_tables=
          lex.create_last_non_select_table->next_global;
        out << select_tables->alias;
    } else {
        auto cl = lex.alter_info.create_list;
        auto kl = lex.alter_info.key_list;

        // columns
        out << "(" << noparen(cl);

        if (cl.elements && kl.elements) out << ", ";

        // keys
        out << noparen(kl) << ")";

        // TODO(stephentu): table options
        //if (lex.create_info.used_fields) {
        //    mysql_thrower() << "WARNING: table options currently unsupported";
        //}

        // create table ... select ...
        // this strange test for this condition comes from:
        //   create_table_set_open_action_and_adjust_tables()
        if (lex.select_lex.item_list.elements) {
            out << " " << lex.select_lex;
        }

	if (lex.create_info.db_type) {
	    out << " ENGINE=" << lex.create_info.db_type->db_type;
	}
	if (lex.create_info.table_charset) {
	    out << " CHARSET=" << *lex.create_info.table_charset;
	}
	if (lex.create_info.default_table_charset) {
	    out << " DEFAULT CHARSET=" << *lex.create_info.default_table_charset;
	}

    }
}

static std::string prefix_drop_column(Alter_drop adrop) {
    std::ostringstream ss;
    ss << "DROP COLUMN " << adrop;
    return ss.str();
}

static std::string prefix_drop_index(Alter_drop adrop) {
    std::ostringstream ss;
    ss << "DROP INDEX " << adrop;
    return ss.str();
}

static std::string prefix_add_column(Create_field cf) {
    std::ostringstream ss;
    ss << "ADD COLUMN " << cf;
    return ss.str();
}

static std::function<std::string(Key_part_spec)>
do_prefix_add_index(std::string index_name) {
    std::function<std::string(Key_part_spec key_part)> fn =
        [index_name](Key_part_spec key_part) {
            std::ostringstream ss;
            ss << " ADD INDEX " << index_name << " (" << key_part << ")";
            return ss.str();
        };

    return fn;
}

static std::string
prefix_add_index(Key key) {
    std::string index_name = convert_lex_str(key.name);
    std::ostringstream key_output;
    key_output << ListJoin<Key_part_spec>(key.columns, ",",
                                          do_prefix_add_index(index_name));
    return key_output.str();
}
static inline std::ostream&
operator<<(std::ostream &out, LEX &lex)
{
    String s;
    THD *t = current_thd;

    switch (lex.sql_command) {
    case SQLCOM_SELECT:
        out << lex.unit;
        break;

    case SQLCOM_UPDATE:
    case SQLCOM_UPDATE_MULTI:
        {
            TABLE_LIST tl;
            st_nested_join nj;
            tl.nested_join = &nj;
            nj.join_list = lex.select_lex.top_join_list;
            tl.print(t, &s, QT_ORDINARY);
            out << "update " << s;
        }

        if (lex.query_tables->lock_type == TL_WRITE_LOW_PRIORITY) {
            out << " low_priority ";
        }

        if (lex.ignore) {
            out << " ignore ";
        }

        {
            auto ii = List_iterator<Item>(lex.select_lex.item_list);
            auto iv = List_iterator<Item>(lex.value_list);
            for (bool first = true;; first = false) {
                Item *i = ii++;
                Item *v = iv++;
                if (!i || !v)
                    break;
                if (first)
                    out << " set ";
                else
                    out << ", ";
                out << *i << "=" << *v;
            }
        }

        if (lex.select_lex.where)
            out << " where " << *lex.select_lex.where;

        if (lex.sql_command == SQLCOM_UPDATE) {
            if (lex.select_lex.order_list.elements) {
                String s0;
                lex.select_lex.print_order(&s0, lex.select_lex.order_list.first,
                                           QT_ORDINARY);
                out << " order by " << s0;
            }

            {
                String s0;
                lex.select_lex.print_limit(t, &s0, QT_ORDINARY);
                out << s0;
            }
        }
        break;

    case SQLCOM_INSERT:
    case SQLCOM_INSERT_SELECT:
    case SQLCOM_REPLACE:
    case SQLCOM_REPLACE_SELECT:
        {
            bool is_insert =
                lex.sql_command == SQLCOM_INSERT ||
                lex.sql_command == SQLCOM_INSERT_SELECT;
            bool no_select =
                lex.sql_command == SQLCOM_INSERT ||
                lex.sql_command == SQLCOM_REPLACE;
            const char *cmd = is_insert ? "insert" : "replace";
            out << cmd << " ";

            switch (lex.query_tables->lock_type) {
            case TL_WRITE_LOW_PRIORITY:
                out << "low_priority ";
                break;
            case TL_WRITE:
                out << "high_priority ";
                break;
            case TL_WRITE_DELAYED:
                out << "delayed ";
                break;
            default:
                ; // no-op
                break;
            }

            if (lex.ignore) {
                out << "ignore ";
            }

            lex.select_lex.table_list.first->print(t, &s, QT_ORDINARY);
            out << "into " << s;
            if (lex.field_list.head())
                out << " " << lex.field_list;
            if (no_select) {
                if (lex.many_values.head())
                    out << " values " << noparen(lex.many_values);
            } else {
                out << " " << lex.select_lex;
            }
            if (is_insert && lex.duplicates == DUP_UPDATE) {
                out << " on duplicate key update ";
                auto ii = List_iterator<Item>(lex.update_list);
                auto iv = List_iterator<Item>(lex.value_list);
                for (bool first = true;; first = false) {
                    Item *i = ii++;
                    Item *v = iv++;
                    if (!i || !v)
                        break;
                    if (!first)
                        out << ", ";
                    out << *i << "=" << *v;
                }
            }
        }
        break;

    case SQLCOM_DELETE:
    case SQLCOM_DELETE_MULTI:
        out << "delete ";

        if (lex.query_tables->lock_type == TL_WRITE_LOW_PRIORITY) {
            out << "low_priority ";
        }

        if (lex.select_lex.options & OPTION_QUICK) {
            out << "quick ";
        }

        if (lex.ignore) {
            out << "ignore ";
        }

        if (lex.sql_command == SQLCOM_DELETE) {
            lex.query_tables->print(t, &s, QT_ORDINARY);
            out << "from " << s;
            if (lex.select_lex.where)
                out << " where " << *lex.select_lex.where;
            if (lex.select_lex.order_list.elements) {
                String s0;
                lex.select_lex.print_order(&s0, lex.select_lex.order_list.first,
                                           QT_ORDINARY);
                out << " order by " << s0;
            }
            {
                String s0;
                lex.select_lex.print_limit(t, &s0, QT_ORDINARY);
                out << s0;
            }
        } else {
            TABLE_LIST *tbl = lex.auxiliary_table_list.first;
            for (bool f = true; tbl; tbl = tbl->next_local, f = false) {
                String s0;
                tbl->print(t, &s0, QT_ORDINARY);
                out << (f ? "" : ", ") << s0;
            }
            out << " from ";

            {
                String s0;
                TABLE_LIST tl;
                st_nested_join nj;
                tl.nested_join = &nj;
                nj.join_list = lex.select_lex.top_join_list;
                tl.print(t, &s0, QT_ORDINARY);
                out << s0;
            }

            if (lex.select_lex.where)
                out << " where " << *lex.select_lex.where;
        }
        break;

    case SQLCOM_CREATE_TABLE:
        do_create_table(out, lex);
        break;

    case SQLCOM_DROP_TABLE:
        out << "drop ";
        if (lex.drop_temporary) {
            out << "temporary ";
        }
        out << "table ";
        if (lex.drop_if_exists) {
            out << "if exists ";
        }
        // table list
        {
            TABLE_LIST *tbl = lex.select_lex.table_list.first;
            for (bool f = true; tbl; tbl = tbl->next_local, f = false) {
                String s0;
                tbl->print(t, &s0, QT_ORDINARY);
                out << (f ? "" : ", ") << s0;
            }
        }
        if (lex.drop_mode == DROP_RESTRICT) {
          out << " restrict";
        } else if (lex.drop_mode == DROP_CASCADE) {
          out << " cascade";
        }
        break;

    case SQLCOM_CHANGE_DB:
        out << "USE " << lex.select_lex.db;
        break;

    case SQLCOM_BEGIN:
        out << "START TRANSACTION";
        if (lex.start_transaction_opt & MYSQL_START_TRANS_OPT_WITH_CONS_SNAPSHOT)
            out << " WITH CONSISTENT SNAPSHOT";
        break;

    case SQLCOM_COMMIT:
        out << "COMMIT";
        if (lex.tx_chain != TVL_UNKNOWN)
            out << " AND" << (lex.tx_chain == TVL_NO ? " NO" : "") << " CHAIN";
        if (lex.tx_release != TVL_UNKNOWN)
            out << (lex.tx_release == TVL_NO ? " NO" : "") << " RELEASE";
        break;

    case SQLCOM_ROLLBACK:
        out << "ROLLBACK";
        if (lex.tx_chain != TVL_UNKNOWN)
            out << " AND" << (lex.tx_chain == TVL_NO ? " NO" : "") << " CHAIN";
        if (lex.tx_release != TVL_UNKNOWN)
            out << (lex.tx_release == TVL_NO ? " NO" : "") << " RELEASE";
        break;

    /*
     * TODO: Support mixed operations.
     * > Will likely require a filter.
     *
     * You can issue multiple ADD, ALTER, DROP, and CHANGE clauses in a
     * single ALTER TABLE statement seperated by columns.  This is a MySQL
     * extension to standard SQL, which permits only one of each clause
     * per ALTER TABLE statement.
     *
     * ALTER TABLE t DROP COLUMN c, DROP COLUMN d;
     */
    case SQLCOM_ALTER_TABLE:
        out << "ALTER TABLE";
        lex.select_lex.table_list.first->print(t, &s, QT_ORDINARY);
        out << " " << s;

        // TODO: Support other flags.
        // ALTER_ADD_COLUMN, ALTER_CHANGE_COLUMN, ALTER_ADD_INDEX,
        // ALTER_DROP_INDEX, ALTER_FOREIGN_KEY
        if (lex.alter_info.flags & ALTER_DROP_COLUMN) {
            out << " " << ListJoin<Alter_drop>(lex.alter_info.drop_list,
                                               ",", prefix_drop_column);
        } else if (lex.alter_info.flags & ALTER_ADD_COLUMN) {
            out << " " << ListJoin<Create_field>(lex.alter_info.create_list,
                                                 ",", prefix_add_column);
        } else if (lex.alter_info.flags & ALTER_ADD_INDEX) {
            out << " " << ListJoin<Key>(lex.alter_info.key_list, ",",
                                        prefix_add_index);
        } else if (lex.alter_info.flags & ALTER_DROP_INDEX) {
            out << " " << ListJoin<Alter_drop>(lex.alter_info.drop_list,
                                               ",", prefix_drop_index);
        } else {
            throw CryptDBError("Unsupported ALTER in stringify");
        }

        break;

    case SQLCOM_SET_OPTION:
    case SQLCOM_SHOW_DATABASES:
    case SQLCOM_SHOW_TABLES:
    case SQLCOM_SHOW_FIELDS:
    case SQLCOM_SHOW_KEYS:
    case SQLCOM_SHOW_VARIABLES:
    case SQLCOM_SHOW_STATUS:
    case SQLCOM_SHOW_COLLATIONS:
        /* placeholders to make analysis work.. */
        out << ".. type " << lex.sql_command << " query ..";
        break;

    default:
        for (std::stringstream ss;;) {
            ss << "unhandled sql command " << lex.sql_command;
            throw std::runtime_error(ss.str());
        }
    }

    return out;
}

