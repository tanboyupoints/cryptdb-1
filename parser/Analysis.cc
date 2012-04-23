#include <parser/Analysis.hh>

using namespace std;

string ItemMeta::stringify() {
    stringstream res;
    res << "(ItemMeta: onion " << o << " basefield " << stringify_ptr(basefield) << ") ";
    return res.str();
}

EncSet::EncSet() : osl(FULL_EncSet.osl) {}

EncSet
EncSet::intersect(const EncSet & es2) const
{
    OnionLevelFieldMap m;
    for (auto it2 = es2.osl.begin();
         it2 != es2.osl.end(); ++it2) {
        auto it = osl.find(it2->first);
        if (it != osl.end()) {
            SECLEVEL sl = (SECLEVEL)min((int)it->second.first,
                                        (int)it2->second.first);
            if (it->second.second == NULL) {
                m[it->first] = LevelFieldPair(
                        sl, it2->second.second);
            } else if (it2->second.second == NULL) {
                m[it->first] = LevelFieldPair(
                        sl, it->second.second);
            } else if (it->second.second == it2->second.second) {
                m[it->first] = LevelFieldPair(
                        sl, it->second.second);
            }
        }
    }
    return EncSet(m);
}

ostream&
operator<<(ostream &out, const EncSet & es)
{
    if (es.osl.size() == 0) {
        out << "empty encset";
    }
    for (auto it : es.osl) {
        out << "(onion " << it.first
            << ", level " << levelnames[(int)it.second.first]
            << ", field `" << (it.second.second == NULL ? "*" : it.second.second->fname) << "`"
            << ") ";
    }
    return out;
}


EncDesc
EncSet::encdesc() {
    OnionLevelMap olm = OnionLevelMap();
    for (auto it : osl) {
	olm[it.first] = it.second.first;
    }
    return EncDesc(olm);
}

void
EncSet::setFieldForOnion(onion o, FieldMeta * fm) {
    LevelFieldPair lfp = getAssert(osl, o);

    osl[o] = LevelFieldPair(lfp.first, fm);
}

EncSet
EncSet::chooseOne() const
{
    // Order of selection is encoded in this array.
    // The onions appearing earlier are the more preferred ones.
    static const onion onion_order[] = {
        oDET,
        oOPE,
        oAGG,
        oSWP,
	oPLAIN, 
    };

    static size_t onion_size = sizeof(onion_order) / sizeof(onion_order[0]);
    for (size_t i = 0; i < onion_size; i++) {
        auto it = osl.find(onion_order[i]);
        if (it != osl.end()) {
            OnionLevelFieldMap m;
	    onion o = onion_order[i];
            m[o] = it->second;
            return EncSet(m);
        }
    }
    return EncSet(OnionLevelFieldMap());
}
