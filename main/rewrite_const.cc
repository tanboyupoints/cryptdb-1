/*
 * Handlers for rewriting constants, integers, strings, decimals, date, etc. 
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

// class/object names we don't care to know the name of
#define ANON                ANON_NAME(__anon_id_const)

// encrypts a constant item based on the information in a
static Item *
encrypt_item(Item * i, const OLK & olk, Analysis & a)
{
    assert(!i->is_null());

    if (olk.l == SECLEVEL::PLAINVAL)
        return i;

    FieldMeta * fm = olk.key;
    assert(fm);

    onion o        = olk.o;
    LOG(cdb_v) << fm->fname << " " << fm->children.size();

    auto it = a.salts.find(fm);
    salt_type IV = 0;
    if (it != a.salts.end()) {
	IV = it->second;
    }
    OnionMeta *om = fm->getOnionMeta(o);
    Item *ret_i = encrypt_item_layers(i, o, om, a, fm, IV);

    return ret_i;
}

static void
encrypt_item_all_onions(Item * i, FieldMeta * fm,
                        uint64_t IV, std::vector<Item*> & l,
                        Analysis &a)
{

    for (auto it : fm->orderedOnionMetas()) {
        onion o = it.first->getValue();
        OnionMeta *om = it.second;
        l.push_back(encrypt_item_layers(i, o, om, a, fm, IV));
    }
}
 
template <typename ItemType>
static void
typical_rewrite_insert_type(ItemType *i, Analysis &a,
                            std::vector<Item *> &l, FieldMeta *fm)
{
    if (!fm->isEncrypted()) {
        l.push_back(make_item(i));
        return;
    }

    // Encrypted

    uint64_t salt = 0;

    if (fm->has_salt) {
        salt = randomValue();
    } else {
        //TODO: need to use table salt in this case
    }

    encrypt_item_all_onions(i, fm, salt, l, a);

    if (fm->has_salt) {
        l.push_back(new Item_int((ulonglong) salt));
    }
}


static class ANON : public CItemSubtypeIT<Item_string, Item::Type::STRING_ITEM> {
    virtual RewritePlan * do_gather_type(Item_string *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << " String item do_gather " << *i;
        /* constant strings are always ok */
       
	tr = reason(FULL_EncSet, "is a constant", i);
	return new RewritePlan(FULL_EncSet_Str, tr);

    }
    virtual Item * do_optimize_type(Item_string *i, Analysis & a) const {
        return i;
    }

    virtual Item * do_rewrite_type(Item_string *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type String item " << *i;

        return encrypt_item(i, constr, a);
    }

    virtual void
    do_rewrite_insert_type(Item_string *i, Analysis & a,
                           std::vector<Item *> &l, FieldMeta *fm) const
    {
        typical_rewrite_insert_type(i, a, l, fm);
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_num, Item::Type::INT_ITEM> {
    virtual RewritePlan * do_gather_type(Item_num *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeIT (L966) num do_gather " << *i;
        /* constant ints are always ok */
  	tr = reason(FULL_EncSet, "is a constant", i);
	return new RewritePlan(FULL_EncSet_Int, tr);

    }

    virtual Item * do_optimize_type(Item_num *i, Analysis & a) const {
        return i;
    }

    virtual Item * do_rewrite_type(Item_num *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type " << *i << std::endl;

        return encrypt_item(i, constr, a);
    }

    virtual void
    do_rewrite_insert_type(Item_num *i, Analysis & a,
                           std::vector<Item *> &l, FieldMeta *fm) const
    {
        typical_rewrite_insert_type(i, a, l, fm);
    }
} ANON;

static class ANON : public CItemSubtypeIT<Item_decimal, Item::Type::DECIMAL_ITEM> {
    virtual RewritePlan * do_gather_type(Item_decimal *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeIT decimal do_gather " << *i;

	tr = reason(FULL_EncSet, "is a constant", i);
        return new RewritePlan(FULL_EncSet_Int, tr);
    }
    virtual Item * do_optimize_type(Item_decimal *i, Analysis & a) const {
        return i;
    }
    virtual Item * do_rewrite_type(Item_decimal *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type " << *i << std::endl;

	return encrypt_item(i, constr, a);
/*        double n = i->val_real();
        char buf[sizeof(double) * 2];
        sprintf(buf, "%x", (unsigned int)n);
        // TODO(stephentu): Do some actual encryption of the double here
        return new Item_hex_string(buf, sizeof(buf));*/
    }
    virtual void
    do_rewrite_insert_type(Item_decimal *i, Analysis & a,
                           std::vector<Item *> &l, FieldMeta *fm) const
    {
	typical_rewrite_insert_type(i, a, l, fm);
    }
} ANON;
