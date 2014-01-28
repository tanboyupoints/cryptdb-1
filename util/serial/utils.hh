#pragma once

#include <typeinfo>
#include <iostream>

#if 0
#include <cxxabi.h>

template <typename Type>
void
printType()
{
    int status;
    char *realname;
    const std::type_info  &ti = typeid(Type);

    realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    // std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
    std::cout << realname << "\t: " << status << '\n';
    free(realname);
}
#endif
// -------------------------------------------------
template <bool If, typename Then, typename Else>
class IfElse;

template <typename Then, typename Else>
class IfElse<true, Then, Else> {
public:
    typedef Then type;
};

template <typename Then, typename Else>
class IfElse<false, Then, Else> {
public:
    typedef Else type;
};
// -------------------------------------------------
// -------------------------------------------------
// basic cons list for manipulating types
class Nil; /* {
public:
    typedef Nil car;
    typedef Nil cdr;
};
*/

template <typename Car, typename Cdr>
class Cons {
public:
    typedef Car car;
    typedef Cdr cdr;
};

template <typename... All>
class TypeList_;

template <typename Head, typename... Tail>
class TypeList_<Head, Tail...> {
public:
    typedef Cons<Head, typename TypeList_<Tail... >::type > type;
};

template <>
class TypeList_<> {
public:
    typedef Nil type;
};

template <typename... All>
using TypeList = typename TypeList_<All...>::type;
// -------------------------------------------------
// -------------------------------------------------
// compute the length of a TypeList
template <typename Cons>
class TypeListLength {
public:
    enum {value = 1 + TypeListLength<typename Cons::cdr>::value};
};

template <>
class TypeListLength<Nil> {
public:
    enum {value = 0};
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// get the tuple type matching the TypeList
template <typename Cons, typename... Types>
class TypeListToTuple {
public:
    typedef typename TypeListToTuple<typename Cons::cdr,
                                     Types...,
                                     typename Cons::car>::type
        type;
};

template <typename... Types>
class TypeListToTuple<Nil, Types...> {
public:
    typedef std::tuple<Types...> type;
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// get the TypeList matching the tuple type
template <typename... Tuple>
class TupleToTypeList;

template <>
class TupleToTypeList<std::tuple<> > {
public:
    typedef Nil type;
};

template <typename TupleHead, typename... TupleTail>
class TupleToTypeList<std::tuple<TupleHead, TupleTail... > > {
public:
    typedef Cons<TupleHead,
            typename TupleToTypeList<std::tuple<TupleTail... > >::type > type;
};

// -------------------------------------------------
// -------------------------------------------------
// take a TypeList of MemberPairz and create a TypeList of the MemberPairz types
template <typename MemberTList>
class MemberTListToSimpleTList {
public:
    typedef Cons<typename MemberTList::car::type,
                 typename MemberTListToSimpleTList<typename MemberTList::cdr>::type>
        type;
};

template <>
class MemberTListToSimpleTList<Nil> {
public:
    typedef Nil type;
};
// -------------------------------------------------
// -------------------------------------------------
// @MemberTList should be a TypeList of MemberPairz
// > create a tuple type of the MemberPair types
template <typename MemberTList>
using MemberTListToTuple =
    TypeListToTuple<typename MemberTListToSimpleTList<MemberTList>::type >;
// -------------------------------------------------
// -------------------------------------------------
// @AttemptName should be an identifier that @TList will be scanned for
// @TList should be a simple TypeList of identifiers
template <typename AttemptName,
          typename SimpleTList>
class Haz {
public:
    typedef
        typename IfElse<std::is_same<AttemptName,
                                     typename SimpleTList::car>::value,
                        std::true_type,
                        typename Haz<AttemptName,
                                     typename SimpleTList::cdr>::type_>::type
        type_;
    static const bool value = type_::value;
};

template <typename AttemptName>
class Haz<AttemptName, Nil> {
public:
    typedef std::false_type type_;
    static const bool value = type_::value;
};
// -------------------------------------------------
// -------------------------------------------------
template <typename AttemptName,
          typename MemberTList,
          typename HazMatch = void>
class Get;

template <typename AttemptName, typename MemberTList>
class Get<AttemptName, MemberTList,
          typename
            std::enable_if<std::is_same<AttemptName,
                                typename MemberTList::car::name>::value>::type> {
public:
    typedef typename MemberTList::car::type type;
};

template <typename AttemptName, typename MemberTList>
class Get<AttemptName, MemberTList,
          typename
            std::enable_if<false ==
                           std::is_same<AttemptName,
                                typename MemberTList::car::name>::value>::type> {
public:
    typedef typename Get<AttemptName, typename MemberTList::cdr>::type type;
};
// -------------------------------------------------

