#include "UopBase.h"

#include <gtest/gtest.h>

namespace {

// Inject() is protected; expose it for the test.
class TestUopBase : public UopBase {
public:
    using UopBase::Inject;
};

// Each test uses its own tag type as the DataType key so the static
// per-DataType injection map in UopBase can't leak state between tests.
struct TagA { int value; };
struct TagB { int value; };
struct TagIsolation { int value; };
struct TagSetReference { FACE::TSS::Base* value; };

} // namespace

TEST(UopBaseTest, InjectedExistsFalseBeforeInject) {
    TestUopBase uop;
    EXPECT_FALSE(uop.InjectedExists<TagA*>(FACE::STRING_TYPE("name")));
}

TEST(UopBaseTest, InjectedExistsTrueAfterInject) {
    TestUopBase uop;
    TagA        value{1};

    uop.Inject<TagA*>(FACE::STRING_TYPE("name"), &value);

    EXPECT_TRUE(uop.InjectedExists<TagA*>(FACE::STRING_TYPE("name")));
}

TEST(UopBaseTest, GetInjectedReturnsStoredValue) {
    TestUopBase uop;
    TagB        value{42};

    uop.Inject<TagB*>(FACE::STRING_TYPE("conn"), &value);

    EXPECT_EQ(uop.GetInjected<TagB*>(FACE::STRING_TYPE("conn")), &value);
}

TEST(UopBaseTest, GetInjectedThrowsWhenTypeNeverInjected) {
    TestUopBase uop;
    EXPECT_THROW(uop.GetInjected<TagA*>(FACE::STRING_TYPE("missing")), std::runtime_error);
}

TEST(UopBaseTest, GetInjectedThrowsWhenNameNotFound) {
    TestUopBase uop;
    TagB        value{1};
    uop.Inject<TagB*>(FACE::STRING_TYPE("known"), &value);

    EXPECT_THROW(uop.GetInjected<TagB*>(FACE::STRING_TYPE("unknown")), std::runtime_error);
}

TEST(UopBaseTest, SupportsMultipleNamedInjectionsOfSameType) {
    TestUopBase uop;
    TagA        first{1};
    TagA        second{2};

    uop.Inject<TagA*>(FACE::STRING_TYPE("first"), &first);
    uop.Inject<TagA*>(FACE::STRING_TYPE("second"), &second);

    EXPECT_EQ(uop.GetInjected<TagA*>(FACE::STRING_TYPE("first")), &first);
    EXPECT_EQ(uop.GetInjected<TagA*>(FACE::STRING_TYPE("second")), &second);
}

TEST(UopBaseTest, DistinctLiveInstancesAreIsolated) {
    TestUopBase uopOne;
    TestUopBase uopTwo;
    TagIsolation valueOne{1};
    TagIsolation valueTwo{2};

    uopOne.Inject<TagIsolation*>(FACE::STRING_TYPE("name"), &valueOne);
    uopTwo.Inject<TagIsolation*>(FACE::STRING_TYPE("name"), &valueTwo);

    EXPECT_EQ(uopOne.GetInjected<TagIsolation*>(FACE::STRING_TYPE("name")), &valueOne);
    EXPECT_EQ(uopTwo.GetInjected<TagIsolation*>(FACE::STRING_TYPE("name")), &valueTwo);
}

TEST(UopBaseTest, SetReferenceStoresValueRetrievableViaGetInjected) {
    class ConcreteUop : public UopBase {};

    ConcreteUop            uop;
    FACE::TSS::Base*       fakeBase = reinterpret_cast<FACE::TSS::Base*>(0x1);
    FACE::RETURN_CODE_TYPE rc;

    uop.Set_Reference(FACE::STRING_TYPE("base"), fakeBase, /*id=*/0, rc);

    EXPECT_EQ(uop.GetInjected<FACE::TSS::Base*>(FACE::STRING_TYPE("base")), fakeBase);
    // UopBase::Set_Reference always reports NOT_AVAILABLE; typed subclasses
    // (see FaceInjectable) report NO_ERROR after a successful injection.
    EXPECT_EQ(rc, FACE::RETURN_CODE_TYPE::NOT_AVAILABLE);
}
