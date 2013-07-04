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
rewrite_table_list(SQL_I_List<TABLE_LIST> tlist, Analysis &a);

List<TABLE_LIST>
rewrite_table_list(List<TABLE_LIST> tll, Analysis & a);

RewritePlan *
gather(Item *i, reason &tr, Analysis & a);

void
analyze(Item *i, Analysis & a);

LEX *
begin_transaction_lex(Analysis a);

LEX *
commit_transaction_lex(Analysis a);

template <typename T>
std::string vector_join(std::vector<T> v, std::string delim,
                        std::string (*finalize)(T));
