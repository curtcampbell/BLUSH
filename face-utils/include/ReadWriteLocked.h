#pragma once

// ReadWriteLocked.h
//
// A value paired with its own reader-writer mutex, providing safe concurrent
// access through typed read/write callables.
//
// Multiple threads may call read() simultaneously (shared lock).
// A write() call acquires an exclusive lock, blocking all concurrent readers
// and writers on this instance until the callable returns.
// Operations on different ReadWriteLocked instances are fully independent.
//
// C++ standard note
// -----------------
// This implementation uses std::shared_timed_mutex (C++14) and
// std::shared_lock (C++14).  std::shared_timed_mutex supports timed
// lock variants (try_lock_for, try_lock_until) which add implementation
// overhead not needed here but required by the C++14 standard.
//
// Upgrading to C++17 allows substituting std::shared_mutex, which:
//   - Omits the timed-lock machinery, reducing internal state.
//   - May be implemented as a leaner primitive on platforms (e.g. POSIX
//     pthread_rwlock_t) where the timed variant requires a heavier path.
//   - On MSVC the difference is modest; on Linux/GCC the gain is measurable
//     under high read contention.
// To upgrade: replace shared_timed_mutex with shared_mutex in this file only.
//
// Usage
// -----
//   ReadWriteLocked<MyRecord> record;
//
//   // Shared read — multiple threads enter simultaneously:
//   auto snapshot = record.read([](const MyRecord& r) { return r.value; });
//
//   // Exclusive write — serialised against all readers and writers:
//   record.write([](MyRecord& r) { r.value = newValue; });

#include <mutex>
#include <shared_mutex>
#include <utility>      // std::forward, std::declval

template<typename T>
class ReadWriteLocked
{
public:
    // -----------------------------------------------------------------------
    // Construction
    // -----------------------------------------------------------------------

    /// Default-constructs the contained value.
    ReadWriteLocked() = default;

    /// Constructs the contained value by forwarding all arguments to T's
    /// constructor.  Example:
    ///   ReadWriteLocked<std::vector<int>> v(10, 0); // vector of 10 zeros
    template<typename... Args>
    explicit ReadWriteLocked(Args&&... args)
        : m_value(std::forward<Args>(args)...)
    {}

    // Non-copyable, non-movable (mutex is not movable)
    ReadWriteLocked(const ReadWriteLocked&)            = delete;
    ReadWriteLocked& operator=(const ReadWriteLocked&) = delete;

    // -----------------------------------------------------------------------
    // Read access  (shared lock — concurrent readers permitted)
    // -----------------------------------------------------------------------

    /// Acquire a shared (read) lock and call fn(const T&).
    /// Returns whatever fn returns (including void).
    /// Multiple threads may hold a shared lock simultaneously.
    ///
    /// @param fn  Callable with signature: ReturnType(const T&)
    template<typename F>
    auto read(F&& fn) const
        -> decltype(fn(std::declval<const T&>()))
    {
        std::shared_lock<std::shared_timed_mutex> lock(m_mutex);
        return fn(m_value);
    }

    // -----------------------------------------------------------------------
    // Write access  (exclusive lock — no concurrent readers or writers)
    // -----------------------------------------------------------------------

    /// Acquire an exclusive (write) lock and call fn(T&).
    /// Returns whatever fn returns (including void).
    /// Blocks until all current shared and exclusive holders release.
    ///
    /// @param fn  Callable with signature: ReturnType(T&)
    template<typename F>
    auto write(F&& fn)
        -> decltype(fn(std::declval<T&>()))
    {
        std::unique_lock<std::shared_timed_mutex> lock(m_mutex);
        return fn(m_value);
    }

private:
    mutable std::shared_timed_mutex m_mutex;
    T                               m_value;
};
