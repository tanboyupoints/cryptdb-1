#pragma once

#include <string>

#include <main/Analysis.hh>

#include <sql_list.h>
#include <sql_table.h>

const std::string BOLD_BEGIN = "\033[1m";
const std::string RED_BEGIN = "\033[1;31m";
const std::string GREEN_BEGIN = "\033[1;92m";
const std::string COLOR_END = "\033[0m";

Item *
rewrite(Item *i, const EncSet &req_enc, Analysis &a,
        std::string context = "");

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
begin_transaction_lex(const std::string &dbname);

LEX *
commit_transaction_lex(const std::string &dbname);

std::vector<Create_field*>
rewrite_create_field(FieldMeta *fm, Create_field *f, const Analysis &a);

std::vector<Key*>
rewrite_key(const TableMeta *tm, Key *key, Analysis &a);

std::string
bool_to_string(bool b);

bool string_to_bool(std::string s);

List<Create_field>
createAndRewriteField(Analysis &a, const ProxyState &ps,
                      Create_field *cf, TableMeta *tm,
                      bool new_table,
                      List<Create_field> &rewritten_cfield_list);

Item *
encrypt_item_layers(Item * i, onion o, OnionMeta * const om,
                    const Analysis &a, uint64_t IV = 0);

std::string
rewriteAndGetSingleQuery(const ProxyState &ps, const std::string &q);

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

