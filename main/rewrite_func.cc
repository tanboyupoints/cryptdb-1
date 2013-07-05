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

#include <main/cdb_rewrite.hh>
#include <main/rewrite_util.hh>
#include <util/cryptdb_log.hh>
#include <main/CryptoHandlers.hh>
#include <parser/lex_util.hh>
#include <main/enum_text.hh>

#include <main/MultiPrinc.hh>


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
rewrite_args_FN(T * i, const OLK & constr,
		const RewritePlanOneOLK * rp, Analysis &a) {

    Item **args = i->arguments();
    uint count = i->argument_count();
    for (uint x = 0; x < count; x++) {
        args[x] = itemTypes.do_rewrite(args[x],
				       rp->olk, rp->childr_rp[x],
				       a);
        args[x]->name = NULL; // args should never have aliases...
    }
}

// An implementation of gather for the common case operation
// Works for Item_func with two arguments, solution encset is intersect of
// children and my_es
// Only works with nodes with one outgoing encset, which could be other_encset
// if encset_from_intersection is false, or else is the intersection with the children
static RewritePlan *
typical_gather(Analysis & a, Item_func * i,
	     const EncSet & my_es, string why, reason & my_r,
	     bool encset_from_intersection, const EncSet & other_encset = PLAIN_EncSet)
{

    Item **args = i->arguments();
    assert(i->argument_count() == 2);

    reason r1, r2;
    RewritePlan ** childr_rp = new RewritePlan*[2];
    childr_rp[0] = gather(args[0], r1, a);
    childr_rp[1] = gather(args[1], r2, a);

    EncSet solution = my_es.intersect(childr_rp[0]->es_out).
	                    intersect(childr_rp[1]->es_out);

    if (solution.empty()) {
	cerr << "crypto schemes does not support this query BECAUSE " << i << "NEEDS " << my_es << "\n" \
	     << " BECAUSE " << why << "\n" \
	     << " AND children have:  " << r1 << r2 << "\n";
	assert(false);
    }

    EncSet out_es;
    if (encset_from_intersection) {
	assert_s(solution.singleton(), "cannot use basic_gather with more outgoing encsets");
	out_es = solution;
    } else {
	out_es = PLAIN_EncSet;
    }

    my_r = reason(out_es, why, i);
    my_r.add_child(r1);
    my_r.add_child(r2);

    return new RewritePlanOneOLK(out_es.extract_singleton(),
				 solution.chooseOne(), childr_rp,
				 my_r);

}


static class ANONU : public CItemSubtypeFT<Item_func_neg, Item_func::Functype::NEG_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_neg *i, reason &tr, Analysis & a) const {
        return gather(i->arguments()[0], tr, a);
    }
    virtual Item * do_optimize_type(Item_func_neg *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;

static class ANONU : public CItemSubtypeFT<Item_func_not, Item_func::Functype::NOT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_not *i, reason &tr, Analysis & a) const {
        return gather(i->arguments()[0], tr, a);
    }
    virtual Item * do_optimize_type(Item_func_not *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;


template<Item_func::Functype FT, class IT>
class CItemCompare : public CItemSubtypeFT<Item_func, FT> {
    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const {
	LOG(cdb_v) << "CItemCompare (L1139) do_gather func " << *i;

        EncSet my_es;
	string why = "";

        if (FT == Item_func::Functype::EQ_FUNC ||
            FT == Item_func::Functype::EQUAL_FUNC ||
            FT == Item_func::Functype::NE_FUNC) {
            my_es = EQ_EncSet;
	    why = "compare equality";
        } else {
            my_es = ORD_EncSet;
	    why = "compare order";
        }

	Item ** args = i->arguments();
	assert_s(i->argument_count() == 2, "expected two arguments for comparison");
        if (!args[0]->const_item() && !args[1]->const_item()) {
            why = why + "; join";
	    cerr << "join";
	    my_es = JOIN_EncSet;
	}

	return typical_gather(a, i, my_es, why, tr, false, PLAIN_EncSet);

    }

    virtual Item * do_optimize_type(Item_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func *i, const OLK & constr,
				   const RewritePlan * rp, Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type Item_func " << *i << " constr " << constr;
	Item ** args = i->arguments();
	assert_s(i->argument_count() == 2, "compare function does not have two arguments as expected");
	IT *res = new IT(args[0], args[1]);
	rewrite_args_FN(res, constr, (const RewritePlanOneOLK *)rp, a);
        LOG(cdb_v) << "return 1171 " << *res;
        return res;
    }
};

static CItemCompare<Item_func::Functype::EQ_FUNC,    Item_func_eq>    ANONU;
static CItemCompare<Item_func::Functype::EQUAL_FUNC, Item_func_equal> ANONU;
static CItemCompare<Item_func::Functype::NE_FUNC,    Item_func_ne>    ANONU;
static CItemCompare<Item_func::Functype::GT_FUNC,    Item_func_gt>    ANONU;
static CItemCompare<Item_func::Functype::GE_FUNC,    Item_func_ge>    ANONU;
static CItemCompare<Item_func::Functype::LT_FUNC,    Item_func_lt>    ANONU;
static CItemCompare<Item_func::Functype::LE_FUNC,    Item_func_le>    ANONU;

template<Item_func::Functype FT, class IT>
class CItemCond : public CItemSubtypeFT<Item_cond, FT> {
    virtual RewritePlan * do_gather_type(Item_cond *i, reason &tr, Analysis & a) const {
	/*   auto it = List_iterator<Item>(*i->argument_list());
        for (;;) {
            Item *argitem = it++;
            if (!argitem)
                break;
	    reason new_tr(tr.encset.intersect(EQ_EncSet), "and/or", i);
            RewritePlan * rp = gather(argitem, tr, a);
            if (!rp->es_out.contains(PLAIN_OLK))
                thrower() << "cannot obtain PLAIN for " << *argitem;
        }

        return rp;*/
	UNIMPLEMENTED;
    }

    virtual Item * do_optimize_type(Item_cond *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_cond *i,
				   const OLK & olk, const RewritePlan * rp, Analysis & a) const {
	List<Item> newlist;
	auto it = List_iterator<Item>(*i->argument_list());
	for (;;) {
	    Item *argitem = it++;
	    if (!argitem) {
		break;
	    }
	    newlist.push_back(rewrite(argitem, olk, a));
	}

	IT * res = new IT(newlist);
	return res;
    }
};

static CItemCond<Item_func::Functype::COND_AND_FUNC, Item_cond_and> ANONU;
static CItemCond<Item_func::Functype::COND_OR_FUNC,  Item_cond_or>  ANONU;

template<Item_func::Functype FT>
class CItemNullcheck : public CItemSubtypeFT<Item_bool_func, FT> {
    virtual RewritePlan * do_gather_type(Item_bool_func *i, reason &tr, Analysis & a) const {
        Item **args = i->arguments();
        assert(i->argument_count() == 1);

        reason r;
        RewritePlan **child_rp = new RewritePlan*[2];
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
                                  const {
        rewrite_args_FN(i, constr, (const RewritePlanOneOLK *)_rp, a);
        return i;
    }

    virtual Item * do_optimize_type(Item_bool_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

};

static CItemNullcheck<Item_func::Functype::ISNULL_FUNC> ANONU;
static CItemNullcheck<Item_func::Functype::ISNOTNULL_FUNC> ANONU;

static class ANONU : public CItemSubtypeFT<Item_func_get_system_var, Item_func::Functype::GSYSVAR_FUNC> {

    virtual RewritePlan * do_gather_type(Item_func_get_system_var *i, reason &tr, Analysis & a) const {
	reason r = reason(PLAIN_EncSet, "system var", i);
	return new RewritePlan(PLAIN_EncSet, r);
    }

    virtual Item * do_rewrite_type(Item_func_get_system_var * i,
	                           const OLK & constr, const RewritePlan * _rp,
	                           Analysis & a) const {
	return i;
    }
} ANONU;


template<const char *NAME>
class CItemAdditive : public CItemSubtypeFN<Item_func_additive_op, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_additive_op *i, reason &tr, Analysis & a) const {
	return typical_gather(a, i, ADD_EncSet, "additive", tr, true);
    }
    virtual Item * do_optimize_type(Item_func_additive_op *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func_additive_op *i,
				   const OLK & constr, const RewritePlan * _rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type Item_func_additive_op" << *i << " with constr " << constr;

	//rewrite children
	assert_s(i->argument_count() == 2, " expecting two arguments for additive operator ");
	Item **args = i->arguments();

	RewritePlanOneOLK * rp = (RewritePlanOneOLK *) _rp;

        cerr << "Rewrite plan is " << rp << "\n";

	Item * arg0 = itemTypes.do_rewrite(args[0],
					   rp->olk, rp->childr_rp[0], a);
	Item * arg1 = itemTypes.do_rewrite(args[1],
					   rp->olk, rp->childr_rp[1], a);

	EncLayer *el = getAssert(constr.key->onions, oAGG)->layers.back();
	assert_s(el->level() == SECLEVEL::HOM, "incorrect onion level on onion oHOM");
	return ((HOM*)el)->sumUDF(arg0, arg1);

	}
};

extern const char str_plus[] = "+";
static CItemAdditive<str_plus> ANONU;

extern const char str_minus[] = "-";
static CItemAdditive<str_minus> ANONU;

template<const char *NAME>
class CItemMath : public CItemSubtypeFN<Item_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_func *i, /* TODO reason not necessary */ reason &tr, Analysis & a) const {
	 Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
	    return a.rewritePlans.find(i)->second;
    }
    virtual Item * do_optimize_type(Item_func *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_mul[] = "*";
static CItemMath<str_mul> ANONU;

extern const char str_div[] = "/";
static CItemMath<str_div> ANONU;

extern const char str_idiv[] = "div";
static CItemMath<str_idiv> ANONU;

extern const char str_sqrt[] = "sqrt";
static CItemMath<str_sqrt> ANONU;

extern const char str_round[] = "round";
static CItemMath<str_round> ANONU;

extern const char str_sin[] = "sin";
static CItemMath<str_sin> ANONU;

extern const char str_cos[] = "cos";
static CItemMath<str_cos> ANONU;

extern const char str_acos[] = "acos";
static CItemMath<str_acos> ANONU;

extern const char str_pow[] = "pow";
static CItemMath<str_pow> ANONU;

extern const char str_log[] = "log";
static CItemMath<str_log> ANONU;

extern const char str_radians[] = "radians";
static CItemMath<str_radians> ANONU;


template<const char *NAME>
class CItemLeafFunc : public CItemSubtypeFN<Item_func, NAME> {
    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const {
	UNIMPLEMENTED;
    }
};

extern const char str_found_rows[] = "found_rows";
static CItemLeafFunc<str_found_rows> ANONU;

extern const char str_last_insert_id[] = "last_insert_id";
static CItemLeafFunc<str_last_insert_id> ANONU;

extern const char str_rand[] = "rand";
static CItemLeafFunc<str_rand> ANONU;

extern const char str_database[] = "database";
static CItemLeafFunc<str_database> ANONU;


static class ANONU : public CItemSubtypeFT<Item_extract, Item_func::Functype::EXTRACT_FUNC> {
    virtual RewritePlan * do_gather_type(Item_extract *i, reason &tr, Analysis & a) const {
	/* analyze(i->arguments()[0], reason(EMPTY_EncSet, "extract", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_extract *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;

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
    virtual RewritePlan * do_gather_type(Item_func_bit *i, reason &tr, Analysis & a) const {
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
static CItemBitfunc<str_bit_not> ANONU;

extern const char str_bit_or[] = "|";
static CItemBitfunc<str_bit_or> ANONU;

extern const char str_bit_xor[] = "^";
static CItemBitfunc<str_bit_xor> ANONU;

extern const char str_bit_and[] = "&";
static CItemBitfunc<str_bit_and> ANONU;

static class ANONU : public CItemSubtypeFT<Item_func_like, Item_func::Functype::LIKE_FUNC> {
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
    virtual Item * do_rewrite_type(Item_func_like *i,
				   const OLK & constr, const RewritePlan *rp,
				   Analysis & a) const {
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
} ANONU;


static class ANONU : public CItemSubtypeFT<Item_func, Item_func::Functype::FUNC_SP> {
    void error(Item_func *i) const __attribute__((noreturn)) {
        thrower() << "unsupported store procedure call " << *i;
    }

    virtual RewritePlan * do_gather_type(Item_func *i, reason &tr, Analysis & a) const __attribute__((noreturn)) { error(i); }
} ANONU;

static class ANONU : public CItemSubtypeFT<Item_func_in, Item_func::Functype::IN_FUNC> {
    virtual RewritePlan * do_gather_type(Item_func_in *i, reason &tr, Analysis & a) const {
	/*  Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EQ_EncSet, "in", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_in *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;

static class ANONU : public CItemSubtypeFT<Item_func_in, Item_func::Functype::BETWEEN> {
    virtual RewritePlan * do_gather_type(Item_func_in *i, reason &tr, Analysis & a) const {
	/*  Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(ORD_EncSet, "between", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_in *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;

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
    virtual RewritePlan * do_gather_type(Item_func_min_max *i, reason &tr, Analysis & a) const {
	Item **args = i->arguments();
        uint argcount = i->argument_count();
	if (argcount != 2) {
	    cerr << "expected two arguments in " << *i << "\n";
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
	    cerr << "cannot support " << i << " BECAUSE it needs any of " << needed_es << "\n" \
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
    virtual Item * do_optimize_type(Item_func_min_max *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }

    virtual Item * do_rewrite_type(Item_func_min_max *i,
				   const OLK & constr, const RewritePlan * _rp,
				   Analysis & a) const {
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

	int cmp_sign = i->*rob<Item_func_min_max, int, &Item_func_min_max::cmp_sign>::ptr();

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
static CItemMinMax<str_greatest, Item_func_max> ANONU;

extern const char str_least[] = "least";
static CItemMinMax<str_least, Item_func_min> ANONU;


extern const char str_strcmp[] = "strcmp";
static class ANONU : public CItemSubtypeFN<Item_func_strcmp, str_strcmp> {
    virtual RewritePlan * do_gather_type(Item_func_strcmp *i, reason &tr, Analysis & a) const {
        //cerr << "do_a_t Item_func_strcmp reason " << tr << "\n";
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EQ_EncSet, "strcmp", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_strcmp *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;



extern const char str_if[] = "if";

static class ANONU : public CItemSubtypeFN<Item_func_if, str_if> {
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
} ANONU;

extern const char str_nullif[] = "nullif";
static class ANONU : public CItemSubtypeFN<Item_func_nullif, str_nullif> {
    virtual RewritePlan * do_gather_type(Item_func_nullif *i, reason &tr, Analysis & a) const {
        /*
	    Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
        return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_nullif *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;


extern const char str_coalesce[] = "coalesce";
static class ANONU : public CItemSubtypeFN<Item_func_coalesce, str_coalesce> {
    virtual RewritePlan * do_gather_type(Item_func_coalesce *i, reason &tr, Analysis & a) const {
        /*
	 Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], a);
	    return a.rewritePlans.find(i)->second;
        */
        UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_coalesce *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;

extern const char str_case[] = "case";
static class ANONU : public CItemSubtypeFN<Item_func_case, str_case> {
    virtual RewritePlan * do_gather_type(Item_func_case *i, reason &tr, Analysis & a) const {
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
    virtual Item * do_optimize_type(Item_func_case *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;



extern const char str_in_optimizer[] = "<in_optimizer>";
static class ANONU : public CItemSubtypeFN<Item_in_optimizer, str_in_optimizer> {
    virtual RewritePlan * do_gather_type(Item_in_optimizer *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeFN (L1107) do_gather " << *i;

	UNIMPLEMENTED;
        //Item **args = i->arguments();
        //analyze(args[0], reason(EMPTY_EncSet, "in_opt", i, &tr), a);
        //analyze(args[1], reason(EMPTY_EncSet, "in_opt", i, &tr), a);
        return NULL;
    }
    virtual Item * do_optimize_type(Item_in_optimizer *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;



template<const char *NAME>
class CItemStrconv : public CItemSubtypeFN<Item_str_conv, NAME> {
    virtual RewritePlan * do_gather_type(Item_str_conv *i, reason & tr, Analysis & a) const {
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++)
            analyze(args[x], reason(EMPTY_EncSet, "strconv", i, &tr), a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_str_conv *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
};

extern const char str_lcase[] = "lcase";
static CItemStrconv<str_lcase> ANONU;

extern const char str_ucase[] = "ucase";
static CItemStrconv<str_ucase> ANONU;

extern const char str_length[] = "length";
static CItemStrconv<str_length> ANONU;

extern const char str_char_length[] = "char_length";
static CItemStrconv<str_char_length> ANONU;

extern const char str_substr[] = "substr";
static CItemStrconv<str_substr> ANONU;

extern const char str_concat[] = "concat";
static CItemStrconv<str_concat> ANONU;

extern const char str_concat_ws[] = "concat_ws";
static CItemStrconv<str_concat_ws> ANONU;

extern const char str_md5[] = "md5";
static CItemStrconv<str_md5> ANONU;

extern const char str_left[] = "left";
static CItemStrconv<str_left> ANONU;

extern const char str_regexp[] = "regexp";
static CItemStrconv<str_regexp> ANONU;
 
extern const char str_second[] = "second";
static CItemDateExtractFunc<str_second> ANONU;

extern const char str_minute[] = "minute";
static CItemDateExtractFunc<str_minute> ANONU;

extern const char str_hour[] = "hour";
static CItemDateExtractFunc<str_hour> ANONU;

extern const char str_to_days[] = "to_days";
static CItemDateExtractFunc<str_to_days> ANONU;

extern const char str_year[] = "year";
static CItemDateExtractFunc<str_year> ANONU;

extern const char str_month[] = "month";
static CItemDateExtractFunc<str_month> ANONU;

extern const char str_dayofmonth[] = "dayofmonth";
static CItemDateExtractFunc<str_dayofmonth> ANONU;

extern const char str_unix_timestamp[] = "unix_timestamp";
static CItemDateExtractFunc<str_unix_timestamp> ANONU;

extern const char str_date_add_interval[] = "date_add_interval";
static class ANONU : public CItemSubtypeFN<Item_date_add_interval, str_date_add_interval> {
    virtual RewritePlan * do_gather_type(Item_date_add_interval *i, reason &tr, Analysis & a) const {
	/* Item **args = i->arguments();
        for (uint x = 0; x < i->argument_count(); x++) {
            // XXX perhaps too conservative
            analyze(args[x], reason(EMPTY_EncSet, "date_add", i, &tr), a);
        }
        return tr.encset;
        */
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_date_add_interval *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;

template<const char *NAME>
class CItemDateNow : public CItemSubtypeFN<Item_func_now, NAME> {
    virtual RewritePlan * do_gather_type(Item_func_now *i, reason &tr, Analysis & a) const {
        UNIMPLEMENTED;
    }
};

extern const char str_now[] = "now";
static CItemDateNow<str_now> ANONU;

extern const char str_utc_timestamp[] = "utc_timestamp";
static CItemDateNow<str_utc_timestamp> ANONU;

extern const char str_sysdate[] = "sysdate";
static CItemDateNow<str_sysdate> ANONU;



static class ANONU: public CItemSubtypeFT<Item_char_typecast, Item_func::Functype::CHAR_TYPECAST_FUNC> {
    virtual RewritePlan * do_gather_type(Item_char_typecast *i, reason &tr, Analysis & a) const {
        thrower() << "what does Item_char_typecast do?";
        UNIMPLEMENTED;
    }
} ANONU;

extern const char str_cast_as_signed[] = "cast_as_signed";
static class ANONU : public CItemSubtypeFN<Item_func_signed, str_cast_as_signed> {
    virtual RewritePlan * do_gather_type(Item_func_signed *i, reason &tr, Analysis & a) const {
        /*LOG(cdb_v) << "do_a_t Item_func_signed reason " << tr;
        analyze(i->arguments()[0], tr, a);
        return tr.encset;
	*/
	UNIMPLEMENTED;
    }
    virtual Item * do_optimize_type(Item_func_signed *i, Analysis & a) const {
        return do_optimize_type_self_and_args(i, a);
    }
} ANONU;
