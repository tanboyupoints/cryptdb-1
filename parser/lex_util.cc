#include <parser/lex_util.hh>


using namespace std;

/* Functions for constructing/copying MySQL structures  */

static void
init_new_ident(Item_ident *const i, const std::string &table_name = "",
               const std::string &field_name = "")
{
    // clear out alias
    i->name = NULL;

    if (!table_name.empty()) {
        i->table_name = make_thd_string(table_name);
    }
    if (!field_name.empty()) {
        i->field_name = make_thd_string(field_name);
    }
}

Item_field *
make_item(Item_field * const t, const std::string &table_name,
          const std::string &field_name)
{
    THD *thd = current_thd;
    assert(thd);

    // bootstrap i0 from t
    Item_field *i0 = new Item_field(thd, t);
    init_new_ident(i0, table_name, field_name); 

    return i0;
}

Item_ref *
make_item(Item_ref *const t, Item *const new_ref,
          const std::string &table_name,
          const std::string &field_name)
{
    assert(field_name.size() > 0 && table_name.size() > 0);

    THD *const thd = current_thd;
    assert(thd);

    // bootstrap i0 from t
    Item_ref *const i0 = new Item_ref(thd, t);

    i0->ref = new Item *;
    *i0->ref = new_ref;

    init_new_ident(i0, table_name, field_name); 

    return i0;
}

Item_string *
make_item(Item_string *const i)
{
    std::string s = ItemToString(i);
    return new Item_string(make_thd_string(s), s.length(),
                           i->default_charset());
}

Item_int *
make_item(Item_int *const i)
{
    return new Item_int(i->value);
}

Item_null *
make_item(Item_null *const i)
{
    return new Item_null(i->name);
}

Item_func *
make_item(Item_func *const i)
{
    switch (i->functype()) {
        case Item_func::Functype::NEG_FUNC:
            return new Item_func_neg(i->arguments()[0]);
        default:
            throw CryptDBError("Can't clone function type: " + i->type());
    }
}

Item *
clone_item(Item *const i)
{
    switch (i->type()) {
        case Item::Type::STRING_ITEM:
            return make_item(static_cast<Item_string *>(i));
        case Item::Type::INT_ITEM:
            return make_item(static_cast<Item_int *>(i));
        case Item::Type::NULL_ITEM:
            return make_item(static_cast<Item_null *>(i));
        case Item::Type::FUNC_ITEM:
            return make_item(static_cast<Item_func *>(i));
        case Item::Type::DECIMAL_ITEM:
            throw CryptDBError("Clone decimal item!");
        default:
            throw CryptDBError("Unable to clone: " +
                               std::to_string(i->type()));
    }
}

ORDER *
make_order(ORDER *const old_order, Item *const i)
{
    ORDER *const new_order = static_cast<ORDER *>(malloc(sizeof(ORDER)));
    memcpy(new_order, old_order, sizeof(ORDER));
    new_order->next = NULL;
    new_order->item_ptr = i;
    new_order->item = static_cast<Item **>(malloc(sizeof(Item *)));
    *new_order->item = i;

    return new_order;
}


void
set_select_lex(LEX *const lex, SELECT_LEX *const select_lex)
{
    lex->select_lex = *select_lex;
    lex->unit.*rob<st_select_lex_node, st_select_lex_node *,
                   &st_select_lex_node::slave>::ptr() =
        &lex->select_lex;
    lex->unit.global_parameters = lex->current_select =
    lex->all_selects_list = &lex->select_lex;
}

void
set_where(st_select_lex *const sl, Item *const where)
{
    sl->where = where;
    if (sl->join) {
        sl->join->conds = where;
    }
}

void
set_having(st_select_lex *const sl, Item *const having)
{
    sl->having = having;
    if (sl->join) {
        sl->join->having = having;
    }
}
