#pragma once

#include <string>


typedef enum onion {
    oDET,
    oOPE,
    oAGG,
    oSWP,
    oINVALID,
} onion;

#define SECLEVELS(m)    \
    m(INVALID)          \
    m(PLAIN)            \
    m(DET)              \
    m(DETJOIN)          \
    m(RND)              \
    m(OPE)              \
    m(OPEJOIN)          \
    m(HOM)   	        \
    m(SEARCH)           \
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





