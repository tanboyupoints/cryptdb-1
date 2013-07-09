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

using namespace std;


CItemTypesDir itemTypes = CItemTypesDir();
CItemFuncDir funcTypes = CItemFuncDir();
CItemFuncNameDir funcNames = CItemFuncNameDir();
CItemSumFuncDir sumFuncTypes = CItemSumFuncDir();


class ANON : public CItemSubtypeIT<Item_field, Item::Type::FIELD_ITEM> {

    virtual RewritePlan * do_gather_type(Item_field *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "FIELD_ITEM do_gather " << *i;

        string fieldname = i->field_name;
        string table = i->table_name;
	string fullname = fullName(fieldname, table);

        FieldMeta * fm = a.getFieldMeta(table, fieldname);

	EncSet es  = EncSet(fm);

	tr = reason(es, "is a field", i);

	return new RewritePlan(es, tr);

    }

    virtual Item *
    do_rewrite_type(Item_field *i,
		    const OLK & constr, const RewritePlan * rp,
		    Analysis & a) const
    {
        LOG(cdb_v) << "do_rewrite_type FIELD_ITEM " << *i;

	FieldMeta *fm = a.getFieldMeta(i->table_name, i->field_name);
	//assert(constr.key == fm);

	//check if we need onion adjustment
        SECLEVEL onion_level = fm->getOnionLevel(constr.o);
        assert(onion_level != SECLEVEL::INVALID);
	if (constr.l < onion_level) {
	    //need adjustment, throw exception
	    throw OnionAdjustExcept(constr.o, fm, constr.l, i);
	}

	Item_field * res = make_item(i);

	if (!fm->isEncrypted()) { // Not encrypted
	    return res;
	}

	// Encrypted item

	res->table_name = make_thd_string(a.getAnonTableName(i->table_name));
	res->field_name = make_thd_string(fm->onions[constr.o]->getAnonOnionName());

        return res;
    }
/*
    static OLK
    chooseProj(FieldMeta * fm) {
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
    do_rewrite_insert_type(Item_field *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
	assert(fm==NULL);
        fm = a.getFieldMeta(i->table_name, i->field_name);

	if (!fm->isEncrypted()) {
	    l.push_back(make_item(i, fm->fname));
	    return;
	}

	// Encrypted field

	Item_field * new_field = NULL;
        for (auto it = fm->onions.begin();
             it != fm->onions.end(); ++it) {
            string name = it->second->getAnonOnionName();
	    new_field = make_item(i, name);
            new_field->table_name =
                make_thd_string(a.getAnonTableName(i->table_name));
            l.push_back(new_field);
        }
        if (fm->has_salt) {
	    assert(new_field); //need an anonymized field as template to create
			       //salt item
            l.push_back(make_item(new_field, fm->saltName()));
        }
    }

} ANON;
