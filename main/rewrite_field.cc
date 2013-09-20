/*
 * Handlers for rewriting fields. 
 */

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <set>
#include <list>
#include <algorithm>
#include <stdio.h>
#include <typeinfo>

#include <main/rewrite_main.hh>
#include <main/rewrite_util.hh>
#include <util/cryptdb_log.hh>
#include <main/CryptoHandlers.hh>
#include <parser/lex_util.hh>
#include <main/enum_text.hh>


// gives names to classes and objects we don't care to know the name of 
#define ANON                ANON_NAME(__anon_id_f_)


CItemTypesDir itemTypes = CItemTypesDir();
CItemFuncDir funcTypes = CItemFuncDir();
CItemFuncNameDir funcNames = CItemFuncNameDir();
CItemSumFuncDir sumFuncTypes = CItemSumFuncDir();


/*
 * Unclear whether this is the correct formulation for subqueries that
 * are not contained in SELECT queries. Refer to Name_resolution_context
 * definition for more information.
 */
static std::string
deductPlainTableName(const std::string &field_name,
                     Name_resolution_context *const context,
                     Analysis &a)
{
    assert(context);

    const TABLE_LIST *current_table =
        context->first_name_resolution_table;
    do {
        const TableMeta &tm = a.getTableMeta(current_table->table_name);
        if (tm.childExists(IdentityMetaKey(field_name))) {
            return std::string(current_table->table_name);
        }

        current_table = current_table->next_local;
    } while (current_table != context->last_name_resolution_table);

    return deductPlainTableName(field_name, context->outer_context, a);
}

class ANON : public CItemSubtypeIT<Item_field, Item::Type::FIELD_ITEM> {

    virtual RewritePlan * do_gather_type(Item_field *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "FIELD_ITEM do_gather " << *i;

        const std::string fieldname = i->field_name;
        const std::string table =
            i->table_name ? i->table_name :
                            deductPlainTableName(i->field_name,
                                                 i->context, a);
 
        FieldMeta &fm = a.getFieldMeta(table, fieldname);
        const EncSet es = EncSet(a, &fm);

        tr = reason(es, "is a field", i);

        return new RewritePlan(es, tr);
    }

    virtual Item *
    do_rewrite_type(Item_field *i, const OLK &constr,
                    const RewritePlan *rp, Analysis &a)
        const
    {
        LOG(cdb_v) << "do_rewrite_type FIELD_ITEM " << *i;

        const std::string plain_table_name =
            i->table_name ? i->table_name :
                            deductPlainTableName(i->field_name,
                                                 i->context, a);
        const FieldMeta &fm =
            a.getFieldMeta(plain_table_name, i->field_name);
        //assert(constr.key == fm);

        //check if we need onion adjustment
        const OnionMeta &om =
            a.getOnionMeta(plain_table_name, i->field_name,
                           constr.o);
        const SECLEVEL onion_level = a.getOnionLevel(om);
        assert(onion_level != SECLEVEL::INVALID);
        if (constr.l < onion_level) {
            //need adjustment, throw exception
            throw OnionAdjustExcept(constr.o, fm, constr.l,
                                    std::string(plain_table_name));
        }

        const std::string anon_table_name =
            a.getAnonTableName(plain_table_name);
        const std::string anon_field_name = om.getAnonOnionName();

        Item_field * const res =
            make_item(i, anon_table_name, anon_field_name);
        // This information is only relevant if it comes from a
        // HAVING clause.
        // FIXME: Enforce this semantically.
        a.item_cache[i] = std::make_pair(res, constr);

        return res;
    }
/*
    static OLK
    chooseProj(FieldMeta * fm)
    {
        SECLEVEL l;
        if (contains_get(fm->encdesc.olm, oDET, l)) {
            return OLK(oDET, l, fm);
        }
        if (contains_get(fm->encdesc.olm, oOPE, l)) {
            return OLK(oOPE, l, fm);
        }
        if (contains_get(fm->encdesc.olm, oAGG, l)) {
            return OLK(oAGG, l, fm);
        }
        assert_s(false, "field " + fm->fname + " does not have any decryptable onions for projection");
        return OLK();
    }
*/

    //do we need do_rewrite_insert?
    virtual void
    do_rewrite_insert_type(Item_field *i, Analysis & a,
                           std::vector<Item *> &l, FieldMeta *fm) const
    {
        assert(NULL == fm);
        fm = &a.getFieldMeta(i->table_name, i->field_name);
        const std::string anon_table_name =
            a.getAnonTableName(i->table_name);

        Item_field * new_field = NULL;
        for (auto it : fm->orderedOnionMetas()) {
            const std::string anon_field_name =
                it.second->getAnonOnionName();
            new_field = make_item(i, anon_table_name, anon_field_name);
            l.push_back(new_field);
        }
        if (fm->has_salt) {
            assert(new_field); // need an anonymized field as template to
                               // create salt item
            l.push_back(make_item(new_field, anon_table_name,
                                  fm->getSaltName()));
        }
    }

} ANON;
