#include "util/onions.hh"
#include "util/util.hh"

onion
getOnion(SECLEVEL l1)
{
    switch (l1) {
    case SECLEVEL::PLAIN_DET: {return oDET; }
    case SECLEVEL::DETJOIN: {return oDET; }
    case SECLEVEL::DET: {return oDET; }
    case SECLEVEL::SEMANTIC_DET: {return oDET; }
    case SECLEVEL::PLAIN_OPE: {return oOPE; }
    case SECLEVEL::OPEJOIN: {return oOPE; }
    case SECLEVEL::OPE: {return oOPE; }
    case SECLEVEL::SEMANTIC_OPE: {return oOPE; }
    case SECLEVEL::PLAIN_AGG: {return oAGG; }
    case SECLEVEL::SEMANTIC_AGG: {return oAGG; }
    case SECLEVEL::PLAIN_SWP: {return oSWP; }
    case SECLEVEL::SWP: {return oSWP; }
    case SECLEVEL::PLAIN: {return oNONE; }
    default: {return oINVALID; }
    }
    return oINVALID;
}


SECLEVEL
decreaseLevel(SECLEVEL l, OnionLayoutId ft,  onion o)
{
    switch (o) {
    case oDET: {
        switch (l) {
        case SECLEVEL::SEMANTIC_DET: {return SECLEVEL::DET; }
        case SECLEVEL::DET: {
            return SECLEVEL::DETJOIN;
        }
        case SECLEVEL::DETJOIN: {return SECLEVEL::PLAIN_DET; }
        default: {
            assert_s(false, "cannot decrease level");
            return SECLEVEL::INVALID;
        }
        }
    }
    case oOPE: {
        switch (l) {
        case SECLEVEL::SEMANTIC_OPE: {return SECLEVEL::OPE; }
        case SECLEVEL::OPE: {
            if (ft == OLID_NUM) {
                return SECLEVEL::OPEJOIN;
            } else {
                return SECLEVEL::PLAIN_OPE;
            }
        }
        case SECLEVEL::OPEJOIN: {return SECLEVEL::PLAIN_OPE;}
        default: {
            assert_s(false, "cannot decrease level");
            return SECLEVEL::INVALID;
        }
        }
    }
    case oAGG: {
        switch (l) {
        case SECLEVEL::SEMANTIC_AGG: {return SECLEVEL::PLAIN_AGG; }
        default: {
            assert_s(false, "cannot decrease level");
            return SECLEVEL::INVALID;
        }
        }
    }
    case oSWP: {
            assert_s(l == SECLEVEL::SWP, "cannot decrease level for other than level SWP on the SWP onion");
            return SECLEVEL::PLAIN_SWP;
    }
    default: {
        assert_s(false, "cannot decrease level");
        return SECLEVEL::INVALID;
    }
    }

}

SECLEVEL
increaseLevel(SECLEVEL l, OnionLayoutId ft, onion o)
{
    switch (o) {
    case oDET: {
        switch (l) {
        case SECLEVEL::DET:         return SECLEVEL::SEMANTIC_DET;
        case SECLEVEL::DETJOIN:     return SECLEVEL::DET;
        case SECLEVEL::PLAIN_DET:   return SECLEVEL::DETJOIN;
        default: {
            assert_s(false, "cannot increase level");
            return SECLEVEL::INVALID;   // unreachable
        }
        }
    }
    case oOPE: {
        switch (l) {
        case SECLEVEL::OPE: {return SECLEVEL::SEMANTIC_OPE; }
        case SECLEVEL::PLAIN_OPE: {
            if (ft == OLID_NUM) {
                return SECLEVEL::OPEJOIN;
            } else {
                return SECLEVEL::OPE;
            }
        }
        case SECLEVEL::OPEJOIN: {return SECLEVEL::OPE;}
        default: {
            assert_s(false, "cannot increase level");
            return SECLEVEL::INVALID;
        }
        }
    }
    case oAGG: {
        switch (l) {
        case SECLEVEL::PLAIN_AGG: {return SECLEVEL::SEMANTIC_AGG; }
        default: {
            assert_s(false, "cannot increase level");
            return SECLEVEL::INVALID;
        }
        }
    }
    case oSWP: {
        assert_s(l == SECLEVEL::PLAIN_SWP,  "cannot increase beyond SWP");
        return SECLEVEL::SWP;
    }
    default: {
        assert_s(false, "cannot increase level");
        return SECLEVEL::INVALID;
    }
    }

}


SECLEVEL
getMin(onion o) {
    switch (o) {
    case oDET: {return SECLEVEL::PLAIN_DET;}
    case oOPE: {return SECLEVEL::PLAIN_OPE;}
    case oAGG: {return SECLEVEL::PLAIN_AGG;}
    case oSWP: {return SECLEVEL::PLAIN_SWP;}
    default: {}
    }
    assert_s(false, "invalid onion");
    return SECLEVEL::INVALID;
    
}


SECLEVEL
getMax(onion o) {
    switch (o) {
    case oDET: {return SECLEVEL::SEMANTIC_DET;}
    case oOPE: {return SECLEVEL::SEMANTIC_OPE;}
    case oAGG: {return SECLEVEL::SEMANTIC_AGG;}
    case oSWP: {return SECLEVEL::SWP;}
    default: {}
    }

    assert_s(false, "invalid onion");
    return SECLEVEL::INVALID;
}

SECLEVEL
getLevelPlain(onion o)
{
    switch (o) {
    case oAGG: {return SECLEVEL::PLAIN_AGG; }
    case oDET: { return SECLEVEL::PLAIN_DET; }
    case oOPE: { return SECLEVEL::PLAIN_OPE; }
    case oNONE: {return SECLEVEL::PLAIN; }
    default: {assert_s(false, "invalid onion type in getLevelForOnion"); }
    }

    return SECLEVEL::INVALID;
}
