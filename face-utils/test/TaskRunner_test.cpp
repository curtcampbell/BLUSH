#include "TaskRunner.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <vector>

namespace {

// Polls `pred` until it's true or `timeout` elapses. Avoids fixed sleeps
// that would either flake under load or slow the suite down needlessly.
template<typename Pred>
bool WaitUntil(Pred pred, std::chrono::milliseconds timeout = std::chrono::seconds(2)) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (pred()) { return true; }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return pred();
}

} // namespace

TEST(TaskRunnerTest, ThrowsOnZeroThreads) {
    EXPECT_THROW(TaskRunner(0), std::invalid_argument);
}

TEST(TaskRunnerTest, ExecutesSingleTask) {
    TaskRunner       runner(1);
    std::atomic<bool> ran{false};

    runner.AddTask([&] { ran = true; });

    EXPECT_TRUE(WaitUntil([&] { return ran.load(); }));
}

TEST(TaskRunnerTest, ExecutesAllQueuedTasks) {
    TaskRunner       runner(4);
    constexpr int    kTaskCount = 200;
    std::atomic<int> completed{0};

    for (int i = 0; i < kTaskCount; ++i) {
        runner.AddTask([&] { ++completed; });
    }

    EXPECT_TRUE(WaitUntil([&] { return completed.load() == kTaskCount; }));
}

TEST(TaskRunnerTest, SingleThreadPreservesSubmissionOrder) {
    TaskRunner        runner(1);
    std::mutex        mutex;
    std::vector<int>  order;
    constexpr int     kTaskCount = 100;

    for (int i = 0; i < kTaskCount; ++i) {
        runner.AddTask([&, i] {
            std::lock_guard<std::mutex> lock(mutex);
            order.push_back(i);
        });
    }

    ASSERT_TRUE(WaitUntil([&] {
        std::lock_guard<std::mutex> lock(mutex);
        return order.size() == static_cast<size_t>(kTaskCount);
    }));

    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < kTaskCount; ++i) {
        EXPECT_EQ(order[i], i);
    }
}

namespace {

class Counter {
public:
    void Increment() { ++m_value; }
    void IncrementConst() const { ++m_value; }

    int Value() const { return m_value; }

private:
    mutable std::atomic<int> m_value{0};
};

} // namespace

TEST(TaskRunnerTest, AddTaskSupportsMemberFunctionPointer) {
    TaskRunner runner(1);
    Counter    counter;

    runner.AddTask(&Counter::Increment, &counter);

    EXPECT_TRUE(WaitUntil([&] { return counter.Value() == 1; }));
}

TEST(TaskRunnerTest, AddTaskSupportsConstMemberFunctionPointer) {
    TaskRunner  runner(1);
    const Counter counter;

    runner.AddTask(&Counter::IncrementConst, &counter);

    EXPECT_TRUE(WaitUntil([&] { return counter.Value() == 1; }));
}

TEST(TaskRunnerTest, DestructorDrainsTasksAlreadyQueued) {
    std::atomic<int> completed{0};
    constexpr int    kTaskCount = 50;

    {
        TaskRunner runner(2);
        for (int i = 0; i < kTaskCount; ++i) {
            runner.AddTask([&] { ++completed; });
        }
    } // destructor joins worker threads, which drain the remaining queue

    EXPECT_EQ(completed.load(), kTaskCount);
}
