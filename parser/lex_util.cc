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
make_item(const Item_field &i, const std::string &table_name,
          const std::string &field_name)
{
    assert(i.type() == Item::Type::FIELD_ITEM);

    THD *const thd = current_thd;
    assert(thd);

    // bootstrap i0 from t
    Item_field *const i0 =
        new Item_field(thd, &const_cast<Item_field &>(i));
    init_new_ident(i0, table_name, field_name);

    return i0;
}

Item_ref *
make_item(const Item_ref &i, Item *const new_ref,
          const std::string &table_name,
          const std::string &field_name)
{
    assert(i.type() == Item::Type::REF_ITEM);
    assert(field_name.size() > 0 && table_name.size() > 0);

    THD *const thd = current_thd;
    assert(thd);

    // bootstrap i0 from t
    Item_ref *const i0 = new Item_ref(thd, &const_cast<Item_ref &>(i));

    i0->ref = new Item *;
    *i0->ref = new_ref;

    init_new_ident(i0, table_name, field_name);

    return i0;
}

Item_string *
make_item(Item_string *const i)
{
    assert(i->type() == Item::Type::STRING_ITEM);
    const std::string s = ItemToString(i);
    return new Item_string(make_thd_string(s), s.length(),
                           i->default_charset());
}

Item_int *
make_item(Item_int *const i)
{
    assert(i->type() == Item::Type::INT_ITEM);
    return new Item_int(i->value);
}

Item_null *
make_item(Item_null *const i)
{
    assert(i->type() == Item::Type::NULL_ITEM);
    return new Item_null(i->name);
}

Item_func *
make_item(Item_func *const i)
{
    assert(i->type() == Item::Type::FUNC_ITEM);
    switch (i->functype()) {
        case Item_func::Functype::NEG_FUNC:
            return new Item_func_neg(i->arguments()[0]);
        default:
            throw CryptDBError("Can't clone function type: " + i->type());
    }
}

Item_decimal *
make_item(Item_decimal *const i)
{
    assert(i->type() == Item::Type::DECIMAL_ITEM);
    return static_cast<Item_decimal *>(i->clone_item());
}

// FIXME: Constants should use Item::clone_item like we do for Decimal
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
            return make_item(static_cast<Item_decimal *>(i));
        default:
            throw CryptDBError("Unable to clone: " +
                               std::to_string(i->type()));
    }
}

Item_string *
make_item_string(const std::string &s)
{
    return new Item_string(make_thd_string(s), s.length(),
                           Item::default_charset());
}

ORDER *
make_order(const ORDER *const old_order, Item *const i)
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

bool RiboldMYSQL::is_null(const Item &i)
{
    return const_cast<Item &>(i).is_null();
}

Item *RiboldMYSQL::clone_item(const Item &i)
{
    return const_cast<Item &>(i).clone_item();
}

const List<Item> *RiboldMYSQL::argument_list(const Item_cond &i)
{
    return const_cast<Item_cond &>(i).argument_list();
}

uint RiboldMYSQL::get_arg_count(const Item_sum &i)
{
    return const_cast<Item_sum &>(i).get_arg_count();
}

const Item *RiboldMYSQL::get_arg(const Item_sum &item, uint i)
{
    return const_cast<Item_sum &>(item).get_arg(i);
}

Item_subselect::subs_type RiboldMYSQL::substype(const Item_subselect &i)
{
    return const_cast<Item_subselect &>(i).substype();
}

const st_select_lex *
RiboldMYSQL::get_select_lex(const Item_subselect &i)
{
    return const_cast<Item_subselect &>(i).get_select_lex();
}
