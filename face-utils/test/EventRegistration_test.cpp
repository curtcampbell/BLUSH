#include "EventRegistration.h"

#include <gtest/gtest.h>
#include <type_traits>

namespace {

class FakeRegistration : public EventRegistration {
public:
    void Cancel() override { ++m_cancelCount; }

    int m_cancelCount = 0;
};

} // namespace

TEST(EventRegistrationTest, IsNotCopyable) {
    static_assert(!std::is_copy_constructible<EventRegistration>::value,
                  "EventRegistration must not be copy-constructible");
    static_assert(!std::is_copy_assignable<EventRegistration>::value,
                  "EventRegistration must not be copy-assignable");
}

TEST(EventRegistrationTest, DispatchesThroughBasePointer) {
    FakeRegistration impl;
    EventRegistration& base = impl;

    base.Cancel();
    base.Cancel();

    EXPECT_EQ(impl.m_cancelCount, 2);
}
