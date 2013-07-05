#pragma once

/*
 * ANON generates a unique identifier, useful in cases when the
 * name of some object or class doesn't matter.
 */

#include "string.h"

#define ANON_CONCAT2(a, b)  a ## b
#define ANON_CONCAT(a, b)   ANON_CONCAT2(a, b)
#define ANON                ANON_CONCAT(__anon_id_, __COUNTER__)
// counter is file specific so need to define a differnet anonf for
// rewrite_field
// we would only have one ANON if  __FILE__ would not print whole path
#define ANONF                ANON_CONCAT(__anon_id_f_, __COUNTER__)
#define ANONU                ANON_CONCAT(__anon_id_func_, __COUNTER__)

/*
 * Cleanup handler object: invokes the supplied function in its
 * destructor.
 */
template<class T>
class cleanup_caller {
 public:
    cleanup_caller(T a) : action(a) {}
    ~cleanup_caller() { action(); }

 private:
    T action;
};

template<class T>
cleanup_caller<T>
cleanup(T a)
{
    return cleanup_caller<T>(a);
}
