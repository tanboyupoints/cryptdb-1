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

#include <main/cdb_rewrite.hh>
#include <main/rewrite_util.hh>
#include <util/cryptdb_log.hh>
#include <main/CryptoHandlers.hh>
#include <parser/lex_util.hh>
#include <main/enum_text.hh>

#include <main/MultiPrinc.hh>

using namespace std;


//TODO: which encrypt/decrypt should handle null?
static Item *
encrypt_item_layers(Item * i, onion o, std::vector<EncLayer *> & layers, Analysis &a, FieldMeta *fm = 0, uint64_t IV = 0) {
    assert(!i->is_null());

    if (o == oPLAIN) {//Unencrypted item
	return i;
    }

    // Encrypted item

    assert_s(layers.size() > 0, "field must have at least one layer");
    Item * enc = i;
    Item * prev_enc = NULL;
    for (auto layer : layers) {
        LOG(encl) << "encrypt layer " << levelnames[(int)layer->level()] << "\n";
	enc = layer->encrypt(enc, IV);
        //need to free space for all enc
        //except the last one
        if (prev_enc) {
            delete prev_enc;
        }
        prev_enc = enc;
    }

    return enc;
}

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
    LOG(cdb_v) << fm->fname << " " << fm->onions.size();

    auto it = a.salts.find(fm);
    salt_type IV = 0;
    if (it != a.salts.end()) {
	IV = it->second;
    }
    return encrypt_item_layers(i, o, fm->onions[o]->layers, a, fm, IV);
}

static void
encrypt_item_all_onions(Item * i, FieldMeta * fm,
                        uint64_t IV, vector<Item*> & l, Analysis &a) {

    for (auto it : fm->onions) {
        l.push_back(encrypt_item_layers(i, it.first, it.second->layers, a, fm, IV));
    }
}
 

template <typename ItemType>
static void
typical_rewrite_insert_type(ItemType *i, Analysis &a, vector<Item *> &l,
                            FieldMeta *fm)
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


static class ANOND : public CItemSubtypeIT<Item_string, Item::Type::STRING_ITEM> {
    virtual RewritePlan * do_gather_type(Item_string *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << " String item do_gather " << *i;
        /* constant strings are always ok */
        for (auto it = a.tmkm.encForVal.begin(); it != a.tmkm.encForVal.end(); it++) {
            if (it->second == "") {
                stringstream temp;
                temp << *i;
                it->second = temp.str();
            }
        }

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
    do_rewrite_insert_type(Item_string *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        typical_rewrite_insert_type(i, a, l, fm);
    }
} ANOND;

static class ANOND : public CItemSubtypeIT<Item_num, Item::Type::INT_ITEM> {
    virtual RewritePlan * do_gather_type(Item_num *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeIT (L966) num do_gather " << *i;
        /* constant ints are always ok */
        for (auto it = a.tmkm.encForVal.begin(); it != a.tmkm.encForVal.end(); it++) {
            if (it->second == "") {
                stringstream temp;
                temp << *i;
                it->second = temp.str();
            }
        }

	tr = reason(FULL_EncSet, "is a constant", i);
	return new RewritePlan(FULL_EncSet_Int, tr);

    }

    virtual Item * do_optimize_type(Item_num *i, Analysis & a) const {
        return i;
    }

    virtual Item * do_rewrite_type(Item_num *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type " << *i << endl;

        return encrypt_item(i, constr, a);
    }

    virtual void
    do_rewrite_insert_type(Item_num *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        typical_rewrite_insert_type(i, a, l, fm);
    }
} ANOND;

static class ANOND : public CItemSubtypeIT<Item_decimal, Item::Type::DECIMAL_ITEM> {
    virtual RewritePlan * do_gather_type(Item_decimal *i, reason &tr, Analysis & a) const {
        LOG(cdb_v) << "CItemSubtypeIT (L1024) decimal do_gather " << *i;
	UNIMPLEMENTED;
        /* constant decimals are always ok */
        return NULL;
    }
        virtual Item * do_optimize_type(Item_decimal *i, Analysis & a) const {
        return i;
    }
    virtual Item * do_rewrite_type(Item_decimal *i,
				   const OLK & constr, const RewritePlan * rp,
				   Analysis & a) const {
        LOG(cdb_v) << "do_rewrite_type L1028";
        double n = i->val_real();
        char buf[sizeof(double) * 2];
        sprintf(buf, "%x", (unsigned int)n);
        // TODO(stephentu): Do some actual encryption of the double here
        return new Item_hex_string(buf, sizeof(buf));
    }
    virtual void
    do_rewrite_insert_type(Item_decimal *i, Analysis & a, vector<Item *> &l, FieldMeta *fm) const
    {
        assert(fm != NULL);
        double n = i->val_real();
        char buf[sizeof(double) * 2];
        sprintf(buf, "%x", (unsigned int)n);
        for (auto it = fm->onions.begin();
             it != fm->onions.end();
             ++it) {
            l.push_back(new Item_hex_string(buf, sizeof(buf)));
        }
        if (fm->has_salt) {
            l.push_back(new Item_hex_string(buf, sizeof(buf)));
        }
        //if no onions, add field as is
        if (l.empty()) {
            l.push_back(new Item_hex_string(buf, sizeof(buf)));
        }
    }
} ANOND;
