/*
 * Handlers for rewriting functions and operations.
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
#define ANON                ANON_NAME(__anon_id_func_)

template <class T>
static Item *
do_optimize_type_self_and_args(T *i, Analysis &a) {
    UNIMPLEMENTED;
    /*
    Item *i0 = do_optimize_const_item(i, a);
    if (i0 == i) {
        // no optimizations done at top level
        // try children
        Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            optimize(&args[x], a);
        }
        return i;
    } else {
        return i0;
    }*/
}



// in place rewriting of arguments
template <class T>
static void
rewrite_args_FN(T * i, const OLK & constr, const RewritePlanOneOLK * rp,
                Analysis &a) {
    Item **args = i->arguments();
    uint count = i->argument_count();
    for (uint x = 0; x < count; x++) {
        args[x] =
            itemTypes.do_rewrite(args[x], rp->olk, rp->childr_rp[x], a);
        args[x]->name = NULL; // args should never have aliases...
    }
}

// An implementation of gather for the common case operation
// Works for Item_func with two arguments, solution encset is intersect of
// children and my_es
// Only works with nodes with one outgoing encset, which could be other_encset
// if encset_from_intersection is false, or else is the intersection with the children
static RewritePlan *
typical_gather(Analysis & a, Item_func * i, const EncSet &my_es,
               std::string why, reason & my_r,
               bool encset_from_intersection,
               const EncSet & other_encset = PLAIN_EncSet)
{
    Item **args = i->arguments();
    assert(i->argument_count() == 2);

    reason r1, r2;
    RewritePlan ** childr_rp = new RewritePlan*[2];
    childr_rp[0] = gather(args[0], r1, a);
    childr_rp[1] = gather(args[1], r2, a);

    EncSet solution =
        my_es.intersect(childr_rp[0]->es_out).
              intersect(childr_rp[1]->es_out);

    if (solution.empty()) {
        std::cerr << "crypto schemes does not support this query BECAUSE "
                  << i << " NEEDS " << my_es << "\n"
                  << " BECAUSE " << why << "\n" \
                  << " AND children have:  " << r1 << r2 << "\n";
        assert(false);
    }

    const EncSet *out_es;
    if (encset_from_intersection) {
        assert_s(solution.single_crypted_and_or_plainvals(),
                 "cannot use typical_gather with more outgoing encsets");
        out_es = &solution;
    } else {
        out_es = &PLAIN_EncSet;
    }

    my_r = reason(*out_es, why, i);
    my_r.add_child(r1);
    my_r.add_child(r2);

    return new RewritePlanOneOLK(out_es->chooseOne(),
                                 solution.chooseOne(), childr_rp, my_r);
}

static class ANON : public CItemSubtypeFT<Item_func_neg, Item_func::Functype::NEG_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_neg *i, reason &tr, Analysis & a) const {
        return gather(i->arguments()[0], tr, a);
    }
    virtual Item * do_optimize_type(Item_func_neg *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual void do_rewrite_insert_type(Item_func_neg *i, Analysis & a,
                                        std::vector<Item *> &l,
                                        FieldMeta *fm) const
    {
        uint64_t salt = randomValue();
    
        if (fm->has_salt) {
            salt = randomValue();
        } else {
        //TODO: need to use table salt in this case
        }
        
        for (auto it : fm->children) {
            onion o = it.first->getValue();
            OnionMeta *om = it.second;
            l.push_back(encrypt_item_layers(i, o, om, a, salt));
        }
    
        if (fm->has_salt) {
            l.push_back(new Item_int((ulonglong) salt));
        }
    }
}ANON;

static class ANON : public CItemSubtypeFT<Item_func_not, Item_func::Functype::NOT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_not *i, reason &tr, Analysis & a) const {
        return gather(i->arguments()[0], tr, a);
    }
    virtual Item * do_optimize_type(Item_func_not *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;


template<Item_func::Functype FT, class IT>
class CItemCompare : public CItemSubtypeFT<Item_func, FT> {
    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const
    {
        LOG(cdb_v) << "CItemCompare (L1139) do_gather func " << *i;

        const EncSet *my_es;
        std::string why = "";

        if (FT == Item_func::Functype::EQ_FUNC ||
            FT == Item_func::Functype::EQUAL_FUNC ||
            FT == Item_func::Functype::NE_FUNC) {
            my_es = &EQ_EncSet;
            why = "compare equality";
        } else {
            my_es = &ORD_EncSet;
            why = "compare order";
        }

        Item ** args = i->arguments();
        assert_s(i->argument_count() == 2, "expected two arguments for comparison");
        if (!args[0]->const_item() && !args[1]->const_item()) {
            why = why + "; join";
            std::cerr << "join";
            my_es = &JOIN_EncSet;
        }

        return typical_gather(a, i, *my_es, why, tr, false, PLAIN_EncSet);
    }

    virtual Item * do_optimize_type(Item_func *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func *i, const OLK & constr,
                                   const RewritePlan * rp, Analysis & a) const
    {
        LOG(cdb_v) << "do_rewrite_type Item_func " << *i << " constr " << constr;
        Item ** args = i->arguments();
        assert_s(i->argument_count() == 2, "compare function does not have two arguments as expected");
        IT *res = new IT(args[0], args[1]);
        rewrite_args_FN(res, constr, (const RewritePlanOneOLK *)rp, a);
        LOG(cdb_v) << "return 1171 " << *res;
        return res;
    }
};

static CItemCompare<Item_func::Functype::EQ_FUNC,    Item_func_eq>    ANON;
static CItemCompare<Item_func::Functype::EQUAL_FUNC, Item_func_equal> ANON;
static CItemCompare<Item_func::Functype::NE_FUNC,    Item_func_ne>    ANON;
static CItemCompare<Item_func::Functype::GT_FUNC,    Item_func_gt>    ANON;
static CItemCompare<Item_func::Functype::GE_FUNC,    Item_func_ge>    ANON;
static CItemCompare<Item_func::Functype::LT_FUNC,    Item_func_lt>    ANON;
static CItemCompare<Item_func::Functype::LE_FUNC,    Item_func_le>    ANON;

template<Item_func::Functype FT, class IT>
class CItemCond : public CItemSubtypeFT<Item_cond, FT> {
    virtual RewritePlan * do_gather_type(Item_cond *i, reason &tr,
                                         Analysis & a) const
    {
        const unsigned int arg_count = i->argument_list()->elements;
        assert(2 <= arg_count);

        EncSet out_es = PLAIN_EncSet;
        EncSet child_es = PLAIN_EncSet;
        const std::string why = "and/or";

        tr = reason(out_es, why, i);

        RewritePlan **childr_rp = new RewritePlan*[arg_count];

        auto it = List_iterator<Item>(*i->argument_list());
        unsigned int index = 0;
        for (;;) {
            Item *argitem = it++;
            if (!argitem)
                break;
            assert(index < arg_count);

            reason r;
            childr_rp[index] = gather(argitem, r, a);
            tr.add_child(r);
            if (!childr_rp[index]->es_out.contains(PLAIN_OLK)) {
                thrower() << "cannot obtain PLAIN for " << *argitem;
            }
            ++index;
        }

        // Must be an OLK for each argument.
        return new RewritePlanOneOLK(out_es.extract_singleton(),
                                     child_es.extract_singleton(),
                                     childr_rp, tr);
    }

    virtual Item * do_optimize_type(Item_cond *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_cond *i, const OLK & olk,
                                   const RewritePlan * rp, Analysis & a) const
    {
        const unsigned int arg_count = i->argument_list()->elements;
        Item **items = new Item*[arg_count];

        const RewritePlanOneOLK * const rp_one =
            static_cast<const RewritePlanOneOLK * const>(rp);
        auto it = List_iterator<Item>(*i->argument_list());
        unsigned int index = 0;
        for (;;) {
            Item *argitem = it++;
            if (!argitem) {
                break;
            }
            assert(index < arg_count);
        
            items[index] =
                itemTypes.do_rewrite(argitem, rp_one->olk,
                                    rp_one->childr_rp[index], a);
            items[index]->name = NULL;
            ++index;
        }

        IT * res = new IT(items[0], items[1]);
        return res;
    }
};

static CItemCond<Item_func::Functype::COND_AND_FUNC, Item_cond_and> ANON;
static CItemCond<Item_func::Functype::COND_OR_FUNC,  Item_cond_or>  ANON;

template<Item_func::Functype FT>
class CItemNullcheck : public CItemSubtypeFT<Item_bool_func, FT> {
    virtual RewritePlan * do_gather_type(Item_bool_func *i, reason &tr, Analysis & a) const
    {
        Item **args = i->arguments();
        assert(i->argument_count() == 1);

        reason r;
        RewritePlan **child_rp = new RewritePlan*[1];
        child_rp[0] = gather(args[0], r, a);

        EncSet solution = child_rp[0]->es_out;
        EncSet out_es = PLAIN_EncSet;

        tr = reason(out_es, "nullcheck", i);
        tr.add_child(r);

        return new RewritePlanOneOLK(out_es.extract_singleton(),
                                     solution.chooseOne(),
                                     child_rp, tr);
    }

    virtual Item * do_rewrite_type(Item_bool_func * i, const OLK & constr,
                                   const RewritePlan * _rp, Analysis & a)
                                  const
    {
        rewrite_args_FN(i, constr, (const RewritePlanOneOLK *)_rp, a);
        return i;
    }

    virtual Item * do_optimize_type(Item_bool_func *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }

};

static CItemNullcheck<Item_func::Functype::ISNULL_FUNC> ANON;
static CItemNullcheck<Item_func::Functype::ISNOTNULL_FUNC> ANON;

static class ANON : public CItemSubtypeFT<Item_func_get_system_var, Item_func::Functype::GSYSVAR_FUNC> {

    virtual RewritePlan * do_gather_type(Item_func_get_system_var *i, reason &tr, Analysis & a) const {
        reason r = reason(PLAIN_EncSet, "system var", i);
        return new RewritePlan(PLAIN_EncSet, r);
    }

    virtual Item * do_rewrite_type(Item_func_get_system_var * i,
                                   const OLK & constr,
                                   const RewritePlan * _rp,
                                   Analysis & a) const {
        return i;
    }
} ANON;

template<class IT, const char *NAME>
class CItemAdditive : public CItemSubtypeFN<IT, NAME> {
    virtual RewritePlan * do_gather_type(IT *i, reason &tr,
                                         Analysis & a) const
    {
        return typical_gather(a, i, ADD_EncSet, "additive", tr, true);
    }
    virtual Item * do_optimize_type(IT *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(IT *i,
                                   const OLK & constr,
                                   const RewritePlan * _rp,
                                   Analysis & a) const
    {
        LOG(cdb_v) << "do_rewrite_type Item_func_additive_op" << *i << " with constr " << constr;

        //rewrite children
        assert_s(i->argument_count() == 2, " expecting two arguments for additive operator ");
        Item **args = i->arguments();

        RewritePlanOneOLK * rp = (RewritePlanOneOLK *) _rp;

        std::cerr << "Rewrite plan is " << rp << "\n";

        Item * arg0 =
            itemTypes.do_rewrite(args[0], rp->olk, rp->childr_rp[0], a);
        Item * arg1 =
            itemTypes.do_rewrite(args[1], rp->olk, rp->childr_rp[1], a);

        if (oAGG == constr.o) {
            OnionMeta *om = constr.key->getOnionMeta(oAGG);
            assert(om);
            EncLayer *el = a.getBackEncLayer(om);
            assert_s(el->level() == SECLEVEL::HOM, "incorrect onion level on onion oHOM");
            return ((HOM*)el)->sumUDF(arg0, arg1);
        } else {
            IT *out_i = new IT(arg0, arg1);
            return out_i;
        }
    }
};

extern const char str_plus[] = "+";
static CItemAdditive<Item_func_plus, str_plus> ANON;

extern const char str_minus[] = "-";
static CItemAdditive<Item_func_minus, str_minus> ANON;

template<class IT, const char *NAME>
class CItemMath : public CItemSubtypeFN<IT, NAME> {
    virtual RewritePlan * do_gather_type(IT *i,
                                         reason &tr, Analysis & a) const
    {
        return typical_gather(a, i, BESTEFFORT_EncSet, "math op", tr,
                              true);
    }

    virtual Item * do_optimize_type(IT *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(IT *i, const OLK & constr,
                                   const RewritePlan * _rp,
                                   Analysis & a) const
    {
        assert(i->argument_count() == 2);
        Item **args = i->arguments();

        RewritePlanOneOLK * rp = (RewritePlanOneOLK *) _rp;

        Item * arg0 =
            itemTypes.do_rewrite(args[0], rp->olk, rp->childr_rp[0], a);
        Item * arg1 =
            itemTypes.do_rewrite(args[1], rp->olk, rp->childr_rp[1], a);

        Item_func *out_i = new IT(arg0, arg1);
        return out_i;
    }
};

extern const char str_mul[] = "*";
static CItemMath<Item_func_mul, str_mul> ANON;

extern const char str_div[] = "/";
static CItemMath<Item_func_div, str_div> ANON;

extern const char str_idiv[] = "div";
static CItemMath<Item_func_int_div, str_idiv> ANON;

/*
extern const char str_sqrt[] = "sqrt";
static CItemMath<Item_func_sqrt, str_sqrt> ANON;

extern const char str_round[] = "round";
static CItemMath<Item_func_round, str_round> ANON;

extern const char str_sin[] = "sin";
static CItemMath<str_sin> ANON;

extern const char str_cos[] = "cos";
static CItemMath<str_cos> ANON;

extern const char str_acos[] = "acos";
static CItemMath<str_acos> ANON;
*/

extern const char str_pow[] = "pow";
static CItemMath<Item_func_pow, str_pow> ANON;

// FIXME: Supports one argument version as well.
extern const char str_log[] = "log";
static CItemMath<Item_func_log, str_log> ANON;

// extern const char str_radians[] = "radians";
// static CItemMath<str_radians> ANON;


template<const char *NAME>
class CItemLeafFunc : public CItemSubtypeFN<Item_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const
    {
        UNIMPLEMENTED;
    }
};

extern const char str_found_rows[] = "found_rows";
static CItemLeafFunc<str_found_rows> ANON;

extern const char str_last_insert_id[] = "last_insert_id";
static CItemLeafFunc<str_last_insert_id> ANON;

extern const char str_rand[] = "rand";
static CItemLeafFunc<str_rand> ANON;

extern const char str_database[] = "database";
static CItemLeafFunc<str_database> ANON;


static class ANON : public CItemSubtypeFT<Item_extract, Item_func::Functype::EXTRACT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_extract *i, reason &tr, Analysis & a) const
    {
    /*  analyze(i->arguments()[0], reason(EMPTY_EncSet, "extract", i, &tr), a);
        return tr.encset;
    */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_extract *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

template<const char *NAME>
class CItemDateExtractFunc : public CItemSubtypeFN<Item_int_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_int_func *i, reason &tr, Analysis & a) const {
        /*Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            // assuming we separately store different date components
            analyze(args[x], tr, a);
        }
        return tr.encset;*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_int_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};


template<const char *NAME>
class CItemBitfunc : public CItemSubtypeFN<Item_func_bit, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_bit *i, reason &tr, Analysis & a) const 
    {
        /*Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EMPTY_EncSet, "bitfunc", i, &tr), a);
        return tr.encset;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_bit *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_bit_not[] = "~";
static CItemBitfunc<str_bit_not> ANON;

extern const char str_bit_or[] = "|";
static CItemBitfunc<str_bit_or> ANON;

extern const char str_bit_xor[] = "^";
static CItemBitfunc<str_bit_xor> ANON;

extern const char str_bit_and[] = "&";
static CItemBitfunc<str_bit_and> ANON;

static class ANON : public CItemSubtypeFT<Item_func_like, Item_func::Functype::LIKE_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_like *i, reason &tr,
                                         Analysis & a) const {

        assert_s(false, "LIKE needs to be adapted to new RewritePlans");
	/*
	LOG(cdb_v) << "Item_func do_gather_type " << *i;

	Item **args = i->arguments();
	assert_s(i->argument_count() == 2, "expecting LIKE to have two arguments");

	if ((args[0]->type() == Item::Type::FIELD_ITEM) && (args[1]->type() == Item::Type::STRING_ITEM)) {

	    string s(args[1]->str_value.ptr(), args[1]->str_value.length());

	    if (s.find('%') == s.npos && s.find('_') == s.npos) {
                // some queries actually use LIKE as an equality check..
		return typical_gather(a, i, EQ_EncSet, "LIKE equality", tr, false, PLAIN_EncSet);

            } else {

		// XXX check if pattern is one we can support?

		reason r1;
		RewritePlan * rp1 = gather(args[0], r1, a);

		EncSet solution = rp1->es_out.intersect(Search_EncSet);

		if (solution.empty()) {
		    cerr << "cannot support " << i << " BECAUSE it needs any of " << Search_EncSet << "\n" \
			 << " but field only supports  " << rp1->es_out << "\n";
		}

		stringstream ss;
		ss << "like:'" << s << "'";
		tr = reason(PLAIN_EncSet, ss.str(), i);
		tr.add_child(r1);

		return new RewritePlan(PLAIN_OLK, solution.chooseOne(), tr);
            }
        } else {
            // we cannot support non-constant search patterns
	    assert_s(false, "we cannot support search patterns not of the form (field like const string)");
	    //for (uint x = 0; x < i->argument_count(); x++)
            //    analyze(args[x], reason(EMPTY_EncSet, "like-non-const", i, &tr), a);
        }


	*/
	return NULL;
    }
    virtual Item * do_optimize_type(Item_func_like *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
    virtual Item * do_rewrite_type(Item_func_like *i, const OLK & constr,
                                   const RewritePlan *rp,
                                   Analysis & a) const
    {
/*	LOG(cdb_v) << "Item_func_like do_rewrite_type " << *i;

	assert_s(i->argument_count() == 2, "expecting LIKE to have two arguements");
	Item **args = i->arguments();

	if ((args[0]->type() == Item::Type::FIELD_ITEM) && (args[1]->type() == Item::Type::STRING_ITEM)) {

	    string s(args[1]->str_value.ptr(), args[1]->str_value.length());

	    if (s.find('%') == s.npos && s.find('_') == s.npos) {
                // some queries actually use LIKE as an equality check..

		Item_func_like * res = new Item_func_like(args[0], args[1], NULL, false);
		do_rewrite_type_args(i, res, constr, a);

		return res;

            } else {
		// XXX check if pattern is one we can support?

		RewritePlan * plan = getAssert(a.itemRewritePlans, (Item*) i);
		auto childr_plan = getAssert(plan->plan, constr);
		OLK child_OLK = getAssert(childr_plan, args[0]);

		if (child_OLK == PLAIN_OLK) {
		    return new Item_func_like(args[0], args[1], NULL, false);
		}

		Item * field = itemTypes.do_rewrite(args[0], child_OLK, a);
		args[0]->name = NULL; //no alias

		Item * expr = args[1];
		FieldMeta * fm = child_OLK.key;

		EncLayer * el = getAssert(fm->onions, oSWP)->layers.back();
		assert_s(el->level() == SECLEVEL::SEARCH, "incorrect onion  level on onion oSWP");

		Item * res = ((Search *) el)->searchUDF(field, expr);
		cerr << "result is " << *res << "\n";

		return res;
            }
        }

	// we cannot support non-constant search patterns
	assert_s(false, "we cannot support search patterns not of the form (field like constant string)");
*/
        assert_s(false, "LIKE needs to be updated to new RewritePlan interface");
        return NULL;
    }
} ANON;


static class ANON : public CItemSubtypeFT<Item_func, Item_func::Functype::FUNC_SP> {
    void error(Item_func *i) const __attribute__((noreturn)) {
        thrower() << "unsupported store procedure call " << *i;
    }

    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const __attribute__((noreturn)) { error(i); }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func_in, Item_func::Functype::IN_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_in *i, reason &tr, Analysis & a) const
    {
	/*  Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EQ_EncSet, "in", i, &tr), a);
        return tr.encset;
	*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_in *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

static class ANON : public CItemSubtypeFT<Item_func_in, Item_func::Functype::BETWEEN> {
    virtual RewritePlan * do_gather_type(Item_func_in *i, reason &tr, Analysis & a) const
    {
	/*  Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(ORD_EncSet, "between", i, &tr), a);
        return tr.encset;
	*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_in *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

//TODO: use this func in other places as well
/*static List<Item> *
getArgs(Item_func * itf) {
    List<Item> * res = new List<Item>();
    Item ** args = itf->arguments();
    for (uint j = 0; j < itf->argument_count(); j++)  {
	res->push_back(args[j]);
    }
    return res;
}
*/

template<const char *FN, class IT>
class CItemMinMax : public CItemSubtypeFN<Item_func_min_max, FN> {
    virtual RewritePlan * do_gather_type(Item_func_min_max *i, reason &tr, Analysis & a) const
    {
        Item **args = i->arguments();
            uint argcount = i->argument_count();
        if (argcount != 2) {
                std::cerr << "expected two arguments in " << *i << "\n";
            assert(false);
        }
        //at least one has to be a constant as we don't support join now
        assert_s(args[0]->const_item() || args[1]->const_item(), "ope join not yet implemented");

        reason r1, r2;
        RewritePlan ** childr_rp = new RewritePlan*[2];
        childr_rp[0] = gather(args[0], r1, a);
            childr_rp[1] = gather(args[1], r2, a);
        EncSet es1 = childr_rp[0]->es_out;
        EncSet es2 = childr_rp[1]->es_out;

        EncSet needed_es = ORD_EncSet;

        EncSet supported_es = needed_es.intersect(es1).intersect(es2);

        if (supported_es.empty()) {
                std::cerr << "cannot support " << i << " BECAUSE it needs any of " << needed_es << "\n" \
             << "BUT children only have (" << r1 << "\n" << r2 << ")\n";
            assert(false);
        }

        EncSet out_es = es1.intersect(es2);

        tr = reason(out_es, "min_max func", i);
        tr.add_child(r1);
        tr.add_child(r2);

        //prepare rewrite plans
        return new RewritePlanOneOLK(out_es,
                         supported_es.chooseOne(),
                         childr_rp, tr);
    }
    virtual Item * do_optimize_type(Item_func_min_max *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func_min_max *i, const OLK & constr,
                                   const RewritePlan * _rp,
                                   Analysis & a) const
    {
        RewritePlanOneOLK * rp = (RewritePlanOneOLK *) _rp;

        if (rp->olk == PLAIN_OLK) { // no change
            return i;
        }

        // replace with IF( cond_arg0 cond cond_arg1, args0, args1)
        Item ** args = i->arguments();
        Item * cond_arg0 = itemTypes.do_rewrite(args[0],
                            rp->olk, rp->childr_rp[0], a);
        Item * cond_arg1 = itemTypes.do_rewrite(args[1],
                            rp->olk, rp->childr_rp[1], a);

        int cmp_sign =
            i->*rob<Item_func_min_max, int,
                    &Item_func_min_max::cmp_sign>::ptr();

        Item * cond;
        if (cmp_sign) {
            cond = new Item_func_gt(cond_arg0, cond_arg1);
        } else {
            cond = new Item_func_lt(cond_arg0, cond_arg1);
        }

        return new Item_func_if(cond,
                    itemTypes.do_rewrite(args[0], constr,
                                 rp->childr_rp[0], a),
                    itemTypes.do_rewrite(args[1], constr,
                                 rp->childr_rp[1], a));
        }
};

//TODO: do we still need the file analyze.cc?

extern const char str_greatest[] = "greatest";
static CItemMinMax<str_greatest, Item_func_max> ANON;

extern const char str_least[] = "least";
static CItemMinMax<str_least, Item_func_min> ANON;


extern const char str_strcmp[] = "strcmp";
static class ANON : public CItemSubtypeFN<Item_func_strcmp, str_strcmp> {
    virtual RewritePlan * do_gather_type(Item_func_strcmp *i, reason &tr, Analysis & a) const
    {
        //cerr << "do_a_t Item_func_strcmp reason " << tr << "\n";
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EQ_EncSet, "strcmp", i, &tr), a);
        return tr.encset;
	*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_strcmp *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;



extern const char str_if[] = "if";

static class ANON : public CItemSubtypeFN<Item_func_if, str_if> {
    virtual RewritePlan * do_gather_type(Item_func_if *i, reason &tr, Analysis & a) const {
        /*
        Item **args = i->arguments();
        assert(i->argument_count() == 3);
        analyze(args[0], a);
        analyze(args[1], a);
        analyze(args[2], a);
        return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_if *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

extern const char str_nullif[] = "nullif";
static class ANON : public CItemSubtypeFN<Item_func_nullif, str_nullif> {
    virtual RewritePlan * do_gather_type(Item_func_nullif *i, reason &tr, Analysis & a) const
    {
        /*
	    Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
        return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_nullif *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;


extern const char str_coalesce[] = "coalesce";
static class ANON : public CItemSubtypeFN<Item_func_coalesce, str_coalesce> {
    virtual RewritePlan * do_gather_type(Item_func_coalesce *i, reason &tr, Analysis & a) const
    {
        /*
	 Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
	    return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_coalesce *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

extern const char str_case[] = "case";
static class ANON : public CItemSubtypeFN<Item_func_case, str_case> {
    virtual RewritePlan * do_gather_type(Item_func_case *i, reason &tr, Analysis & a) const
    {
	/*     Item **args = i->arguments();
        int first_expr_num = i->*rob<Item_func_case, int,
                &Item_func_case::first_expr_num>::ptr();
        int else_expr_num = i->*rob<Item_func_case, int,
                &Item_func_case::else_expr_num>::ptr();
        uint ncases = i->*rob<Item_func_case, uint,
                &Item_func_case::ncases>::ptr();

        if (first_expr_num >= 0)
            analyze(args[first_expr_num],
                    reason(EQ_EncSet, "case_first", i, &tr), a);
        if (else_expr_num >= 0)
            analyze(args[else_expr_num], tr, a);

        for (uint x = 0; x < ncases; x += 2) {
            if (first_expr_num < 0)
            analyze(args[x],
                    reason(EMPTY_EncSet, "case_nofirst", i, &tr), a);
            else
            analyze(args[x],
                    reason(EQ_EncSet, "case_w/first", i, &tr), a);
            analyze(args[x+1], tr, a);
        }
        return tr.encset;
	*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_case *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;



extern const char str_in_optimizer[] = "<in_optimizer>";
static class ANON : public CItemSubtypeFN<Item_in_optimizer, str_in_optimizer> {
    virtual RewritePlan * do_gather_type(Item_in_optimizer *i, reason &tr, Analysis & a) const
    {
        LOG(cdb_v) << "CItemSubtypeFN (L1107) do_gather " << *i;

        UNIMPLEMENTED;
        //Item **args = i->arguments();
        //analyze(args[0], reason(EMPTY_EncSet, "in_opt", i, &tr), a);
        //analyze(args[1], reason(EMPTY_EncSet, "in_opt", i, &tr), a);
        return NULL;
    }
    virtual Item * do_optimize_type(Item_in_optimizer *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;



template<const char *NAME>
class CItemStrconv : public CItemSubtypeFN<Item_str_conv, NAME> {
    virtual RewritePlan * do_gather_type(Item_str_conv *i, reason & tr, Analysis & a) const
    {
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EMPTY_EncSet, "strconv", i, &tr), a);
        return tr.encset;
	*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_str_conv *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_lcase[] = "lcase";
static CItemStrconv<str_lcase> ANON;

extern const char str_ucase[] = "ucase";
static CItemStrconv<str_ucase> ANON;

extern const char str_length[] = "length";
static CItemStrconv<str_length> ANON;

extern const char str_char_length[] = "char_length";
static CItemStrconv<str_char_length> ANON;

extern const char str_substr[] = "substr";
static CItemStrconv<str_substr> ANON;

extern const char str_concat[] = "concat";
static CItemStrconv<str_concat> ANON;

extern const char str_concat_ws[] = "concat_ws";
static CItemStrconv<str_concat_ws> ANON;

extern const char str_md5[] = "md5";
static CItemStrconv<str_md5> ANON;

extern const char str_left[] = "left";
static CItemStrconv<str_left> ANON;

extern const char str_regexp[] = "regexp";
static CItemStrconv<str_regexp> ANON;
 
extern const char str_second[] = "second";
static CItemDateExtractFunc<str_second> ANON;

extern const char str_minute[] = "minute";
static CItemDateExtractFunc<str_minute> ANON;

extern const char str_hour[] = "hour";
static CItemDateExtractFunc<str_hour> ANON;

extern const char str_to_days[] = "to_days";
static CItemDateExtractFunc<str_to_days> ANON;

extern const char str_year[] = "year";
static CItemDateExtractFunc<str_year> ANON;

extern const char str_month[] = "month";
static CItemDateExtractFunc<str_month> ANON;

extern const char str_dayofmonth[] = "dayofmonth";
static CItemDateExtractFunc<str_dayofmonth> ANON;

extern const char str_unix_timestamp[] = "unix_timestamp";
static CItemDateExtractFunc<str_unix_timestamp> ANON;

extern const char str_date_add_interval[] = "date_add_interval";
static class ANON : public CItemSubtypeFN<Item_date_add_interval, str_date_add_interval> {
    virtual RewritePlan * do_gather_type(Item_date_add_interval *i,
                                         reason &tr, Analysis & a) const
    {
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            // XXX perhaps too conservative
            analyze(args[x], reason(EMPTY_EncSet, "date_add", i, &tr), a);
        }
        return tr.encset;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_date_add_interval *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;

template<const char *NAME>
class CItemDateNow : public CItemSubtypeFN<Item_func_now, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_now *i, reason &tr, Analysis & a) const {
        UNIMPLEMENTED;
    }
};

extern const char str_now[] = "now";
static CItemDateNow<str_now> ANON;

extern const char str_utc_timestamp[] = "utc_timestamp";
static CItemDateNow<str_utc_timestamp> ANON;

extern const char str_sysdate[] = "sysdate";
static CItemDateNow<str_sysdate> ANON;

static class ANON: public CItemSubtypeFT<Item_char_typecast, Item_func::Functype::CHAR_TYPECAST_FUNC> {
    virtual RewritePlan * do_gather_type(Item_char_typecast *i, reason &tr, Analysis & a) const {
        thrower() << "what does Item_char_typecast do?";
        UNIMPLEMENTED;
    }
} ANON;

extern const char str_cast_as_signed[] = "cast_as_signed";
static class ANON : public CItemSubtypeFN<Item_func_signed, str_cast_as_signed> {
    virtual RewritePlan * do_gather_type(Item_func_signed *i, reason &tr,
                                         Analysis & a) const
    {
        /*LOG(cdb_v) << "do_a_t Item_func_signed reason " << tr;
        analyze(i->arguments()[0], tr, a);
        return tr.encset;
	*/
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_signed *i, Analysis & a) const
    {
        return do_optimize_type_self_and_args(i, a);
    }
} ANON;
