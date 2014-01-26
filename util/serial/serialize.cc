// the metafunctions primarily deal with three inputs, std::tuple<...>, 
// TypeList<Type> (where Type is _not_ MemberPair) and TypeList<MemberPair>
// (where MemberPair<Name, MemberType>)
//
// in all three cases the types of a Serializable's datamembers are encoded;
// the naming convention is as such
// std::tuple<...>      : Tuple
// TypeList<Type>       : SimpleTypeList (or SimpleTList)
// TypeList<MemberPair> : MemberTypeList (or MemberTList)
//
// the same convention is followed in utils.hh
//
//
// Other Notes
// > serializeType(...) should always produce a singular encoding
// > if additional functionality is needed on Serializable to get deserialization
//   to do something; consider splitting Serializable into ConcreteSerializable
//   and AbstractSerializable
//
//
// Serialization Contract
// > any class inheriting from Serialization must implement a move constructor
//   that takes it's base class (the Serialization class) as an argument; for
//   other constructors it must use Serialization::expandAndTuple(...) in
//   conjunction with the Serialization tuple constructor

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <assert.h>
#include <functional>
#include <memory>
#include <iomanip>
#include <type_traits>

#include "utils.hh"
#include "tuple_cdr.hh"
#include "serialize.hh"

void test();


#ifdef ISLAND
int
main()
{
    test();
    return 0;
}
#endif


// ##########################
//        serialization
// ##########################

// taken from cryptdb
static std::string
serialEncode(const std::string &s)
{
    return std::string(std::to_string(s.length()) + "_" + s);
}

std::string
serializeType(const std::string &s, SerializationData &)
{
    return serialEncode(s);
}

std::string
serializeType(int i, const SerializationData &)
{
    return serialEncode(std::to_string(i));
}

static std::string
serializeNull()
{
    return serialEncode("NULL");
}

template <typename Type>
std::string
serializeType(const std::unique_ptr<Type> &ptr, SerializationData &sdata)
{
    if (NULL == ptr) {
        return serializeNull();
    }

    // if 'Type' is derived from Serializable we want to dispatch to the
    // serializeType for Serializablez; since C++ only provides virtual dispatch
    // for member functions we must convert the object to Serializable static type
    const typename CondType<Serializable<Type>, Type>::type *inner = ptr.get();
    return serializeType(*inner, sdata);
}

template <typename Type>
std::string
serializeType(const std::vector<Type> &vec, SerializationData &sdata)
{
    std::string serial;
    for (const auto &it : vec) {
        const typename CondType<Serializable<Type>, Type>::type &inner = it;
        serial += serializeType(inner, sdata);
    }

    return serialEncode(serial);
}

template <typename Key, typename Value>
std::string
serializeType(const std::map<Key, Value> &map, SerializationData &sdata)
{
    std::string serial;
    for (const auto &it : map) {
        const typename CondType<Serializable<Key>, Key>::type &inner_key =
            it.first;
        const typename CondType<Serializable<Value>, Value>::type &inner_value =
            it.second;

        serial += serialEncode(serializeType(inner_key, sdata)
                               + serializeType(inner_value, sdata));
    }

    return serialEncode(serial);
}

// base case
template <typename Derived>
std::string
serializeHelper(const Serializable<Derived, Nil> &, SerializationData &)
{
    return "";
}

template <typename Derived, typename HeadName,
                            typename HeadType,
                            typename MemberPairTail>
std::string
serializeHelper(const Serializable<Derived,
                             Cons<MemberPair<HeadName, HeadType>,
                                  MemberPairTail> > &s,
                SerializationData &sdata)
{
    return serializeType(s.member_pair.value, sdata) +
        serializeHelper(static_cast<const Serializable<Derived,
                                                       MemberPairTail> &>(s),
                        sdata);
}

template <typename Derived, typename... MemberPairs>
std::string
serializeType(const Serializable<Derived, Cons<MemberPairs...> > &s,
              SerializationData &sdata)
{
    // register as a child of some other citizen
    sdata.parent_id.fn(
        [&sdata, &s] (const ID &id) -> void {sdata.graph[id].push_back(s.id);});

    // graph: add new first class citizen
    assert(sdata.graph.end() == sdata.graph.find(s.id));
    sdata.graph.insert(std::make_pair(s.id, std::list<ID>()));

    // serialize the fields
    // > make this current object the 'parent' object
    sdata.parent_id.push(s.id);
    const std::string &field_encoding = serializeHelper(s, sdata);
    sdata.parent_id.pop();

    // encodings: add new first class citizen
    assert(sdata.encodings.end() == sdata.encodings.find(s.id));
    sdata.encodings.insert(std::make_pair(s.id, field_encoding));

    return serialEncode(s.id.to_string());
}

template <typename Derived, typename MemberTList>
SerializationData
Serializable<Derived, MemberTList>::serialize() const
{
    SerializationData sdata;
    serializeType(*this, sdata);

    return sdata;
}

template <typename MemberName, typename MemberType, typename Derived,
          typename Cdr>
MemberType &
atKey(Serializable<Derived,
                   Cons<MemberPair<MemberName, MemberType>, Cdr> > &s)
{
    return s.member_pair.value;
}

template <typename MemberName, typename MemberType, typename Derived,
          typename Cdr>
std::string
atKeyID(Serializable<Derived,
                     Cons<MemberPair<MemberName, MemberType>, Cdr> > &s)
{
    return s.member_pair.id.to_string();
}

// ##########################
//      deserialization
// ##########################
// taken from CryptDB
inline std::vector<std::string>
unserialize_string(std::string serial)
{
    std::vector<std::string> output;
    std::size_t start = 0;
    std::size_t under_pos = serial.find_first_of("_");
    while (under_pos != std::string::npos) {
        const std::size_t length =
            atoi(serial.substr(start, under_pos-start).c_str());
        output.push_back(serial.substr(under_pos+1, length)); 
        start = under_pos + 1 + length;
        under_pos = serial.find_first_of("_", start);
    }

    return output;
}

template <typename Type>
typename std::enable_if<is_string<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings)

{
    (void)encodings;
    return std::string(serial);
}

template <typename Type>
typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings)
{
    (void)encodings;
    return atoi(serial.c_str());
}

static bool
nullEncoding(const std::string &serial)
{
    return serial == unserialize_string(serializeNull()).back();
}

template <typename Type>
typename std::enable_if<is_smart<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings)
{
    if (nullEncoding(serial)) {
        return std::unique_ptr<typename SmartType<Type>::value>(nullptr);
    }

    // FIXME: generalize
    return
        std::unique_ptr<typename SmartType<Type>::value>(
                new typename SmartType<Type>::value(
                    deserializeType<typename SmartType<Type>::value>(serial,
                                                                     encodings)));
}

template <typename Type>
typename std::enable_if<is_vector<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings)
{
    Type out_v;
    auto elements = unserialize_string(serial);
    for (auto &it : elements) {
        out_v.push_back(deserializeType<typename Type::value_type>(it,
                                                                   encodings));
    }

    return out_v;
}

template <typename Type>
typename std::enable_if<is_map<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings)
{
    Type out_map;
    const auto &elements = unserialize_string(serial);
    for (const auto &it : elements) {
        const auto &key_and_value = unserialize_string(it);
        assert(2 == key_and_value.size());

        out_map.insert(std::make_pair(
            deserializeType<typename Type::key_type>(key_and_value[0], encodings),
            deserializeType<typename Type::mapped_type>(key_and_value[1],
                                                        encodings)));
    }

    return out_map;
}

// -------------------------------------------------
// NOTE: The order of doDeserialization templates matters
template <typename DerivedBase>
typename std::enable_if<std::is_same<Nil,
                                    typename DerivedBase::member_type_list>::value,
                        std::tuple<> >::type
doDeserialization(const std::vector<std::string> &empty_untypeds,
                  const std::map<ID, std::string> &encodings)
{
    (void)encodings;

    assert(0 == empty_untypeds.size());
    return std::make_tuple();
}

template<typename DerivedBase>
typename std::enable_if<false
                        == std::is_same<Nil,
                                typename DerivedBase::member_type_list>::value,
                        typename MemberTListToTuple<
                            typename DerivedBase::member_type_list>::type>::type
doDeserialization(const std::vector<std::string> &untyped,
                  const std::map<ID, std::string> &encodings)
{
    assert(untyped.size() > 0);
    typedef typename DerivedBase::member_type_list MemberTList;

    typename MemberTList::car::type head =
        deserializeType<typename MemberTList::car::type>(untyped[0], encodings);
    return std::tuple_cat(std::make_tuple(std::move(head)),
                          doDeserialization<typename DerivedBase::next>(
                              std::vector<std::string>(untyped.begin() + 1,
                                                       untyped.end()),
                              encodings));
}

template <typename Type>
typename std::enable_if<is_serializable<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings)
{
    typedef Serializable<Type, typename Type::member_type_list> DerivedBase;

    // serial is the ID for the first class citizen we want to deserialize
    ID id(strtoull(serial.c_str(), nullptr, 0));

    const auto &enc = encodings.find(id);
    assert(enc != encodings.end());

    const auto &vec = unserialize_string(enc->second);
    auto t = doDeserialization<DerivedBase>(vec, encodings);

    // use the tuple constructor and the move constructor
    return Type(DerivedBase(std::move(t)));
}
// -------------------------------------------------

template<typename Derived, typename MemberTList>
Serializable<Derived, MemberTList>::~Serializable() {}

// @id        : is the root first class citizen ID
// @encodings : ID to serial mapping for all first class citizens
template<typename Derived, typename MemberTList>
Derived *
Serializable<Derived, MemberTList>::deserialize(const ID &id,
                                                const std::map<ID, std::string>
                                                    &encodings)
{
    auto it = encodings.find(id);
    assert(encodings.end() != it);

    // use the move constructor
    return new Derived(deserializeType<Derived>(id.to_string(), encodings));
}

// ##########################
//        misc helpers
// ##########################

#include "serialize_tests.cpp"

// must come after the inclusion of serialize_tests.cpp
void test() {runTests();}
