#pragma once

#include <string>


/*
 * Onion layout
 * - for certain types of fields, we use a set of certain onions --> an
 * onion layout
 *
 * OLID_NUM : has three onions:
 *     DET (plain, join, det, rnd),
 *     OPE (plain, ope, rnd), and
 *     HOM (plain, hom)
 *
 * OLID_TEXT: has three onions:
 *     DET (plain, join, det, rnd),
 *     OPE (plain, ope, rnd), and
 *     SEARCH (plain, SWP)
 *
 */


typedef enum OnionLayoutId {
    OLID_NUM,
    OLID_TEXT
} OnionLayoutId;



typedef enum onion {
    oDET,
    oOPE,
    oAGG,
    oNONE,
    oSWP,
    oINVALID,
} onion;

#define SECLEVELS(m)    \
    m(INVALID)          \
    m(PLAIN)            \
    m(PLAIN_DET)        \
    m(DETJOIN)          \
    m(DET)              \
    m(SEMANTIC_DET)     \
    m(PLAIN_OPE)        \
    m(OPEJOIN)          \
    m(OPE)              \
    m(SEMANTIC_OPE)     \
    m(PLAIN_AGG)        \
    m(SEMANTIC_AGG)     \
    m(PLAIN_SWP)        \
    m(SWP)              \
    m(SEMANTIC_VAL)     \
    m(SECLEVEL_LAST)

typedef enum class SECLEVEL {
#define __temp_m(n) n,
SECLEVELS(__temp_m)
#undef __temp_m
} SECLEVEL;

const std::string levelnames[] = {
#define __temp_m(n) #n,
SECLEVELS(__temp_m)
#undef __temp_m
};

inline SECLEVEL string_to_sec_level(const std::string &s)
{
#define __temp_m(n) if (s == #n) return SECLEVEL::n;
SECLEVELS(__temp_m)
#undef __temp_m
    // TODO: possibly raise an exception
    return SECLEVEL::INVALID;
}

//decreases given sec level on the same onion
SECLEVEL
decreaseLevel(SECLEVEL l, OnionLayoutId ft, onion o);

//increases given sec level on the same onion
SECLEVEL
increaseLevel(SECLEVEL l, OnionLayoutId ft, onion o);

//returns onion for given level
onion
getOnion(SECLEVEL l);

//returns max and min levels on onion o
SECLEVEL getMax(onion o);
SECLEVEL getMin(onion o);

SECLEVEL getLevelPlain(onion o);


