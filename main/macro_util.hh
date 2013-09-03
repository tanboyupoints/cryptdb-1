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
