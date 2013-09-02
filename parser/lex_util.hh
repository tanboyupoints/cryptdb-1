#pragma once

#include <string>
#include <parser/sql_utils.hh>
#include <util/rob.hh>

#include <sql_select.h>
#include <sql_delete.h>
#include <sql_insert.h>
#include <sql_update.h>

/*
 * lex_util.hh
 *
 * Helper methods for rewriting parts of lex.
 *
 *
 *  Approach for rewriting any data structure of the lex:
 * -- copy old data structure
 * -- replace any data fields that needs to change with their rewritten versions
 * -- update pointers in other parts of lex pointing to these
 * (hence, old and new data structure share items that need not change)
 */

//copies any data structure shallowly
template <typename T>
T *copy(const T *const x) {
    T *const res = static_cast<T *>(malloc(sizeof(T)));
    memcpy(res, x, sizeof(T));
    return res;
}

/* Makes a new item based on
 * information from an old item */
Item_field *make_item(Item_field *const t,
                      const std::string &table_name = "",
                      const std::string &field_name = "");
Item_ref *make_item(Item_ref *const t, Item *const new_ref,
                    const std::string &table_name = "",
                    const std::string &field_name = "");
Item_string *make_item(Item_string *const i);
Item_int *make_item(Item_int *const i);
Item_null *make_item(Item_null *const i);
Item_func *make_item(Item_func *const i);
Item *clone_item(Item *const i);
ORDER *make_order(ORDER *const old_order, Item *const i);


// sets the select_lex in a lex
void
set_select_lex(LEX *const lex, SELECT_LEX *const select_lex);

void
set_where(st_select_lex *const sl, Item *const where);

void
set_having(st_select_lex *const sl, Item *const having);


// Creates a SQL_I_List that contains one element
template <typename T>
SQL_I_List<T> *
oneElemList(T *const elem) {
    SQL_I_List<T> *const res = new SQL_I_List<T>();
    res->elements = 1;
    res->first = elem;
    res->next = NULL;

    return res;
}

template <typename T>
List<T> *
dptrToList(T **const es, unsigned int count)
{
    List<T> *const out = new List<T>();
    for (unsigned int i = 0; i < count; ++i) {
        out->push_back(es[i]);
    }

    return out;
}

