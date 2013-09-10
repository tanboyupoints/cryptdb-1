#pragma once

#include <string>

#include <main/Analysis.hh>
#include <main/rewrite_ds.hh>

#include <sql_list.h>
#include <sql_table.h>

const std::string BOLD_BEGIN = "\033[1m";
const std::string RED_BEGIN = "\033[1;31m";
const std::string GREEN_BEGIN = "\033[1;92m";
const std::string COLOR_END = "\033[0m";

Item *
rewrite(Item * const i, const EncSet &req_enc, Analysis &a);

TABLE_LIST *
rewrite_table_list(const TABLE_LIST * const t, const Analysis &a);

TABLE_LIST *
rewrite_table_list(const TABLE_LIST * const t,
                   const std::string &anon_name);

SQL_I_List<TABLE_LIST>
rewrite_table_list(SQL_I_List<TABLE_LIST> tlist, Analysis &a,
                   bool if_exists=false);

List<TABLE_LIST>
rewrite_table_list(List<TABLE_LIST> tll, Analysis &a);

RewritePlan *
gather(Item * const i, reason &tr, Analysis &a);

void
analyze(Item * const i, Analysis &a);

void
optimize(Item ** const i, Analysis &a);

LEX *
begin_transaction_lex(const std::string &dbname);

LEX *
commit_transaction_lex(const std::string &dbname);

std::vector<Create_field *>
rewrite_create_field(const FieldMeta * const fm, Create_field * const f,
                     const Analysis &a);

std::vector<Key *>
rewrite_key(const std::shared_ptr<TableMeta> &tm, Key * const key,
            const Analysis &a);

std::string
bool_to_string(bool b);

bool string_to_bool(const std::string &s);

List<Create_field>
createAndRewriteField(Analysis &a, const ProxyState &ps,
                      Create_field * const cf,
                      const std::shared_ptr<TableMeta> &tm,
                      bool new_table,
                      List<Create_field> &rewritten_cfield_list);

Item *
encrypt_item_layers(Item * const i, onion o, OnionMeta * const om,
                    const Analysis &a, uint64_t IV = 0);

std::string
rewriteAndGetSingleQuery(const ProxyState &ps, const std::string &q);

// FIXME(burrows): Generalize to support any container with next AND end
// semantics.
template <typename T>
std::string vector_join(std::vector<T> v, const std::string &delim,
                        std::string (*finalize)(T))
{
    std::string accum;
    for (typename std::vector<T>::iterator it = v.begin();
         it != v.end(); ++it) {
        std::string element = (*finalize)((T)*it);
        accum.append(element);
        accum.append(delim);
    }

    AssignOnce<std::string> output;
    if (accum.length() > 0) {
        output = accum.substr(0, accum.length() - delim.length());
    } else {
        output = accum;
    }

    return output.get();
}

std::string
escapeString(const std::unique_ptr<Connect> &c,
             const std::string &escape_me);

void
encrypt_item_all_onions(Item *i, FieldMeta *fm,
                        uint64_t IV, std::vector<Item*> &l,
                        Analysis &a);

std::vector<onion>
getOnionIndexTypes();

void
typical_rewrite_insert_type(Item *const i, Analysis &a,
                            std::vector<Item *> &l, FieldMeta *const fm);
