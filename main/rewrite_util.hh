#pragma once

#include <string>

#include <main/Analysis.hh>

#include <sql_list.h>
#include <sql_table.h>

Item *
rewrite(Item *i, const OLK & constr, Analysis &a, std::string context = "");

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, Analysis &a);

TABLE_LIST *
rewrite_table_list(TABLE_LIST *t, std::string anon_name);

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

std::vector<Create_field*>
rewrite_create_field(FieldMeta *fm, Create_field *f, const Analysis &a);

std::vector<Key*>
rewrite_key(const std::string &table, Key *key, Analysis &a);

std::string
bool_to_string(bool b);

bool string_to_bool(std::string s);

LEX **
single_lex_output(LEX *out_me, unsigned *out_lex_count);

List<Create_field>
createAndRewriteField(Create_field *cf, TableMeta *tm,
                      const std::string &table, const std::string &dbname,
                      Analysis &a, bool new_table,
                      List<Create_field> &rewritten_cfield_list);

Item *
encrypt_item_layers(Item * i, onion o, OnionMeta *om, Analysis &a,
					FieldMeta *fm = NULL, uint64_t IV = 0);

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

