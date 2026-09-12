#pragma once

// IDGenerator.h
//
// Lock-free, thread-safe generator of monotonically increasing integer IDs.
//
// Each call to Next() atomically increments an internal counter and returns
// the previous value.  The counter starts at `initial` (default 1) so that
// zero is never returned and can be used as a sentinel "no ID" value by
// callers.
//
// Lock-freedom
// ------------
// The implementation relies on std::atomic<T>::fetch_add with
// memory_order_relaxed.  For all integral types on x86-64 (Windows and
// Linux), std::atomic<T> is lock-free when sizeof(T) <= sizeof(void*).
// A runtime assertion in the constructor verifies this at startup.
//
// memory_order_relaxed is correct here: the only guarantee required is that
// each call receives a value distinct from every other call.  No ordering
// relative to other memory operations is needed from the generator itself.
//
// Overflow behaviour
// ------------------
// Signed-integer overflow is technically undefined behaviour in C++14.  At
// 64-bit width and practical entity creation rates this is not a real concern
// — at 1 billion IDs per second the counter would take ~292 years to wrap.
// If overflow must be handled, switch T to uint64_t (unsigned wrap is
// well-defined in C++) and treat 0 as an additional skipped value in Next().
//
// C++ standard note
// -----------------
// Requires C++14.  In C++17 the is_always_lock_free static member can replace
// the runtime assertion with a compile-time check:
//   static_assert(std::atomic<T>::is_always_lock_free, "...");
//
// Usage
// -----
//   IDGenerator<int64_t> gen;          // starts at 1
//   IDGenerator<int64_t> gen(100);     // starts at 100
//
//   int64_t id = gen.Next();           // 1, 2, 3, ... thread-safe, lock-free

#include <atomic>
#include <cassert>
#include <type_traits>

template<typename T>
class IDGenerator
{
    static_assert(std::is_integral<T>::value,
                  "IDGenerator requires an integral type");

public:
    /// Construct a generator whose first Next() call returns @p initial.
    /// @p initial must not be zero (zero is reserved as the sentinel).
    explicit IDGenerator(T initial = static_cast<T>(1))
        : m_next(initial)
    {
        assert(initial != static_cast<T>(0) &&
               "IDGenerator: initial value 0 is reserved as a sentinel");

        // Runtime lock-freedom check (compile-time requires C++17).
        assert(m_next.is_lock_free() &&
               "IDGenerator: std::atomic<T> is not lock-free on this platform");
    }

    // Non-copyable, non-movable (std::atomic is not copyable)
    IDGenerator(const IDGenerator&)            = delete;
    IDGenerator& operator=(const IDGenerator&) = delete;

    /// Return the next unique ID.  Never returns zero.
    /// Safe to call concurrently from any number of threads.
    T Next()
    {
        return m_next.fetch_add(static_cast<T>(1), std::memory_order_relaxed);
    }

private:
    std::atomic<T> m_next;
};
