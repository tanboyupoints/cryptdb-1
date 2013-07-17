#pragma once

#include <main/schema.hh>

// If mkey == NULL, the field is not encrypted
inline void
init_onions_layout(Analysis a, AES_KEY * m_key, FieldMeta * fm,
                   Create_field * cf)
{
    assert(fm->has_salt == static_cast<bool>(m_key));
    assert(0 == fm->onions.size());

    for (auto it: fm->onion_layout) {
        onion o = it.first;
        std::vector<SECLEVEL> levels = it.second;
        OnionMeta * om = new OnionMeta(o, levels, m_key, cf);
        AbstractMetaKey *key =
            new MetaKey<onion>(o, OnionMeta::serializeOnion);
        a.deltas.push_back(Delta(Delta::CREATE, om, fm, key));

        LOG(cdb_v) << "adding onion layer " << om->getAnonOnionName()
                   << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }
}

