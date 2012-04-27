#include <parser/lex_util.hh>


using namespace std;

/* Functions for constructing/copying MySQL structures  */

Item_field *
make_item(Item_field *t, string field_name)
{
    THD *thd = current_thd;
    assert(thd);
    // bootstrap i0 from t
    Item_field *i0 = new Item_field(thd, t);
    // clear out alias
    i0->name = NULL;
    if (field_name.size() > 0) {
	i0->field_name = make_thd_string(field_name);
    }
    return i0;
}

Item_string *
make_item(Item_string * i) {
    string s = ItemToString(i);
    return new Item_string(make_thd_string(s), s.length(), i->default_charset());
}


Item_int *
make_item(Item_int * i) {
    return new Item_int(i->value);
}

ORDER *
make_order(ORDER * old_order, Item * i) {
    ORDER * new_order = (ORDER *)malloc(sizeof(ORDER));
    memcpy(new_order, old_order, sizeof(ORDER));
    new_order->next = NULL;
    new_order->item_ptr = i;
    new_order->item = (Item **)malloc(sizeof(Item *));
    *new_order->item = i; 

    return new_order;
}


void
set_select_lex(LEX * lex, SELECT_LEX * select_lex) {
    lex->select_lex = *select_lex;
    lex->unit.*rob<st_select_lex_node, st_select_lex_node *,
		   &st_select_lex_node::slave>::ptr() = &lex->select_lex;
    lex->unit.global_parameters = lex->current_select =
	lex->all_selects_list = &lex->select_lex;
}

void
set_where(st_select_lex * sl, Item * where) {
    sl->where = where;
    if (sl->join) {
	sl->join->conds = where;
    }
}
