#include "Cancellable.h"

#include <gtest/gtest.h>
#include <type_traits>

namespace {

class FakeCancellable : public Cancellable {
public:
    ScheduleStatus Status() const override { return m_status; }

    void Cancel() override {
        ++m_cancelCount;
        m_status = ScheduleStatus::Cancelled;
    }

    ScheduleStatus m_status = ScheduleStatus::Scheduled;
    int            m_cancelCount = 0;
};

} // namespace

TEST(CancellableTest, IsNotCopyable) {
    static_assert(!std::is_copy_constructible<Cancellable>::value,
                  "Cancellable must not be copy-constructible");
    static_assert(!std::is_copy_assignable<Cancellable>::value,
                  "Cancellable must not be copy-assignable");
}

TEST(CancellableTest, ScheduleStatusValuesAreDistinct) {
    EXPECT_NE(ScheduleStatus::Scheduled, ScheduleStatus::Running);
    EXPECT_NE(ScheduleStatus::Running, ScheduleStatus::Completed);
    EXPECT_NE(ScheduleStatus::Completed, ScheduleStatus::Cancelled);
    EXPECT_NE(ScheduleStatus::Scheduled, ScheduleStatus::Cancelled);
}

TEST(CancellableTest, DispatchesThroughBasePointer) {
    FakeCancellable impl;
    Cancellable&    base = impl;

    EXPECT_EQ(base.Status(), ScheduleStatus::Scheduled);

    base.Cancel();

    EXPECT_EQ(base.Status(), ScheduleStatus::Cancelled);
    EXPECT_EQ(impl.m_cancelCount, 1);
}
