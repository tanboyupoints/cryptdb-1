#pragma once

#define RETURN_FALSE_IF_FALSE(status)       \
{                                           \
    if (!(status)) {                        \
        return false;                       \
    }                                       \
}


