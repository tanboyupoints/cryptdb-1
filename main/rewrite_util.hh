#pragma once

#include <string>

#include <main/Analysis.hh>

#include <sql_list.h>
#include <sql_table.h>

// anonymizes table name based on the information in a.schema
// TODO(burrows): Do we want to handle aliasing here, or up a level?
std::string
anonymize_table_name(const std::string &tname,
                     Analysis & a);

Item *
rewrite(Item *i, const OLK & constr, Analysis &a, string context = "");

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, Analysis &a);

SQL_I_List<TABLE_LIST>
rewrite_table_list(SQL_I_List<TABLE_LIST> tlist, Analysis &a,
                   bool if_exists=false);

List<TABLE_LIST>
rewrite_table_list(List<TABLE_LIST> tll, Analysis & a);

RewritePlan *
gather(Item *i, reason &tr, Analysis & a);

void
analyze(Item *i, Analysis & a);

void
optimize(Item **i, Analysis &a);

LEX *
begin_transaction_lex(Analysis a);

LEX *
commit_transaction_lex(Analysis a);

void
do_field_rewriting(LEX *lex, LEX *new_lex, const string &table, Analysis &a);

void
do_key_rewriting(LEX *lex, LEX *new_lex, const string &table, Analysis &a);

bool
create_field_meta(TableMeta *tm, Create_field *field,
                  const Analysis a, bool encByDefault);

bool
do_add_field(TableMeta *tm, const Analysis &a, std::string dbname,
             std::string table, unsigned long long *tid=NULL);

std::string
bool_to_string(bool b);

void
init_onions(AES_KEY * mKey, FieldMeta * fm, Create_field * cf, uint index = 0);

// FIXME(burrows): Generalize to support any container with next AND end
// semantics.
template <typename T>
std::string vector_join(std::vector<T> v, std::string delim,
                        std::string (*finalize)(T))
{
    std::string accum;
    for (typename std::vector<T>::iterator it = v.begin();
         it != v.end(); ++it) {
        std::string element = (*finalize)((T)*it);
        accum.append(element);
        accum.append(delim);
    }

    std::string output;
    if (accum.length() > 0) {
        output = accum.substr(0, accum.length() - delim.length());
    } else {
        output = accum;
    }

    return output;
}


