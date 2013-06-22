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
T * copy(T *x) {
    T * res = (T *) malloc(sizeof(T));
    memcpy(res, x, sizeof(T));
    return res;
}

/* Makes a new item based on
 * information from an old item */
Item_field *make_item(Item_field *t, std::string field_name = "");
Item_string * make_item(Item_string * i);
Item_int * make_item(Item_int * i);
Item_int * make_item(Item_num * i);
ORDER * make_order(ORDER * old_order, Item * i);


// sets the select_lex in a lex
void
set_select_lex(LEX * lex, SELECT_LEX * select_lex);

void
set_where(st_select_lex * sl, Item * where);


// Creates a SQL_I_List that contains one element
template <typename T>
SQL_I_List<T> *
oneElemList(T * elem) {
    SQL_I_List<T> * res = new SQL_I_List<T>();
    res->elements = 1;
    res->first = elem;
    res->next = NULL;
   
    return res;
}
