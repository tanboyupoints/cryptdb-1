#include <memory>

#include "testing.hh"
#include "serialize.hh"

// check that there is one encoding and one graph entry for each first clas
// citizen; and check that the parent ID stack is empty
static bool
simpleSerializationCheck(const SerializationData &sdata,
                         uint32_t first_class_count)
{
    if (first_class_count != sdata.graph.size()
        || first_class_count != sdata.encodings.size()
        || sdata.parent_id.ids.size() != 0) {
        return false;
    }

    return true;
}

#define SERIALIZATION_CHECK(sdata, first_class_count)                       \
{                                                                           \
    if (false == simpleSerializationCheck((sdata), (first_class_count))) {  \
        return false;                                                       \
    }                                                                       \
}

namespace TestMember {
    struct Foo;
    struct Bar;
    struct Cork;
}

typedef TypeList<MemberPair<TestMember::Foo,  int>,
                 MemberPair<TestMember::Bar,  int>,
                 MemberPair<TestMember::Cork, std::string> >::type
    TestTypes;

class Test;
typedef Serializable<Test, TestTypes> TestBase;
class Test : public TestBase {
public:
    Test() {}
};

static bool
testAtKey()
{
    Test t;

    // ------------------------
    //  manually set the value
    // ------------------------
    static_cast<TestBase &>(t).member_pair.value = 45;

    if (45 != atKey<TestMember::Foo>(t)) {
        return false;
    }

    // -------------------------------------------
    //  set the value through the atKey interface
    // -------------------------------------------
    atKey<TestMember::Foo>(t) = 33;
    if (33 != atKey<TestMember::Foo>(t)) {
        return false;
    }

    auto &temp_foo = static_cast<TestBase &>(t);
    if (33 != temp_foo.member_pair.value) {
        return false;
    }

    // -----------------------------
    //  manually set another value
    // -----------------------------
    static_cast<Serializable<Test,
                             TypeList<MemberPair<TestMember::Cork,
                                                 std::string> >::type > &>
        (t).member_pair.value = "howdy";

    if ("howdy" != atKey<TestMember::Cork>(t)) {
        return false;
    }

    // make sure our previous change still exists
    if (33 != atKey<TestMember::Foo>(t) || 33 != temp_foo.member_pair.value) {
        return false;
    }

    // ---------------------------------------
    //  set another value through atKey iface
    // ---------------------------------------
    atKey<TestMember::Cork>(t) = "folks";
    if ("folks" != atKey<TestMember::Cork>(t)) {
        return false;
    }

    // make sure our previous change still exists
    if (33 != atKey<TestMember::Foo>(t) || 33 != temp_foo.member_pair.value) {
        return false;
    }

    return true;
}

static bool
testSerialize()
{
    Test t;

    atKey<TestMember::Foo>(t) = 105;
    atKey<TestMember::Bar>(t) = 22;
    const std::string &spork = "spork";
    atKey<TestMember::Cork>(t) = spork;

    const std::string serial_foo = "3_105";

    // -------------------------------
    //  check serialization primitive
    // -------------------------------
    {
        SerializationData dummy_sdata;
        if (serial_foo != serializeType(105, dummy_sdata)) {
            return false;
        }

        if ("2_22" != serializeType(22, dummy_sdata)) {
            return false;
        }

        if ("5_spork" != serializeType(spork, dummy_sdata)) {
            return false;
        }
    }

    // -------------------------
    //  serialize entire object
    // -------------------------
    const SerializationData &sdata = t.serialize();
    SERIALIZATION_CHECK(sdata, 1)

    const auto &graph_pair = sdata.graph.begin();
    // std::cout << graph_pair->first << "\t" << t.id << std::endl;
    if (graph_pair->first != t.getID() || (false == graph_pair->second.empty())) {
        return false;
    }

    {
        const auto &foo = serialEncode(std::to_string(atKey<TestMember::Foo>(t)));
        const auto &bar = serialEncode(std::to_string(atKey<TestMember::Bar>(t)));
        const auto &cork = serialEncode(atKey<TestMember::Cork>(t));

        const std::string serial_t = foo + bar + cork;
        if (sdata.encodings.find(t.getID()) == sdata.encodings.end()
            || serial_t != sdata.encodings.find(t.getID())->second) {
            return false;
        }
    }

    return true;
}

class TestPointer
    : public Serializable<TestPointer,
                          TypeList<MemberPair<TestMember::Bar,
                                              std::unique_ptr<int> > >::type> {
};

static bool
testSerializePointer()
{
    TestPointer t;

    int *const n = new int(12);
    atKey<TestMember::Bar>(t) = std::unique_ptr<int>(n);

    const SerializationData &sdata = t.serialize();
    SERIALIZATION_CHECK(sdata, 1);

    const auto &graph_pair = sdata.graph.begin();
    if (graph_pair->first != t.getID() || (false == graph_pair->second.empty())) {
        return false;
    }

    const std::string &serial_t = serialEncode(std::to_string(*n));
    if (sdata.encodings.find(t.getID()) == sdata.encodings.end()
        || serial_t != sdata.encodings.find(t.getID())->second) {
        return false;
    }

    return true;
}

class TestNull;
typedef Serializable<TestNull,
                        TypeList<MemberPair<TestMember::Foo,
                                            std::unique_ptr<int> > >::type>
    TestNullBase;

class TestNull : public TestNullBase {
public:
    TestNull() {}
    TestNull(TestNullBase &&base)
        : TestNullBase(std::move(base)) {}
};

static bool
testSecondClassNull()
{
    TestNull t;

    atKey<TestMember::Foo>(t) = std::unique_ptr<int>(nullptr);

    // -------------------------
    //        serialize
    // -------------------------
    auto sdata = t.serialize();
    const auto &graph_pair = sdata.graph.begin();
    if (graph_pair->first != t.getID() || (false == graph_pair->second.empty())) {
        return false;
    }

    if (sdata.encodings.find(t.getID()) == sdata.encodings.end()
        || serializeNull() != sdata.encodings.find(t.getID())->second) {
        return false;
    }

    // -------------------------
    //       deserialize
    // -------------------------
    TestNull *const new_t = TestNull::deserialize(t.getID(), sdata.encodings);
    if (new_t->getID() != t.getID()) {
        return false;
    }
    if (atKey<TestMember::Foo>(*new_t) != nullptr) {
        std::cout << atKey<TestMember::Foo>(*new_t).get() << std::endl;
        return false;
    }

    return true;
}

class NestedSerializable :
    public Serializable<NestedSerializable,
                        TypeList<MemberPair<TestMember::Foo,
                                            std::unique_ptr<TestNull> > >::type> {};

static bool
testNestedSerializable()
{
    NestedSerializable t;

    TestNull *const nested = new TestNull;
    atKey<TestMember::Foo>(t) = std::unique_ptr<TestNull>(nested);

    auto sdata = t.serialize();

    // -----------------------
    //  examine the encodings
    // -----------------------
    if (sdata.encodings.size() != 2) {
        return false;
    }

    auto t_encoding = sdata.encodings.find(t.getID());
    if (sdata.encodings.end() == t_encoding) {
        return false;
    }
    if (serialEncode(nested->getID().to_string()) != t_encoding->second) {
        return false;
    }

    auto nested_encoding = sdata.encodings.find(nested->getID());
    if (sdata.encodings.end() == nested_encoding) {
        return false;
    }
    if (serializeNull() != nested_encoding->second) {
        return false;
    }

    // -------------------
    //  examine the graph
    // -------------------
    if (sdata.graph.size() != 2) {
        return false;
    }

    auto t_graph = sdata.graph.find(t.getID());
    if (sdata.graph.end() == t_graph) {
        return false;
    }
    // 't' should have one child, 'nested'
    if (t_graph->second.size() != 1
        || t_graph->second.front() != nested->getID()) {
        return false;
    }

    auto nested_graph = sdata.graph.find(nested->getID());
    if (sdata.graph.end() == nested_graph) {
        return false;
    }
    // 'nested' should have no children
    if (nested_graph->second.size() != 0) {
        return false;
    }

    return true;
}

class TestDes;

typedef Serializable<TestDes,
                     TypeList<MemberPair<TestMember::Foo, int>,
                              MemberPair<TestMember::Bar, int>,
                              MemberPair<TestMember::Cork, std::string> >::type>
    TestDesBase;

class TestDes : public TestDesBase {
public:
    TestDes(TestDesBase &&base)
        : TestDesBase(std::move(base)) {}
};

static bool
testDeserialize()
{
    const int foo = 100;
    const int bar = 200;
    const std::string &cork = "three hundred";
    SerializationData sdata;
    const std::string &serial =
        serializeType(foo, sdata) + serializeType(bar, sdata)
        + serializeType(cork, sdata);

    std::map<ID, std::string> encodings;
    ID id;
    encodings.insert(std::make_pair(id, serial));

    TestDes *const td = TestDes::deserialize(id, encodings);
    if (id != td->getID()) {
        return false;
    }

    if (foo != atKey<TestMember::Foo>(*td)) {
        return false;
    }
    if (bar != atKey<TestMember::Bar>(*td)) {
        return false;
    }
    if (cork != atKey<TestMember::Cork>(*td)) {
        return false;
    }

    return true;
}

class TestRoundTrip;

typedef Serializable<TestRoundTrip,
                     TypeList<MemberPair<TestMember::Foo, int>,
                              MemberPair<TestMember::Bar, std::string>,
                              MemberPair<TestMember::Cork,
                                         std::unique_ptr<std::string> > >::type>
    TestRoundTripBase;

class TestRoundTrip : public TestRoundTripBase {
public:
    TestRoundTrip() {}
    TestRoundTrip(TestRoundTripBase &&base)
        : TestRoundTripBase(std::move(base)) {}
};

static bool
testRoundTrip()
{
    TestRoundTrip round;
    int foo; std::string bar; std::string cork;
    atKey<TestMember::Foo>(round) = foo = 205;
    atKey<TestMember::Bar>(round) = bar = "catdog";
    cork = "flying castles";
    atKey<TestMember::Cork>(round) =
        std::unique_ptr<std::string>(new std::string(cork));

    // serialize
    const SerializationData &sdata = round.serialize();

    // deserialize
    TestRoundTrip *const new_round =
        TestRoundTrip::deserialize(round.getID(), sdata.encodings);
    if (new_round->getID() != round.getID()) {
        return false;
    }
    if (atKey<TestMember::Foo>(*new_round) != atKey<TestMember::Foo>(round)
        || atKey<TestMember::Foo>(*new_round) != foo) {
        return false;
    }
    if (atKey<TestMember::Bar>(*new_round) != atKey<TestMember::Bar>(round)
        || atKey<TestMember::Bar>(*new_round) != bar) {
        return false;
    }
    if (*atKey<TestMember::Cork>(*new_round) != *atKey<TestMember::Cork>(round)
        || *atKey<TestMember::Cork>(*new_round) != cork) {
        return false;
    }

    return true;
}

class TestTrivial;
typedef Serializable<TestTrivial,
                     TypeList<MemberPair<TestMember::Foo,
                                         std::vector<int> > >::type>
    TestTrivialBase;

class TestTrivial : public TestTrivialBase {
public:
    TestTrivial() {}
    TestTrivial(TestTrivialBase &&base)
        : TestTrivialBase(std::move(base)) {}
};

static bool
testVectorTrivial()
{
    TestTrivial trivial;

    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(5);
    atKey<TestMember::Foo>(trivial) = vec;

    // -------------------------
    //        serialize
    // -------------------------
    const SerializationData &sdata = trivial.serialize();
    SERIALIZATION_CHECK(sdata, 1);

    const std::string &control_serial =
        serialEncode(serialEncode(std::to_string(1))
                     + serialEncode(std::to_string(5)));
    if (sdata.encodings.find(trivial.getID()) == sdata.encodings.end()
        || sdata.encodings.find(trivial.getID())->second != control_serial) {
        return false;
    }

    // -------------------------
    //       deserialize
    // -------------------------
    TestTrivial *const new_trivial =
        TestTrivial::deserialize(trivial.getID(), sdata.encodings);
    if (new_trivial->getID() != trivial.getID()) {
        return false;
    }
    if (atKey<TestMember::Foo>(*new_trivial) != vec) {
        return false;
    }
    return true;
}

class Simple;
typedef Serializable<Simple,
                     TypeList<MemberPair<TestMember::Foo, std::string>,
                              MemberPair<TestMember::Bar, unsigned long> >::type >
    SimpleBase;
class Simple : public SimpleBase {
public:
    Simple() {}
    Simple(const Simple &rhs)
        : SimpleBase(static_cast<const SimpleBase &>(rhs)) {}
    Simple(SimpleBase &&base)
        : SimpleBase(std::move(base)) {}

    // FIXME: make pretty
    bool operator==(const Simple &rhs) const
    {
        return this->getID() == rhs.getID()
        && this->member_pair.value == rhs.member_pair.value
        && static_cast<const std::decay<decltype(*this)>::type::next *>(this)->member_pair.value
           == static_cast<const std::decay<decltype(rhs)>::type::next &>(rhs).member_pair.value;
    }
};

class TestVector;
typedef Serializable<TestVector,
                     TypeList<MemberPair<TestMember::Foo,
                                         std::vector<Simple> > >::type>
    TestVectorBase;

class TestVector : public TestVectorBase {
public:
    TestVector() {}
    TestVector(TestVectorBase &&base)
        : TestVectorBase(std::move(base)) {}
};

static bool
testVector()
{
    Simple s1;
    atKey<TestMember::Foo>(s1) = "dead";
    atKey<TestMember::Bar>(s1) = 0xBEEF;

    Simple s2;
    atKey<TestMember::Foo>(s2) = "homerun";
    atKey<TestMember::Bar>(s2) = 0x1337;

    std::vector<Simple> v;
    v.push_back(s1);
    v.push_back(s2);

    TestVector tvec;
    atKey<TestMember::Foo>(tvec) = std::vector<Simple>(v);

    const SerializationData &sdata = tvec.serialize();
    SERIALIZATION_CHECK(sdata, 3);

    const std::string control_serial =
        serialEncode(serialEncode(s1.getID().to_string())
                     + serialEncode(s2.getID().to_string()));
    if (sdata.encodings.find(tvec.getID()) == sdata.encodings.end()
        || sdata.encodings.find(tvec.getID())->second != control_serial) {
        return false;
    }

    // -------------------------
    //     now deserialize
    // -------------------------
    TestVector *const new_tvec =
        TestVector::deserialize(tvec.getID(), sdata.encodings);
    if (new_tvec->getID() != tvec.getID()) {
        return false;
    }
    if (atKey<TestMember::Foo>(*new_tvec) != v) {
        return false;
    }

    return true;
}

class TestMap;
typedef Serializable<TestMap,
                     TypeList<MemberPair<TestMember::Foo,
                                    std::map<std::string, uint16_t> > >::type>
    TestMapBase;

class TestMap : public TestMapBase {
public:
    TestMap() {}
    TestMap(TestMapBase &&base)
        : TestMapBase(std::move(base)) {}
};

static bool
testMapTrivial()
{
    TestMap trivial_map;

    std::map<std::string, uint16_t> m{{"cow", 10}, {"horse", 25}};
    atKey<TestMember::Foo>(trivial_map) = m;

    // -------------------------
    //        serialize
    // -------------------------
    const SerializationData &sdata = trivial_map.serialize();
    SERIALIZATION_CHECK(sdata, 1);

    // -------------------------
    //       deserialize
    // -------------------------
    TestMap *const new_trivial =
        TestMap::deserialize(trivial_map.getID(), sdata.encodings);
    if (new_trivial->getID() != trivial_map.getID()) {
        return false;
    }
    if (atKey<TestMember::Foo>(*new_trivial) != m) {
        return false;
    }

    return true;
}

class TestFirstTime;
typedef Serializable<TestFirstTime,
                     TypeList<MemberPair<TestMember::Foo, std::string>,
                              MemberPair<TestMember::Bar, std::string>,
                              MemberPair<TestMember::Cork, uint32_t> >::type>
    TestFirstTimeBase;

class TestFirstTime : public TestFirstTimeBase {
public:
    TestFirstTime(const std::string &foo, uint32_t cork)
        : TestFirstTimeBase(Serializable::expandAndTuple<TypeList<TestMember::Foo,
                                       TestMember::Cork>::type>(foo, cork)) {}
};

static bool
testFirstTimeConstructor()
{
    const std::string foo = "somestring";
    const uint32_t cork = 156;
    TestFirstTime first(foo, cork);

    if (foo != atKey<TestMember::Foo>(first)) {
        return false;
    }

    if (cork != atKey<TestMember::Cork>(first)) {
        return false;
    }

    return true;
}

class TestVector1stPtrz;
typedef Serializable<TestVector1stPtrz,
                     TypeList<MemberPair<TestMember::Foo,
                                std::vector<std::unique_ptr<Simple> > > >::type>
    TestVector1stPtrzBase;
class TestVector1stPtrz : public TestVector1stPtrzBase {
public:
    TestVector1stPtrz() {}
    TestVector1stPtrz(TestVector1stPtrzBase &&base)
        : TestVector1stPtrzBase(std::move(base)) {}
};

static bool
testVectorFirstClassPointers()
{
    Simple s1;
    atKey<TestMember::Foo>(s1) = "dead";
    atKey<TestMember::Bar>(s1) = 0xBEEF;

    Simple s2;
    atKey<TestMember::Foo>(s2) = "homerun";
    atKey<TestMember::Bar>(s2) = 0x1337;

    std::vector<std::unique_ptr<Simple> > v;
    v.push_back(std::unique_ptr<Simple>(new Simple(s1)));
    v.push_back(std::unique_ptr<Simple>(new Simple(s2)));

    TestVector1stPtrz tvec;
    atKey<TestMember::Foo>(tvec) = std::move(v);

    const SerializationData &sdata = tvec.serialize();
    SERIALIZATION_CHECK(sdata, 3);

    TestVector1stPtrz *const new_tvec =
        TestVector1stPtrz::deserialize(tvec.getID(), sdata.encodings);
    if (new_tvec->getID() != tvec.getID()) {
        return false;
    }

    const auto &new_v = atKey<TestMember::Foo>(*new_tvec);
    if (2 != new_v.size() || nullptr == new_v[0] || nullptr == new_v[1]
        || !(*new_v[0] == s1) || !(*new_v[1] == s2)) {
        return false;
    }

    return true;
}

static void
runTests()
{
    std::cout << "running tests!" << std::endl;
    TEST(testAtKey);
    TEST(testSerialize);
    TEST(testSerializePointer);
    TEST(testSecondClassNull);
    TEST(testNestedSerializable);
    TEST(testDeserialize);
    TEST(testRoundTrip);
    TEST(testVectorTrivial);
    TEST(testVector);
    TEST(testMapTrivial);
    TEST(testFirstTimeConstructor);
    TEST(testVectorFirstClassPointers);
}

