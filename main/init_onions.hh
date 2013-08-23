#pragma once

#include <main/schema.hh>

// If mkey == NULL, the field is not encrypted
inline void
init_onions_layout(const AES_KEY * const m_key,
                   FieldMeta * const fm, Create_field * const cf)
{
    assert(fm->has_salt == static_cast<bool>(m_key));
    assert(0 == fm->children.size());

    for (auto it: fm->onion_layout) {
        const onion o = it.first;
        const std::vector<SECLEVEL> levels = it.second;
        // A new OnionMeta will only occur with a new FieldMeta so
        // we never have to build Deltaz for our OnionMetaz.
        std::unique_ptr<OnionMeta>
            om(new OnionMeta(o, levels, m_key, cf, fm->leaseIncUniq()));
        const std::string onion_name = om->getAnonOnionName();
        fm->addChild(new OnionMetaKey(o), std::move(om));

        LOG(cdb_v) << "adding onion layer " << onion_name
                   << " for " << fm->fname;

        //set outer layer
        // fm->setCurrentOnionLevel(o, it.second.back());
    }
}

