#pragma once

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
