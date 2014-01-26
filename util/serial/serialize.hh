#pragma once

#include <random>

// ##########################
//    conditional typing
// ##########################
// if @Base is a baseclass of @Type, return Type::type, otherwise Type
template <typename Base,
          typename Type,
          typename HazMatch = void>
class CondType;

template <typename Base, typename Type>
class CondType<Base, Type,
               typename std::enable_if<std::is_base_of<Base, Type>::value>::type> {
public:
    typedef typename Type::type type;
};

template <typename Base, typename Type>
class CondType<Base, Type,
               typename std::enable_if<false ==
                                       std::is_base_of<Base, Type>::value>::type> {
public:
    typedef Type type;
};

template <typename T>
class is_vector : public std::false_type {};

template <typename T>
class is_vector<std::vector<T>> : public std::true_type {};

template <typename T>
class is_map : public std::false_type {};

template <typename Key, typename Value>
class is_map<std::map<Key, Value>> : public std::true_type {};

template <typename T>
class is_string : public std::false_type {};

template <>
class is_string<std::string> : public std::true_type {};

template <typename T>
class is_smart : public std::false_type {};

template <typename T>
class is_smart<std::unique_ptr<T> > : public std::true_type {};

template <typename T>
class SmartType {};

template <typename T>
class SmartType<std::unique_ptr<T> > {
public:
    typedef T value;
};


// ##########################
//       serialization
// ##########################

static uint64_t
random64()
{
    // FIXME: not thread safe
    static std::mt19937_64 rng;
    static int one_time = (rng.seed(time(nullptr)), 1);
    (void)one_time;

    return rng();
}

class ID {
    const uint64_t value;

public:
    explicit ID(uint64_t value) : value(value) {}
    ID() : value(random64()) {}
    // uint64_t get() {return this->value;}

    std::string to_string() const {return std::to_string(this->value);}
    friend bool operator<(const ID &lhs, const ID &rhs) {
        return lhs.value < rhs.value;
    }
    friend bool operator==(const ID &lhs, const ID &rhs) {
        return lhs.value == rhs.value;
    }
    friend bool operator!=(const ID &lhs, const ID &rhs) {
        return !(lhs.value == rhs.value);
    }
};

class IDWrapper {
    std::list<ID> ids;

public:
    IDWrapper() {}
    void fn(std::function<void(ID &id)> fn) {
        if (ids.size() > 0) fn(this->ids.front());
    }
    void push(const ID &id) {
        ids.push_front(id);
    }
    void pop() {
        ids.pop_front();
    }
    const std::list<ID> &getIDs() const {return ids;}
};

struct SerializationData {
    SerializationData() : parent_id(IDWrapper()) {}
    void updateGraph(ID &id);

    IDWrapper parent_id;

    // first class citizen encodings
    //                     dependencies, ID -> dependency IDs
    std::map<ID, std::string> encodings;        // used for deserialization
    std::map<ID, std::list<ID> > graph;         // used for writing to database
};

// designates a field to be serialized
template <typename Name, typename Type>
class MemberPair {
public:
    MemberPair() {;}
    // MemberPair(Type value) : value(value) {}
    MemberPair(Type &&value) : value(std::move(value)) {}

    Type value;
    ID id;

    typedef Type type;
    typedef Name name;
};

template <typename Derived, typename MemberTList = Nil>
class Serializable : public Serializable<Derived, typename MemberTList::cdr> {
public:
    typename MemberTList::car member_pair;

    typedef Serializable<Derived, MemberTList> type;
    typedef MemberTList member_type_list;
    typedef Serializable<Derived, typename MemberTList::cdr> next;

    Serializable() {}         // FIXME: should this remain?
    // used for deserialization and arbitrary first time construction
    Serializable(typename MemberTListToTuple<MemberTList>::type &&t,
                 const ID &id = ID())
        : Serializable<Derived, typename MemberTList::cdr >(tuple_cdr(t), id),
          member_pair(std::move(std::get<0>(t))) {}
    // used for deserialization
    // WARN: unsafe -- double move
    Serializable(Serializable<Derived, MemberTList> &&s)
        : Serializable<Derived, typename MemberTList::cdr>(std::move(s)),
          member_pair(std::move(s.member_pair)) {}
    Serializable(const Serializable<Derived, MemberTList> &s)
        : Serializable<Derived, typename MemberTList::cdr>(s),
          member_pair(s.member_pair) {}
    virtual ~Serializable();

    SerializationData serialize() const;
    static Derived *deserialize(const ID &id,
                                const std::map<ID, std::string> &encodings);

    template <typename NamesTList, typename... Args>
    static typename MemberTListToTuple<MemberTList>::type
    expandAndTuple(Args... a)
    {
        return delegate<NamesTList, MemberTList, Args...>(a...);
    }

    // -------------------------------------------------
    // 'delegate' handles iteration through A's Member TypeList

    // instantiated when the head of the member's list is in our names list
    // thus implicitly our next Arg
    template <typename NamesTList, typename MTList,
              typename HeadArgs, typename... TailArgs>
    static typename std::enable_if<Haz<typename MTList::car::name,
                                       NamesTList>::value
                                   && TypeListLength<MTList>::value != 0,
                                   typename MemberTListToTuple<MTList>::type>::type
    delegate(HeadArgs head_args, TailArgs... tail_args)
    {
        return std::tuple_cat(std::make_tuple(head_args),
                              delegate<NamesTList, typename MTList::cdr,
                                       TailArgs...>(tail_args...));
    }

    // instantiated when the next Arg was not explicitly passed by caller
    template <typename NamesTList, typename MTList, typename... Args>
    static typename
        std::enable_if<false == Haz<typename MTList::car::name,
                                    NamesTList>::value
                        && TypeListLength<MTList>::value != 0,
                       typename MemberTListToTuple<MTList>::type>::type
    delegate(Args... args)
    {
        return std::tuple_cat(std::make_tuple(typename MTList::car::type()),
                              delegate<NamesTList, typename MTList::cdr,
                                       Args...>(args...));
    }

    // instantiated when there are no more Members
    template <typename NamesTList, typename MTList, typename... Args>
    static typename
        std::enable_if<0 == TypeListLength<MTList>::value,
                       std::tuple<> >::type
    delegate(Args... args)
    {
        static_assert(0 == sizeof...(Args),
                      "bad args to expandAndTuple!");
        return std::make_tuple();
    }
    // -------------------------------------------------
};

template <typename Derived>
class Serializable<Derived, Nil> {
    const ID id;

public:
    typedef Nil member_type_list;

    Serializable() {}
    Serializable(std::tuple<> &&, const ID &id)
        : id(id) {}

    const ID &getID() const {return id;}
};

template <typename Type>
class is_serializable {
public:
    static const bool value =
        std::is_base_of<Serializable<Type, Nil>, Type>::value;
};

std::string
serializeType(const std::string &s, SerializationData &);

std::string
serializeType(int i, const SerializationData &);

template <typename Type>
std::string
serializeType(const std::unique_ptr<Type> &ptr, SerializationData &sdata);

template <typename Type>
std::string
serializeType(const std::vector<Type> &vec, SerializationData &sdata);

template <typename Key, typename Value>
std::string
serializeType(const std::map<Key, Value> &map, SerializationData &sdata);

template <typename Derived, typename... MemberPairTail>
std::string
serializeType(const Serializable<Derived, Cons<MemberPairTail... > > &s,
              SerializationData &sdata);

template <typename Type>
typename std::enable_if<is_string<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings);

template <typename Type>
typename std::enable_if<std::is_arithmetic<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings);

template <typename Type>
typename std::enable_if<is_smart<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings);

template <typename Type>
typename std::enable_if<is_vector<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings);

template <typename Type>
typename std::enable_if<is_map<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings);

template <typename Type>
typename std::enable_if<is_serializable<Type>::value, Type>::type
deserializeType(const std::string &serial,
                const std::map<ID, std::string> &encodings);


