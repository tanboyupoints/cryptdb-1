#pragma once

#include <main/schema.hh>

// If mkey == NULL, the field is not encrypted
static void
init_onions_layout(AES_KEY * mKey, FieldMeta * fm, uint index,
                   Create_field * cf, onionlayout ol) {

    fm->onions.clear();

    // This additional reflection is needed as we must rebuild the
    // OnionMeta's (and their layers) after a restart.
    fm->onion_layout = ol;

    for (auto it: ol) {
        onion o = it.first;
        OnionMeta * om = new OnionMeta(o, index, fm->fname);
        fm->onions[o] = om;

        if (mKey) {
	    Create_field * newcf = cf;
            //generate enclayers for encrypted field
            string uniqueFieldName = fm->fullName(o);
            for (auto l: it.second) {
                string key;
                key = getLayerKey(mKey, uniqueFieldName, l);

		EncLayer * el = EncLayerFactory::encLayer(o, l, newcf, key);

		Create_field * oldcf = newcf;
		newcf = el->newCreateField(oldcf);
		
                om->layers.push_back(el);

		if (oldcf != cf) {
		    delete oldcf;
		}
            }
	    if (newcf != cf) {
		delete newcf;
	    }
        }

        LOG(cdb_v) << "adding onion layer " << om->getAnonOnionName() << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }
}

// @index: default value 0
static void
init_onions(AES_KEY * mKey, FieldMeta * fm, Create_field * cf,
            uint index=0) {
    if (!mKey) {
        // unencrypted field
        init_onions_layout(NULL, fm, 0, cf, PLAIN_ONION_LAYOUT);
        fm->has_salt = false;
        return;
    }

    // Encrypted field

    fm->has_salt = true;

    if (IsMySQLTypeNumeric(cf->sql_type)) {
        init_onions_layout(mKey, fm, index, cf, NUM_ONION_LAYOUT);
    } else {
        init_onions_layout(mKey, fm, index, cf, STR_ONION_LAYOUT);
    }
}


