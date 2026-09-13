#include "ReadWriteLocked.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <type_traits>
#include <vector>

TEST(ReadWriteLockedTest, IsNotCopyable) {
    static_assert(!std::is_copy_constructible<ReadWriteLocked<int>>::value,
                  "ReadWriteLocked must not be copy-constructible");
    static_assert(!std::is_copy_assignable<ReadWriteLocked<int>>::value,
                  "ReadWriteLocked must not be copy-assignable");
}

namespace {
// ReadWriteLocked() = default; default-initializes T, which for a raw int
// leaves an indeterminate value (not value-initialized to 0). Use a type
// with an in-class default member initializer to test this meaningfully.
struct WidgetWithDefault {
    int value = 42;
};
} // namespace

TEST(ReadWriteLockedTest, DefaultConstructsContainedValue) {
    ReadWriteLocked<WidgetWithDefault> locked;
    EXPECT_EQ(locked.read([](const WidgetWithDefault& v) { return v.value; }), 42);
}

TEST(ReadWriteLockedTest, ForwardsConstructorArguments) {
    ReadWriteLocked<std::vector<int>> locked(3, 7);

    auto snapshot = locked.read([](const std::vector<int>& v) { return v; });
    EXPECT_EQ(snapshot, (std::vector<int>{7, 7, 7}));
}

TEST(ReadWriteLockedTest, WriteMutatesValueObservedByRead) {
    ReadWriteLocked<int> locked(1);

    locked.write([](int& v) { v = 42; });

    EXPECT_EQ(locked.read([](const int& v) { return v; }), 42);
}

TEST(ReadWriteLockedTest, ReadAndWriteSupportVoidCallables) {
    ReadWriteLocked<int> locked(1);
    int                  observed = 0;

    locked.write([](int& v) { v += 1; });
    locked.read([&](const int& v) { observed = v; });

    EXPECT_EQ(observed, 2);
}

TEST(ReadWriteLockedTest, ConcurrentReadsAreAllowedSimultaneously) {
    ReadWriteLocked<int> locked(5);

    constexpr int     kReaders = 8;
    std::atomic<int>  concurrentReaders{0};
    std::atomic<int>  maxObservedConcurrency{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < kReaders; ++i) {
        threads.emplace_back([&] {
            locked.read([&](const int&) {
                int current = ++concurrentReaders;
                int prevMax = maxObservedConcurrency.load();
                while (current > prevMax &&
                       !maxObservedConcurrency.compare_exchange_weak(prevMax, current)) {
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                --concurrentReaders;
            });
        });
    }
    for (auto& t : threads) { t.join(); }

    EXPECT_GT(maxObservedConcurrency.load(), 1);
}

TEST(ReadWriteLockedTest, WriteExcludesConcurrentReadersAndWriters) {
    ReadWriteLocked<int> locked(0);

    constexpr int    kWriters = 6;
    std::atomic<int> activeWriters{0};
    std::atomic<int> maxObservedConcurrency{0};

    std::vector<std::thread> threads;
    for (int i = 0; i < kWriters; ++i) {
        threads.emplace_back([&] {
            locked.write([&](int& v) {
                int current = ++activeWriters;
                int prevMax = maxObservedConcurrency.load();
                while (current > prevMax &&
                       !maxObservedConcurrency.compare_exchange_weak(prevMax, current)) {
                }
                ++v;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                --activeWriters;
            });
        });
    }
    for (auto& t : threads) { t.join(); }

    EXPECT_EQ(maxObservedConcurrency.load(), 1);
    EXPECT_EQ(locked.read([](const int& v) { return v; }), kWriters);
}
