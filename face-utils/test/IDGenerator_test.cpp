#include "IDGenerator.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <mutex>
#include <set>
#include <thread>
#include <type_traits>
#include <vector>

TEST(IDGeneratorTest, IsNotCopyable) {
    static_assert(!std::is_copy_constructible<IDGenerator<int64_t>>::value,
                  "IDGenerator must not be copy-constructible");
    static_assert(!std::is_copy_assignable<IDGenerator<int64_t>>::value,
                  "IDGenerator must not be copy-assignable");
}

TEST(IDGeneratorTest, DefaultStartsAtOneAndIncrements) {
    IDGenerator<int64_t> gen;

    EXPECT_EQ(gen.Next(), 1);
    EXPECT_EQ(gen.Next(), 2);
    EXPECT_EQ(gen.Next(), 3);
}

TEST(IDGeneratorTest, CustomInitialValue) {
    IDGenerator<int64_t> gen(100);

    EXPECT_EQ(gen.Next(), 100);
    EXPECT_EQ(gen.Next(), 101);
}

TEST(IDGeneratorTest, DefaultConstructedGeneratorNeverReturnsZero) {
    IDGenerator<int64_t> gen;

    EXPECT_NE(gen.Next(), 0);
    EXPECT_NE(gen.Next(), 0);
    EXPECT_NE(gen.Next(), 0);
}

TEST(IDGeneratorTest, WorksWithDifferentIntegralTypes) {
    IDGenerator<int32_t>  gen32;
    IDGenerator<uint64_t> gen64;

    EXPECT_EQ(gen32.Next(), 1);
    EXPECT_EQ(gen64.Next(), 1u);
}

#ifndef NDEBUG
TEST(IDGeneratorDeathTest, ZeroInitialValueAsserts) {
    EXPECT_DEATH({ IDGenerator<int64_t> gen(0); }, "");
}
#endif

TEST(IDGeneratorTest, ConcurrentCallsProduceUniqueIds) {
    constexpr int kThreads         = 8;
    constexpr int kCallsPerThread  = 2000;

    IDGenerator<int64_t> gen;
    std::mutex            resultsMutex;
    std::vector<int64_t>  results;
    results.reserve(kThreads * kCallsPerThread);

    std::vector<std::thread> threads;
    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&] {
            std::vector<int64_t> local;
            local.reserve(kCallsPerThread);
            for (int i = 0; i < kCallsPerThread; ++i) {
                local.push_back(gen.Next());
            }
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.insert(results.end(), local.begin(), local.end());
        });
    }
    for (auto& t : threads) { t.join(); }

    std::set<int64_t> unique(results.begin(), results.end());
    EXPECT_EQ(unique.size(), results.size());
    EXPECT_EQ(results.size(), static_cast<size_t>(kThreads) * kCallsPerThread);
}
