#pragma once

#include <main/rewrite_ds.hh>
#include <util/errstream.hh>

#define RETURN_FALSE_IF_FALSE(status)       \
{                                           \
    if (!(status)) {                        \
        return false;                       \
    }                                       \
}

inline void
testBadItemArgumentCount(const std::string &file_name,
                         unsigned int line_number, int type, int expected,
                         int actual)
{
    if (expected != actual) {
        throw BadItemArgumentCount(file_name, line_number, type, expected,
                                   actual);
    }
}

#define TEST_BadItemArgumentCount(type, expected, actual)       \
{                                                               \
    testBadItemArgumentCount(__FILE__, __LINE__, (type),        \
                             (expected), (actual));             \
}

inline void
testUnexpectedSecurityLevel(const std::string &file_name,
                            unsigned int line_number, onion o,
                            SECLEVEL expected, SECLEVEL actual)
{
    if (expected != actual) {
        throw UnexpectedSecurityLevel(file_name, line_number, o,
                                      expected, actual);
    }
}

#define TEST_UnexpectedSecurityLevel(o, expected, actual)           \
{                                                                   \
    testUnexpectedSecurityLevel(__FILE__, __LINE__, (o),            \
                                (expected), (actual));              \
}

// FIXME: Nicer to take reason instead of 'why'.
inline void
testNoAvailableEncSet(const std::string &file_name,
                      unsigned int line_number, const EncSet &enc_set,
                      unsigned int type, const EncSet &req_enc_set,
                      const std::string &why,
                      const RewritePlan *const *const childr_rp,
                      unsigned int child_count)
{
    if (false == enc_set.available()) {
        throw NoAvailableEncSet(file_name, line_number, type,
                                req_enc_set, why, childr_rp, child_count);
    }
}

#define TEST_NoAvailableEncSet(enc_set, type, req_enc_set, why,     \
                               childr_rp, child_count)              \
{                                                                   \
    testNoAvailableEncSet(__FILE__, __LINE__, (enc_set), (type),    \
                          (req_enc_set), (why), (childr_rp),        \
                          (child_count));                           \
}
