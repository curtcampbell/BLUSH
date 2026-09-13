#include "FaceInjectable.h"

#include <gtest/gtest.h>

namespace {

struct FakeData {
    int id = 0;
};

class FakeTypedTS {
public:
    virtual ~FakeTypedTS() = default;
};

class FakeInjectableInterface {
public:
    virtual ~FakeInjectableInterface() = default;
    virtual void Set_Reference(
        const FACE::STRING_TYPE& interface_name,
        FakeTypedTS*&            interface_reference,
        FACE::GUID_TYPE          id,
        FACE::RETURN_CODE_TYPE&  return_code) = 0;
};

struct FakeData2 {
    int id = 0;
};

class FakeTypedTS2 {
public:
    virtual ~FakeTypedTS2() = default;
};

class FakeInjectableInterface2 {
public:
    virtual ~FakeInjectableInterface2() = default;
    virtual void Set_Reference(
        const FACE::STRING_TYPE& interface_name,
        FakeTypedTS2*&           interface_reference,
        FACE::GUID_TYPE          id,
        FACE::RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace

template<>
struct Traits<FakeData> {
    using InjectableInterface = FakeInjectableInterface;
    using TypedTS             = FakeTypedTS;
};

template<>
struct Traits<FakeData2> {
    using InjectableInterface = FakeInjectableInterface2;
    using TypedTS             = FakeTypedTS2;
};

namespace {

class SingleUop : public FaceInjectable<FakeData> {};

class MultiUop : public FaceInjectable<FakeData>,
                  public FaceInjectable<FakeData2> {};

} // namespace

TEST(FaceInjectableTest, SetReferenceStoresValueAndReportsNoError) {
    SingleUop               uop;
    FakeTypedTS              instance;
    FakeTypedTS*             ptr = &instance;
    FACE::RETURN_CODE_TYPE   rc  = FACE::RETURN_CODE_TYPE::NOT_AVAILABLE;

    uop.Set_Reference(FACE::STRING_TYPE("conn"), ptr, /*id=*/0, rc);

    EXPECT_EQ(rc, FACE::RETURN_CODE_TYPE::NO_ERROR);
    EXPECT_EQ(uop.GetInjected<FakeTypedTS*>(FACE::STRING_TYPE("conn")), &instance);
}

TEST(FaceInjectableTest, MultipleMixinsShareOneUopBaseViaVirtualInheritance) {
    MultiUop                uop;
    FakeTypedTS              tsA;
    FakeTypedTS2             tsB;
    FakeTypedTS*             ptrA = &tsA;
    FakeTypedTS2*            ptrB = &tsB;
    FACE::RETURN_CODE_TYPE   rc;

    // Set_Reference is declared in both FaceInjectable<FakeData> and
    // FaceInjectable<FakeData2> (unrelated sibling bases), so unqualified
    // lookup is ambiguous regardless of argument types; a real external
    // executive would likewise hold a reference typed as the specific
    // XInjectable::Injectable interface it is injecting through.
    static_cast<FakeInjectableInterface&>(uop).Set_Reference(FACE::STRING_TYPE("a"), ptrA, 0, rc);
    static_cast<FakeInjectableInterface2&>(uop).Set_Reference(FACE::STRING_TYPE("b"), ptrB, 0, rc);

    // Both injections land in the same (virtual) UopBase subobject.
    EXPECT_EQ(uop.GetInjected<FakeTypedTS*>(FACE::STRING_TYPE("a")), &tsA);
    EXPECT_EQ(uop.GetInjected<FakeTypedTS2*>(FACE::STRING_TYPE("b")), &tsB);

    UopBase* base = &uop; // unambiguous only because UopBase is a virtual base
    EXPECT_NE(base, nullptr);
}
