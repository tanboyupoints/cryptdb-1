/*
 * Handlers for rewriting aggregation functions, classes
 * inheriting from Item_sum, such as sum, max, min. 
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
#include <main/enum_text.hh>
#include <main/CryptoHandlers.hh>
#include <main/macro_util.hh>
#include <util/cryptdb_log.hh>
#include <parser/lex_util.hh>

// gives names to classes and objects we don't care to know the name of 
#define ANON                ANON_NAME(__anon_id_sum_)

// rewrites the arguments of aggregators
// no_args specifies a certain number of arguments that I must have
// if negative, i can have any no. of arguments
static std::list<Item *>
rewrite_agg_args(Item_sum * oldi, const OLK & constr,
                 const RewritePlanOneOLK * rp, Analysis & a,
                 int no_args = -1)
{
    if (no_args >= 0) {
        TEST_BadItemArgumentCount(oldi->type(), no_args,
                                  oldi->get_arg_count());
    } else {
        no_args = oldi->get_arg_count();
    }

    std::list<Item *> res = std::list<Item *>();
    for (int j = 0; j < no_args; j++) {
        Item *const child_item = oldi->get_arg(j);
        Item *const out_child_item =
            itemTypes.do_rewrite(child_item, rp->olk,
                                 rp->childr_rp[j].get(), a);
        res.push_back(out_child_item);
    }

    return res;
}


template<Item_sum::Sumfunctype SFT>
class CItemCount : public CItemSubtypeST<Item_sum_count, SFT> {
    virtual RewritePlan * do_gather_type(Item_sum_count *i, reason &tr,
                                         Analysis & a) const
    {
        const unsigned int arg_count = i->get_arg_count();
        TEST_BadItemArgumentCount(i->type(), 1, arg_count);
        Item *const child = i->get_arg(0);

        reason r;
        std::vector<std::shared_ptr<RewritePlan> >
            childr_rp({std::shared_ptr<RewritePlan>(gather(child, r, a))});
        const EncSet needed = EQ_EncSet;
        const EncSet solution = childr_rp[0]->es_out.intersect(needed);

        std::string why = "count";
        if (i->has_with_distinct()) {
            why += " distinct";
            TEST_NoAvailableEncSet(solution, i->type(), needed, why,
                                   childr_rp);
        }

        const EncSet out_enc_set = PLAIN_EncSet;
        tr = reason(out_enc_set, why, i);
        tr.add_child(r);

        return new RewritePlanOneOLK(out_enc_set, solution.chooseOne(),
                                     childr_rp, tr);
    }

    virtual Item * do_rewrite_type(Item_sum_count *i,
                                   const OLK & constr,
                                   const RewritePlan * rp,
                                   Analysis & a) const
    {
        std::list<Item *> args =
            rewrite_agg_args(i, constr,
                             static_cast<const RewritePlanOneOLK *>(rp),
                             a, 1);
        auto out_item = new Item_sum_count(args.front());
        out_item->set_distinct(i->has_with_distinct());
        return out_item;
    }
};

static CItemCount<Item_sum::Sumfunctype::COUNT_FUNC> ANON;
static CItemCount<Item_sum::Sumfunctype::COUNT_DISTINCT_FUNC> ANON;



template<Item_sum::Sumfunctype SFT, class IT>
class CItemChooseOrder : public CItemSubtypeST<Item_sum_hybrid, SFT> {
    virtual RewritePlan * do_gather_type(Item_sum_hybrid *i, reason &tr,
                                         Analysis &a) const
    {
        const unsigned int arg_count = i->get_arg_count();
        TEST_BadItemArgumentCount(i->type(), 1, arg_count);
        Item *const child = i->get_arg(0);

        reason r;
        std::vector<std::shared_ptr<RewritePlan> >
            childr_rp({std::shared_ptr<RewritePlan>(gather(child, r, a))});
        const EncSet needed = ORD_EncSet;
        const EncSet supported = needed.intersect(childr_rp[0]->es_out);
        const std::string why = "min/max";
        TEST_NoAvailableEncSet(supported, i->type(), needed, why,
                               childr_rp);
        const OLK olk = supported.chooseOne();
        const EncSet out = EncSet(olk);
        tr = reason(out, why, i);
        // INVESTIGATE: Should 'out' be 'supported'?
        return new RewritePlanOneOLK(out, olk, childr_rp, tr);
    }

    virtual Item *do_rewrite_type(Item_sum_hybrid *i, const OLK &constr,
                                  const RewritePlan *rp, Analysis &a)
        const
    {
        std::list<Item *> args =
            rewrite_agg_args(i, constr,
                             static_cast<const RewritePlanOneOLK *>(rp),
                             a, 1);
        return new IT(args.front());
    }
};

static CItemChooseOrder<Item_sum::Sumfunctype::MIN_FUNC, Item_sum_min> ANON;
static CItemChooseOrder<Item_sum::Sumfunctype::MAX_FUNC, Item_sum_max> ANON;

template<Item_sum::Sumfunctype SFT>
class CItemSum : public CItemSubtypeST<Item_sum_sum, SFT> {
    virtual RewritePlan * do_gather_type(Item_sum_sum *i, reason &tr,
                                         Analysis &a) const
    {
        LOG(cdb_v) << "gather Item_sum_sum " << *i;
        const unsigned int arg_count = i->get_arg_count();
        TEST_BadItemArgumentCount(i->type(), 1, arg_count);
        Item *const child_item = i->get_arg(0);

        reason child_r;
        std::vector<std::shared_ptr<RewritePlan> >
            childr_rp({std::shared_ptr<RewritePlan>(gather(child_item,
                                                           child_r, a))});

        if (i->has_with_distinct()) {
            UNIMPLEMENTED;
        }

        const EncSet my_es = ADD_EncSet;
        const EncSet solution = my_es.intersect(childr_rp[0]->es_out);
        const std::string why = "summation";
        TEST_NoAvailableEncSet(solution, i->type(), my_es, why,
                               childr_rp);

        const OLK olk = solution.chooseOne();
        const EncSet return_es = EncSet(olk);
        tr = reason(return_es, why, i);

        return new RewritePlanOneOLK(return_es, olk, childr_rp, tr);
    }

    virtual Item * do_rewrite_type(Item_sum_sum *i,
                                   const OLK &constr,
                                   const RewritePlan *rp,
                                   Analysis &a) const
    {
        LOG(cdb_v) << "Item_sum_sum rewrite " << *i;

        std::list<Item *> args =
            rewrite_agg_args(i, constr,
                             static_cast<const RewritePlanOneOLK *>(rp),
                             a, 1);

        if (oAGG == constr.o) {
            OnionMeta *const om = constr.key->getOnionMeta(oAGG);
            assert(om);
            EncLayer const &el = a.getBackEncLayer(*om);
            TEST_UnexpectedSecurityLevel(oAGG, SECLEVEL::HOM,
                                         el.level());
            return static_cast<const HOM &>(el).sumUDA(args.front());
        } else {
            TEST_UnexpectedSecurityLevel(constr.o, SECLEVEL::PLAINVAL,
                                         constr.l);
            Item_sum_sum *const out_i =
                new Item_sum_sum(i->get_arg(0), i->has_with_distinct());
            return out_i;
        }
    }
};

//TODO: field OPE should not be blob for text either

static CItemSum<Item_sum::Sumfunctype::SUM_FUNC> ANON;
static CItemSum<Item_sum::Sumfunctype::SUM_DISTINCT_FUNC> ANON;
static CItemSum<Item_sum::Sumfunctype::AVG_FUNC> ANON;
static CItemSum<Item_sum::Sumfunctype::AVG_DISTINCT_FUNC> ANON;

static class ANON : public CItemSubtypeST<Item_sum_bit, Item_sum::Sumfunctype::SUM_BIT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_sum_bit *i, reason &tr,
                                         Analysis & a) const
    {
        /* LOG(cdb_v) << "do_a_t Item_sum_bit reason " << tr;
            analyze(i->get_arg(0), reason(EMPTY_EncSet, "bitagg", i, &tr, false), a);
            return tr.encset;
        */
        UNIMPLEMENTED;
    }
} ANON;

static class ANON : public CItemSubtypeST<Item_func_group_concat, Item_sum::Sumfunctype::GROUP_CONCAT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_group_concat *i,
                                         reason &tr, Analysis & a) const
    {
        /*  LOG(cdb_v) << "do_a_t Item_func_group reason " << tr;
            uint arg_count_field = i->*rob<Item_func_group_concat, uint,
                    &Item_func_group_concat::arg_count_field>::ptr();
            for (uint x = 0; x < arg_count_field; x++) {
                // XXX could perform in the proxy..
                analyze(i->get_arg(x), reason(EMPTY_EncSet, "group_concat", i, &tr), a);
            }

            // XXX order, unused in trace queries..
            return tr.encset;
        */
        UNIMPLEMENTED;
    }
    // TODO(stephentu): figure out how to rob the arg fields for optimization
} ANON;

static class ANON : public CItemSubtypeIT<Item_ref, Item::Type::REF_ITEM> {
    virtual RewritePlan * do_gather_type(Item_ref *i, reason &tr,
                                         Analysis & a) const
    {
        const std::string why = "ref_item";

        reason r;
        std::vector<std::shared_ptr<RewritePlan> >
            childr_rp({std::shared_ptr<RewritePlan>(gather(*i->ref, r,
                                                           a))});

        const EncSet out_es = EncSet(childr_rp[0]->es_out);
        const EncSet child_es = childr_rp[0]->es_out;

        tr = reason(out_es, why, i);
        tr.add_child(r);

        return new RewritePlanOneOLK(out_es, child_es.chooseOne(),
                                     childr_rp, tr);
    }

    virtual Item * do_rewrite_type(Item_ref *i, const OLK & constr,
                                   const RewritePlan * rp,
                                   Analysis & a) const
    {
        // HACK.
        const std::string plain_table =
            static_cast<Item_field *>(*i->ref)->table_name;
        const std::string anon_table =
            a.getAnonTableName(plain_table);

        const std::string plain_field = i->field_name;
        OnionMeta const &om =
            a.getOnionMeta(plain_table, plain_field, constr.o);
        const std::string anon_field = om.getAnonOnionName();

        Item *new_ref = itemTypes.do_rewrite(*i->ref, constr, rp, a);
        Item_ref *out_i = make_item(i, new_ref, anon_table, anon_field);

        return out_i;
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_null, Item::Type::NULL_ITEM> {
    virtual RewritePlan * do_gather_type(Item_null *i, reason &tr,
                                         Analysis & a) const
    {
        tr = reason(FULL_EncSet, "is a constant", i);
        return new RewritePlan(FULL_EncSet, tr);
    }

    virtual Item * do_rewrite_type(Item_null *i,
                                   const OLK & constr,
                                   const RewritePlan * rp,
                                   Analysis & a) const
    {
        return i;
        // return encrypt_item(i, constr, a);
    }

    virtual void do_rewrite_insert_type(Item_null *i, Analysis & a,
                                        std::vector<Item *> &l,
                                        FieldMeta *fm) const
    {
        for (uint j = 0; j < fm->children.size(); ++j) {
            l.push_back(make_item(i));
        }
        if (fm->has_salt) {
            ulonglong salt = randomValue();
            l.push_back(new Item_int((ulonglong) salt));
        }
    }
} ANON;

