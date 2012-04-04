#pragma once

#include <string>
#include <map>
#include <list>


typedef enum onion {
    oDET,
    oOPE,
    oAGG,
    oSWP,
    oINVALID,
    oPLAIN, 
} onion;

//Sec levels ordered such that
// if a is less secure than b.
// a appears before b
// (note, this is not "iff")
#define SECLEVELS(m)    \
    m(INVALID)          \
    m(PLAINVAL)            \
    m(OPEJOIN)     	\
    m(OPE)		\
    m(DETJOIN)          \
    m(DET)              \
    m(SEARCH)           \
    m(HOM)              \
    m(RND) 		\
    m(SECLEVEL_LAST)

typedef enum class SECLEVEL {
#define __temp_m(n) n,
SECLEVELS(__temp_m)
#undef __temp_m
} SECLEVEL;

//TODO: what is seclevel_last needed for?

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

//Onion layouts - initial structure of onions
typedef std::map<onion, std::list<SECLEVEL> > onionlayout;

static onionlayout NUM_ONION_LAYOUT = {
    {oDET, std::list<SECLEVEL>({SECLEVEL::DETJOIN, SECLEVEL::DET, SECLEVEL::RND})},
    {oOPE, std::list<SECLEVEL>({SECLEVEL::OPE, SECLEVEL::RND})},
    {oAGG, std::list<SECLEVEL>({SECLEVEL::HOM})}
};
    
static onionlayout STR_ONION_LAYOUT = {
    {oDET, std::list<SECLEVEL>({SECLEVEL::DETJOIN, SECLEVEL::DET, SECLEVEL::RND})},
    {oOPE, std::list<SECLEVEL>({SECLEVEL::OPE, SECLEVEL::RND})},
    {oAGG, std::list<SECLEVEL>({SECLEVEL::SEARCH})}
};




