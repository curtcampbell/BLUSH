#include "TaskScheduler.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

namespace {

template<typename Pred>
bool WaitUntil(Pred pred, std::chrono::milliseconds timeout = std::chrono::seconds(3)) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (pred()) { return true; }
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return pred();
}

} // namespace

TEST(TaskSchedulerTest, OneShotTaskFiresAfterDelay) {
    TaskScheduler     scheduler;
    std::atomic<bool> ran{false};

    auto handle = scheduler.ScheduleTask(std::chrono::milliseconds(20), [&] { ran = true; });

    EXPECT_TRUE(WaitUntil([&] { return ran.load(); }));
    EXPECT_TRUE(WaitUntil([&] { return handle->Status() == ScheduleStatus::Completed; }));
}

TEST(TaskSchedulerTest, OneShotTaskAtAbsoluteTimePointFires) {
    TaskScheduler     scheduler;
    std::atomic<bool> ran{false};

    auto when = TaskScheduler::Clock::now() + std::chrono::milliseconds(20);
    scheduler.ScheduleTask(when, [&] { ran = true; });

    EXPECT_TRUE(WaitUntil([&] { return ran.load(); }));
}

TEST(TaskSchedulerTest, CancelBeforeFireRunsNothing) {
    TaskScheduler     scheduler;
    std::atomic<bool> ran{false};

    auto handle = scheduler.ScheduleTask(std::chrono::milliseconds(200), [&] { ran = true; });
    handle->Cancel();

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    EXPECT_FALSE(ran.load());
    EXPECT_EQ(handle->Status(), ScheduleStatus::Cancelled);
}

TEST(TaskSchedulerTest, RepeatingTaskFiresMultipleTimes) {
    TaskScheduler    scheduler;
    std::atomic<int> count{0};

    auto handle = scheduler.ScheduleRepeatingTask(
        std::chrono::milliseconds(15), [&] { ++count; });

    EXPECT_TRUE(WaitUntil([&] { return count.load() >= 3; }));
    handle->Cancel();
}

TEST(TaskSchedulerTest, CancelStopsFurtherRepeatingInvocations) {
    TaskScheduler    scheduler;
    std::atomic<int> count{0};

    auto handle = scheduler.ScheduleRepeatingTask(
        std::chrono::milliseconds(15), [&] { ++count; });

    ASSERT_TRUE(WaitUntil([&] { return count.load() >= 2; }));
    handle->Cancel();

    int countAtCancel = count.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Allow at most one more in-flight invocation to land after Cancel().
    EXPECT_LE(count.load(), countAtCancel + 1);
}

TEST(TaskSchedulerTest, RepeatingTaskHonoursCustomInitialDelay) {
    TaskScheduler    scheduler;
    std::atomic<int> count{0};

    auto start = std::chrono::steady_clock::now();
    auto handle = scheduler.ScheduleRepeatingTask(
        std::chrono::milliseconds(5), std::chrono::milliseconds(500), [&] { ++count; });

    // Well before the long period elapses, but after the short initial delay.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(count.load(), 1);
    (void)start;

    handle->Cancel();
}

TEST(TaskSchedulerTest, SkipPolicyDoesNotOverlapInvocations) {
    TaskScheduler    scheduler;
    std::atomic<int> concurrent{0};
    std::atomic<int> maxConcurrent{0};
    std::atomic<int> invocations{0};

    auto handle = scheduler.ScheduleRepeatingTask(
        std::chrono::milliseconds(10),
        [&] {
            int current = ++concurrent;
            int prevMax = maxConcurrent.load();
            while (current > prevMax &&
                   !maxConcurrent.compare_exchange_weak(prevMax, current)) {
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
            ++invocations;
            --concurrent;
        },
        OverlapPolicy::Skip);

    EXPECT_TRUE(WaitUntil([&] { return invocations.load() >= 2; }, std::chrono::seconds(3)));
    handle->Cancel();

    EXPECT_EQ(maxConcurrent.load(), 1);
}

TEST(TaskSchedulerTest, ConstructsWithExternallyOwnedRunner) {
    auto              runner = std::make_shared<TaskRunner>(2);
    TaskScheduler     scheduler(runner);
    std::atomic<bool> ran{false};

    scheduler.ScheduleTask(std::chrono::milliseconds(10), [&] { ran = true; });

    EXPECT_TRUE(WaitUntil([&] { return ran.load(); }));
}

TEST(TaskSchedulerTest, StatusTransitionsForOneShotTask) {
    TaskScheduler     scheduler;
    std::atomic<bool> release{false};
    std::atomic<bool> isRunning{false};

    auto handle = scheduler.ScheduleTask(std::chrono::milliseconds(10), [&] {
        isRunning = true;
        while (!release.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    EXPECT_TRUE(WaitUntil([&] { return isRunning.load(); }));
    EXPECT_EQ(handle->Status(), ScheduleStatus::Running);

    release = true;
    EXPECT_TRUE(WaitUntil([&] { return handle->Status() == ScheduleStatus::Completed; }));
}
