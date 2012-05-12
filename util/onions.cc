#include "util/onions.hh"
#include "util/util.hh"

std::ostream&
operator<<(std::ostream &out, const EncDesc & ed)
{
    if (ed.olm.size() == 0) {
        out << "empty encdesc";
    }
    for (auto it : ed.olm) {
        out << "(onion " << it.first
            << ", level " << levelnames[(int)it.second]
            << ") ";
    }
    return out;
}


bool
EncDesc::restrict(onion o, SECLEVEL maxl)
{
    //TODO:
    //assert(maxl is on onion o);

    auto it = olm.find(o);
    assert_s(it != olm.end(), "onion not found in encdesc");

    if (it->second > maxl) {
        it->second = maxl;
        return true;
    }

    return false;
}

EncDesc
EncDesc::intersect(EncDesc & ed) {
    OnionLevelMap new_olm = OnionLevelMap();
    for (auto it: ed.olm) {
	auto jt = olm.find(it.first);
	if (jt != olm.end()) {
	    new_olm[it.first] = std::min(it.second, jt->second);
	}
    }
    return EncDesc(new_olm);
}
