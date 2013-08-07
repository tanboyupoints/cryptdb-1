#pragma once

#include <string>
#include <map>
#include <list>
#include <iostream>
#include <vector>


typedef enum onion {
    oDET,
    oOPE,
    oAGG,
    oSWP,
    oPLAIN,
    oBESTEFFORT,
    oINVALID,
} onion;

//Sec levels ordered such that
// if a is less secure than b.
// a appears before b
// (note, this is not "iff")
#define SECLEVELS(m)    \
    m(INVALID)          \
    m(PLAINVAL)         \
    m(OPEJOIN)          \
    m(OPE)              \
    m(DETJOIN)          \
    m(DET)              \
    m(SEARCH)           \
    m(HOM)              \
    m(RND)              \
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

bool needsSalt(SECLEVEL l);

//Onion layouts - initial structure of onions
typedef std::map<onion, std::vector<SECLEVEL> > onionlayout;

static onionlayout PLAIN_ONION_LAYOUT = {
    {oPLAIN, std::vector<SECLEVEL>({SECLEVEL::PLAINVAL})}
};

static onionlayout NUM_ONION_LAYOUT = {
    {oDET, std::vector<SECLEVEL>({SECLEVEL::DETJOIN, SECLEVEL::DET,
                                  SECLEVEL::RND})},
    {oOPE, std::vector<SECLEVEL>({SECLEVEL::OPE, SECLEVEL::RND})},
    {oAGG, std::vector<SECLEVEL>({SECLEVEL::HOM})},
    {oBESTEFFORT, std::vector<SECLEVEL>({SECLEVEL::PLAINVAL,
                                         SECLEVEL::RND})}
};

static onionlayout STR_ONION_LAYOUT = {
    {oDET, std::vector<SECLEVEL>({SECLEVEL::DETJOIN, SECLEVEL::DET,
                                  SECLEVEL::RND})},
    {oOPE, std::vector<SECLEVEL>({SECLEVEL::OPE, SECLEVEL::RND})},
    {oSWP, std::vector<SECLEVEL>({SECLEVEL::SEARCH})},
    // HACK: RND_str expects the data to be a multiple of 16, so we use
    // DET (it supports decryption UDF) to handle the padding for us.
    {oBESTEFFORT, std::vector<SECLEVEL>({SECLEVEL::PLAINVAL,
                                         SECLEVEL::DET,
                                         SECLEVEL::RND})}
};


typedef std::map<onion, SECLEVEL>  OnionLevelMap;

enum class SECURITY_RATING {SENSITIVE, BEST_EFFORT, PLAIN};
