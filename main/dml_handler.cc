#include <main/dml_handler.hh>
#include <main/rewrite_main.hh>
#include <main/rewrite_util.hh>
#include <main/dispatcher.hh>
#include <main/macro_util.hh>
#include <parser/lex_util.hh>

extern CItemTypesDir itemTypes;

// Prototypes.
static void
process_select_lex(LEX *lex, Analysis & a);

static void
process_field_value_pairs(List_iterator<Item> fd_it,
                          List_iterator<Item> val_it, Analysis &a);

static void
process_filters_lex(st_select_lex * select_lex, Analysis & a);

static inline void
analyze_field_value_pair(Item_field * field, Item * val, Analysis & a);

static st_select_lex *
rewrite_filters_lex(st_select_lex *select_lex, Analysis &a);

static void
rewrite_field_value_pairs(List_iterator<Item> fd_it,
                          List_iterator<Item> val_it, Analysis &a,
                          bool *invalids, List<Item> *res_items,
                          List<Item> *res_values);

static bool
invalidates(FieldMeta * fm, const EncSet & es);

template <typename ContainerType>
void rewriteInsertHelper(Item *const i, Analysis &a, FieldMeta *const fm,
                         ContainerType *const append_list)
{
    std::vector<Item *> l;
    itemTypes.do_rewrite_insert(i, a, l, fm);
    for (auto it : l) {
        append_list->push_back(it);
    }
}

class InsertHandler : public DMLHandler {
    virtual void gather(Analysis &a, LEX *lex, const ProxyState &ps) const
    {
        process_select_lex(lex, a);

        // -----------------------
        // ON DUPLICATE KEY UPDATE
        // -----------------------
        auto fd_it = List_iterator<Item>(lex->update_list);
        auto val_it = List_iterator<Item>(lex->value_list);
        process_field_value_pairs(fd_it, val_it, a);

        return;
    }

    virtual LEX *rewrite(Analysis &a, LEX *const lex, const ProxyState &ps)
        const
    {
        LEX *const new_lex = copy(lex);

        const std::string &table =
                lex->select_lex.table_list.first->table_name;
        const TableMeta *const tm = a.getTableMeta(table);

        //rewrite table name
        new_lex->select_lex.table_list.first =
            rewrite_table_list(lex->select_lex.table_list.first, a);

        // -------------------------
        // Fields (and default data)
        // -------------------------
        std::vector<FieldMeta *> fmVec;
        std::vector<Item *> implicit_defaults;
        if (lex->field_list.head()) {
            auto it = List_iterator<Item>(lex->field_list);
            List<Item> newList;
            for (;;) {
                Item *const i = it++;
                if (!i) {
                    break;
                }
                TEST_TextMessageError(i->type() == Item::FIELD_ITEM,
                                      "Expected field item!");
                Item_field *const ifd = static_cast<Item_field*>(i);
                fmVec.push_back(a.getFieldMeta(ifd->table_name,
                                               ifd->field_name));
                rewriteInsertHelper(i, a, NULL, &newList);
            }

            // Collect the implicit defaults.
            std::vector<FieldMeta *> field_implicit_defaults =
                vectorDifference(tm->defaultedFieldMetas(), fmVec);
            Item_field *const seed_item_field =
                static_cast<Item_field *>(new_lex->field_list.head());
            for (auto implicit_it : field_implicit_defaults) {
                // Get default fields.
                Item_field *const item_field =
                    make_item(seed_item_field, table, implicit_it->fname);
                rewriteInsertHelper(item_field, a, NULL, &newList);

                // Get default values.
                const std::string def_value = implicit_it->defaultValue();
                rewriteInsertHelper(make_item_string(def_value), a,
                                    implicit_it, &implicit_defaults);
            }

            new_lex->field_list = newList;
        } else {
            // No field list, use the table order.
            // > Because there is no field list, fields with default
            //   values must be explicity INSERTed so we don't have to
            //   take any action with respect to defaults.
            assert(fmVec.empty());
            std::vector<FieldMeta *> fmetas = tm->orderedFieldMetas();
            fmVec.assign(fmetas.begin(), fmetas.end());
        }

        // -----------------
        //      Values
        // -----------------
        if (lex->many_values.head()) {
            auto it = List_iterator<List_item>(lex->many_values);
            List<List_item> newList;
            for (;;) {
                List_item *const li = it++;
                if (!li) {
                    break;
                }
                assert(li->elements == fmVec.size());
                List<Item> *const newList0 = new List<Item>();
                auto it0 = List_iterator<Item>(*li);
                auto fmVecIt = fmVec.begin();
                for (;;) {
                    Item *const i = it0++;
                    if (!i) {
                        assert(fmVec.end() == fmVecIt);
                        break;
                    }
                    assert(fmVec.end() != fmVecIt);
                    rewriteInsertHelper(i, a, *fmVecIt, newList0);
                    ++fmVecIt;
                }
                for (auto def_it : implicit_defaults) {
                    newList0->push_back(def_it);
                }
                newList.push_back(newList0);
            }
            new_lex->many_values = newList;
        }

        // -----------------------
        // ON DUPLICATE KEY UPDATE
        // -----------------------
        {
            auto fd_it = List_iterator<Item>(lex->update_list);
            auto val_it = List_iterator<Item>(lex->value_list);
            bool invalids;
            List<Item> res_items, res_values;
            rewrite_field_value_pairs(fd_it, val_it, a, &invalids,
                                      &res_items, &res_values);
            //TODO: cleanup old item and value list
            new_lex->update_list = res_items;
            new_lex->value_list = res_values;
        }

        return new_lex;
    }
};

class UpdateHandler : public DMLHandler {
    virtual void gather(Analysis &a, LEX *lex, const ProxyState &ps)
        const
    {
        process_table_list(&lex->select_lex.top_join_list, a);

        if (lex->select_lex.item_list.head()) {
            assert(lex->value_list.head());

            auto fd_it = List_iterator<Item>(lex->select_lex.item_list);
            auto val_it = List_iterator<Item>(lex->value_list);
            process_field_value_pairs(fd_it, val_it, a);
        }

        process_filters_lex(&lex->select_lex, a);
    }

    virtual  LEX *rewrite(Analysis &a, LEX *lex, const ProxyState &ps)
        const
    {
        LEX * new_lex = copy(lex);

        LOG(cdb_v) << "rewriting update \n";

        assert_s(lex->select_lex.item_list.head(),
                 "update needs to have item_list");

        // Rewrite table name
        new_lex->select_lex.top_join_list =
            rewrite_table_list(lex->select_lex.top_join_list, a);

        // Rewrite filters
        set_select_lex(new_lex,
                       rewrite_filters_lex(&new_lex->select_lex, a));

        // Rewrite SET values
        assert(lex->select_lex.item_list.head());
        assert(lex->value_list.head());

        auto fd_it = List_iterator<Item>(lex->select_lex.item_list);
        auto val_it = List_iterator<Item>(lex->value_list);
        bool invalids;
        List<Item> res_items, res_values;
        rewrite_field_value_pairs(fd_it, val_it, a, &invalids,
                                  &res_items, &res_values);
        a.special_update = invalids;
        //TODO: cleanup old item and value list
        new_lex->select_lex.item_list = res_items;
        new_lex->value_list = res_values;
        return new_lex;
    }
};

class DeleteHandler : public DMLHandler {
    virtual void gather(Analysis &a, LEX *lex, const ProxyState &ps)
        const
    {
        process_select_lex(lex, a);
    }
    virtual LEX *rewrite(Analysis &a, LEX *lex, const ProxyState &ps)
        const
    {
        LEX * new_lex = copy(lex);
        new_lex->query_tables = rewrite_table_list(lex->query_tables, a);
        set_select_lex(new_lex,
                       rewrite_select_lex(&new_lex->select_lex, a));

        return new_lex;
    }
};

class SelectHandler : public DMLHandler {
    virtual void gather(Analysis &a, LEX *lex, const ProxyState &ps)
        const
    {
        process_select_lex(lex, a);
    }
    virtual LEX *rewrite(Analysis &a, LEX *lex, const ProxyState &ps)
        const
    {
        LEX * new_lex = copy(lex);
        new_lex->select_lex.top_join_list =
            rewrite_table_list(lex->select_lex.top_join_list, a);
        set_select_lex(new_lex,
            rewrite_select_lex(&new_lex->select_lex, a));

        return new_lex;
    }
};

 LEX *DMLHandler::transformLex(Analysis &analysis, LEX *lex,
                               const ProxyState &ps) const
{
    this->gather(analysis, lex, ps);

    return this->rewrite(analysis, lex, ps);
}

// Helpers.

static void
process_order(Analysis & a, SQL_I_List<ORDER> & lst) {

    for (ORDER *o = lst.first; o; o = o->next) {
        analyze(*o->item, a);
    }
}

//TODO: not clear how these process_*_lex and rewrite_*_lex overlap, cleanup
static void
process_filters_lex(st_select_lex * select_lex, Analysis & a) {

    if (select_lex->where) {
        analyze(select_lex->where, a);
    }

    /*if (select_lex->join &&
        select_lex->join->conds &&
        select_lex->where != select_lex->join->conds)
        analyze(select_lex->join->conds, reason(FULL_EncSet, "join->conds", select_lex->join->conds, 0), a);*/

    if (select_lex->having) {
        analyze(select_lex->having, a);
    }

    process_order(a, select_lex->group_list);
    process_order(a, select_lex->order_list);

}

static void
process_select_lex(LEX *lex, Analysis & a)
{
    process_table_list(&lex->select_lex.top_join_list, a);
    process_select_lex(&lex->select_lex, a);
}

void
process_select_lex(st_select_lex *select_lex, Analysis &a)
{
    //select clause
    auto item_it = List_iterator<Item>(select_lex->item_list);
    for (;;) {
        Item *item = item_it++;
        if (!item)
            break;

        analyze(item, a);
    }

    process_filters_lex(select_lex, a);
}

static void
process_field_value_pairs(List_iterator<Item> fd_it,
                          List_iterator<Item> val_it, Analysis &a)
{
    for (;;) {
        Item *field_item = fd_it++;
        Item *value_item = val_it++;
        if (!field_item) {
            assert(!value_item);
            break;
        }
        assert(value_item != NULL);
        assert(field_item->type() == Item::FIELD_ITEM);
        Item_field *ifd = static_cast<Item_field *>(field_item);

        // FIXME: Change function name.
        analyze_field_value_pair(ifd, value_item, a);
    }
}

// TODO:
// should be able to support general updates such as
// UPDATE table SET x = 2, y = y + 1, z = y+2, z =z +1, w = y, l = x
// this has a few corner cases, since y can only use HOM
// onion so does w, but not l

//analyzes an expression of the form field = val expression from
// an UPDATE
static inline void
analyze_field_value_pair(Item_field * field, Item * val, Analysis & a) {

    reason r;
    a.rewritePlans[val] = gather(val, r, a);
    a.rewritePlans[field] = gather(field, r, a);

    //TODO: an optimization could be performed here to support more updates
    // For example: SET x = x+1, x = 2 --> no need to invalidate DET and OPE
    // onions because first SET does not matter
}

static SQL_I_List<ORDER> *
rewrite_order(Analysis &a, SQL_I_List<ORDER> &lst,
              const EncSet &constr, const std::string &name)
{
    SQL_I_List<ORDER> *new_lst = copy(&lst);
    ORDER * prev = NULL;
    for (ORDER *o = lst.first; o; o = o->next) {
        Item *const i = *o->item;
        RewritePlan *const rp = getAssert(a.rewritePlans, i);
        assert(rp);
        const EncSet es = constr.intersect(rp->es_out);
        // FIXME: Add version that will take a second EncSet of what
        // we had available (ie, rp->es_out).
        TEST_NoAvailableEncSet(es, i->type(), constr, rp->r.why_t,
                               NULL, 0);
        const OLK olk = es.chooseOne();

        Item *const new_item = itemTypes.do_rewrite(*o->item, olk, rp, a);
        ORDER *const neworder = make_order(o, new_item);
        if (NULL == prev) {
            *new_lst = *oneElemList(neworder);
        } else {
            prev->next = neworder;
        }
        prev = neworder;
    }

    return new_lst;
}

static st_select_lex *
rewrite_filters_lex(st_select_lex * select_lex, Analysis & a)
{
    st_select_lex * new_select_lex = copy(select_lex);

    new_select_lex->group_list =
        *rewrite_order(a, select_lex->group_list, EQ_EncSet, "group by");
    new_select_lex->order_list =
        *rewrite_order(a, select_lex->order_list, ORD_EncSet, "order by");

    if (select_lex->where) {
        set_where(new_select_lex, rewrite(select_lex->where,
                                          PLAIN_EncSet, a));
    }
    //  if (select_lex->join &&
    //     select_lex->join->conds &&
    //    select_lex->where != select_lex->join->conds) {
    //cerr << "select_lex join conds " << select_lex->join->conds << "\n";
    //rewrite(&select_lex->join->conds, a);
    //}

    // HACK: We only care about Analysis::item_cache from HAVING.
    a.item_cache.clear();
    if (select_lex->having) {
        set_having(new_select_lex, rewrite(select_lex->having,
                                   PLAIN_EncSet, a));
    }

    return new_select_lex;
}

static void
rewrite_field_value_pairs(List_iterator<Item> fd_it,
                          List_iterator<Item> val_it, Analysis &a,
                          bool *invalids, List<Item> *res_items,
                          List<Item> *res_values)
{
    *invalids = false;
    for (;;) {
        Item *const field_item = fd_it++;
        Item *const value_item = val_it++;
        if (!field_item) {
            assert(NULL == value_item);
            break;
        }
        assert(NULL != value_item);

        assert(field_item->type() == Item::FIELD_ITEM);
        const Item_field *const ifd =
            static_cast<Item_field *>(field_item);
        FieldMeta *const fm =
            a.getFieldMeta(ifd->table_name, ifd->field_name);

        const RewritePlan *const rp =
            getAssert(a.rewritePlans, value_item);
        const EncSet needed = EncSet(a, fm);
        const EncSet r_es = rp->es_out.intersect(needed);
        // FIXME: Add version for situations when we don't know about
        // children.
        TEST_NoAvailableEncSet(r_es, ifd->type(), needed, rp->r.why_t,
                               NULL, 0);

        // Determine salt for field
        bool add_salt = false;
        if (fm->has_salt) {
            const auto it_salt = a.salts.find(fm);
            if ((it_salt == a.salts.end()) && needsSalt(r_es)) {
                add_salt = true;
                const salt_type salt = randomValue();
                a.salts[fm] = salt;
            }
        }

        for (auto pair : r_es.osl) {
            const OLK olk = {pair.first, pair.second.first, fm};
            RewritePlan *const rp_field =
                getAssert(a.rewritePlans, field_item);
            Item *const re_field =
                itemTypes.do_rewrite(field_item, olk, rp_field, a);
            res_items->push_back(re_field);

            RewritePlan *const rp_value =
                getAssert(a.rewritePlans, value_item);
            Item *const re_value =
                itemTypes.do_rewrite(value_item, olk, rp_value, a);
            res_values->push_back(re_value);
        }

        // Add the salt field
        if (add_salt) {
            const salt_type salt = a.salts[fm];
            assert(res_items->elements != 0);
            Item_field * const rew_fd =
                static_cast<Item_field *>(res_items->head());
            assert(rew_fd);
            const std::string anon_table_name = rew_fd->table_name;
            const std::string anon_field_name = fm->getSaltName();
            res_items->push_back(make_item(rew_fd, anon_table_name,
                                           anon_field_name));
            res_values->push_back(new Item_int((ulonglong)salt));
        }

        // Determine if the query invalidates onions.
        *invalids = *invalids || invalidates(fm, r_es);
    }
}

static void
addToReturn(ReturnMeta *rm, int pos, const OLK &constr, bool has_salt,
            const std::string &name)
{
    if (static_cast<unsigned int>(pos) != rm->rfmeta.size()) {
        throw CryptDBError("ReturnMeta has badly ordered ReturnFields!");
    }
    const int salt_pos = has_salt ? pos + 1 : -1;
    std::pair<int, ReturnField>
        pair(pos, ReturnField(false, name, constr, salt_pos));
    rm->rfmeta.insert(pair);
}

static void
addSaltToReturn(ReturnMeta *rm, int pos)
{
    if (static_cast<unsigned int>(pos) != rm->rfmeta.size()) {
        throw CryptDBError("ReturnMeta has badly ordered ReturnFields!");
    }
    std::pair<int, ReturnField>
        pair(pos, ReturnField(true, "", OLK::invalidOLK(), -1));
    rm->rfmeta.insert(pair);
}

static void
rewrite_proj(Item *i, const RewritePlan *rp, Analysis &a,
             List<Item> *newList)
{
    AssignOnce<OLK> olk;
    AssignOnce<Item *> ir;
    if (i->type() == Item::Type::FIELD_ITEM) {
        Item_field *const field_i = static_cast<Item_field *>(i);
        const auto cached_rewritten_i = a.item_cache.find(field_i);
        if (cached_rewritten_i != a.item_cache.end()) {
            ir = cached_rewritten_i->second.first;
            olk = cached_rewritten_i->second.second;
        } else {
            ir = rewrite(i, rp->es_out, a);
            olk = rp->es_out.chooseOne();
        }
    } else {
        ir = rewrite(i, rp->es_out, a);
        olk = rp->es_out.chooseOne();
    }
    assert(ir.assigned() && ir.get());
    newList->push_back(ir.get());
    const bool use_salt = needsSalt(olk.get());

    // This line implicity handles field aliasing for at least some cases.
    // As i->name can/will be the alias.
    addToReturn(a.rmeta, a.pos++, olk.get(), use_salt, i->name);

    if (use_salt) {
        const std::string anon_table_name =
            static_cast<Item_field *>(ir.get())->table_name;
        const std::string anon_field_name = olk.get().key->getSaltName();
        Item_field *const ir_field =
            make_item(static_cast<Item_field *>(ir.get()),
                      anon_table_name, anon_field_name);
        newList->push_back(ir_field);
        addSaltToReturn(a.rmeta, a.pos++);
    }
}

st_select_lex *
rewrite_select_lex(st_select_lex *select_lex, Analysis &a)
{
    // rewrite_filters_lex must be called before rewrite_proj because
    // it is responsible for filling Analysis::item_cache which
    // rewrite_proj uses.
    st_select_lex *new_select_lex =
        rewrite_filters_lex(select_lex, a);

    LOG(cdb_v) << "rewrite select lex input is " << *select_lex;
    auto item_it = List_iterator<Item>(select_lex->item_list);

    List<Item> newList;
    for (;;) {
        Item * const item = item_it++;
        if (!item)
            break;
        LOG(cdb_v) << "rewrite_select_lex " << *item << " with name "
                   << item->name;
        rewrite_proj(item, getAssert(a.rewritePlans, item), a, &newList);
    }

    // TODO(stephentu): investigate whether or not this is a memory leak
    new_select_lex->item_list = newList;

    return new_select_lex;
}

static void
process_table_aliases(List<TABLE_LIST> *tll, Analysis & a)
{
    List_iterator<TABLE_LIST> join_it(*tll);
    for (;;) {
        TABLE_LIST *t = join_it++;
        if (!t)
            break;

        if (t->is_alias) {
            assert(a.addAlias(t->alias, t->table_name));
        }

        if (t->nested_join) {
            process_table_aliases(&t->nested_join->join_list, a);
            return;
        }
    }
}

static void
process_table_joins_and_derived(List<TABLE_LIST> *tll, Analysis & a)
{
    List_iterator<TABLE_LIST> join_it(*tll);
    for (;;) {
        TABLE_LIST *t = join_it++;
        if (!t) {
            break;
        }

        if (t->nested_join) {
            process_table_joins_and_derived(&t->nested_join->join_list, a);
            return;
        }

        if (t->on_expr) {
            analyze(t->on_expr, a);
        }

        //std::string db(t->db, t->db_length);
        //std::string table_name(t->table_name, t->table_name_length);
        //std::string alias(t->alias);

        // Handles SUBSELECTs in table clause.
        if (t->derived) {
            st_select_lex_unit *u = t->derived;
             // Not quite right, in terms of softness:
             // should really come from the items that eventually
             // reference columns in this derived table.
            process_select_lex(u->first_select(), a);
        }
    }
}

void
process_table_list(List<TABLE_LIST> *tll, Analysis & a)
{
    /*
     * later, need to rewrite different joins, e.g.
     * SELECT g2_ChildEntity.g_id, IF(ai0.g_id IS NULL, 1, 0) AS albumsFirst, g2_Item.g_originationTimestamp FROM g2_ChildEntity LEFT JOIN g2_AlbumItem AS ai0 ON g2_ChildEntity.g_id = ai0.g_id INNER JOIN g2_Item ON g2_ChildEntity.g_id = g2_Item.g_id INNER JOIN g2_AccessSubscriberMap ON g2_ChildEntity.g_id = g2_AccessSubscriberMap.g_itemId ...
     */

    process_table_aliases(tll, a);
    process_table_joins_and_derived(tll, a);
}

bool invalidates(FieldMeta * fm, const EncSet & es)
{
    for (auto o_l : fm->children) {
        onion o = o_l.first->getValue();
        if (es.osl.find(o) == es.osl.end()) {
            return true;
        }
    }

    return false;
}

// FIXME: Add test to make sure handlers added successfully.
SQLDispatcher *buildDMLDispatcher()
{
    DMLHandler *h;
    SQLDispatcher *dispatcher = new SQLDispatcher();
    
    h = new InsertHandler();
    dispatcher->addHandler(SQLCOM_INSERT, h);
    dispatcher->addHandler(SQLCOM_REPLACE, h);

    h = new UpdateHandler;
    dispatcher->addHandler(SQLCOM_UPDATE, h);

    h = new DeleteHandler;
    dispatcher->addHandler(SQLCOM_DELETE, h);

    h = new SelectHandler;
    dispatcher->addHandler(SQLCOM_SELECT, h);

    return dispatcher;
}
