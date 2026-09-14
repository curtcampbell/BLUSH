// FACE/String.hpp
// Class representing a sequence of characters.
// FACE Technical Standard Edition 3.2, §K.2.3
//
// A FACE::String is defined by three characteristics:
//   length   — current number of characters (excluding NUL) in the String
//   bound    — maximum number of characters (excluding NUL) the String can
//              ever hold; fixed for the lifetime of the object.  An unbounded
//              String has bound == FACE::String::UNBOUNDED_SENTINEL.
//   capacity — number of characters (excluding NUL) for which memory is
//              currently allocated; length <= capacity <= bound is always true.
//
// Managed vs. Unmanaged
//   A "managed" String owns and manages the lifetime of its character buffer.
//   An "unmanaged" String wraps a pointer whose lifetime is managed elsewhere.
//
// Invalid state
//   When a constructor encounters a precondition violation or allocation
//   failure the object is placed in the "invalid state":
//     length() == 0, bound() == 0, capacity() == 0
//     buffer() == NULL
//     is_managed() == false, is_bounded() == false
//     is_valid() == false
//
// IDL mapping (§4.14.8.7.3):
//   IDL bounded:   string<N>  → typedef FACE::String Name;
//                               #define Name_bound_value ((FACE::UnsignedLong) N)
//   IDL unbounded: string     → typedef FACE::String Name;
//                               #define Name_bound_value FACE::String::UNBOUNDED_SENTINEL
//
// Implementation note: this file provides a concrete, fully implemented
// version of the §K.2.3 specification skeleton, following the same
// implementation strategy as FACE/Sequence.hpp (operator new via
// std::nothrow for managed storage). Unlike Sequence, String guarantees its
// buffer() is always a NUL-terminated C string, and out-of-range operator[]
// is defined (returns a reference to a '\0' sentinel) rather than
// implementation-defined, per the spec text below.

#ifndef FACE_STRING_HPP
#define FACE_STRING_HPP

#include <FACE/types.hpp>
#include <limits.h>  // UINT_MAX  (per §K.2.3)
#include <new>       // std::nothrow
#include <cstring>   // std::memcpy, std::strlen

namespace FACE
{

class String
{
public:

    // -----------------------------------------------------------------------
    // RETURN_CODE
    // -----------------------------------------------------------------------
    enum RETURN_CODE {
        NO_ERROR,               ///< No error has occurred.
        INSUFFICIENT_BOUND,     ///< Executing a function would exceed the bound.
        INSUFFICIENT_MEMORY,    ///< Allocation failed.
        PRECONDITION_VIOLATED   ///< A precondition of the function was violated.
    };

    /// Bound value representing an unbounded String.
    static const UnsignedLong UNBOUNDED_SENTINEL = static_cast<UnsignedLong>(UINT_MAX);

    // -----------------------------------------------------------------------
    // Default constructor — managed, unbounded, empty.
    // No memory is allocated; buffer() returns a valid pointer to "".
    // -----------------------------------------------------------------------
    String()
        : buf_(0), length_(0), capacity_(0),
          bound_(UNBOUNDED_SENTINEL), managed_(true), valid_(true)
    {}

    // -----------------------------------------------------------------------
    // Managed bounded constructor — managed, bounded to @p bound, empty.
    // Preconditions: bound != 0, bound != UNBOUNDED_SENTINEL.
    // -----------------------------------------------------------------------
    String(UnsignedLong bound, RETURN_CODE& return_code)
        : buf_(0), length_(0), capacity_(0),
          bound_(0), managed_(true), valid_(false)
    {
        if (bound == 0 || bound == UNBOUNDED_SENTINEL) {
            return_code = PRECONDITION_VIOLATED;
            return;
        }
        bound_      = bound;
        valid_      = true;
        return_code = NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // Unmanaged constructor — wraps externally managed, NUL-terminated memory.
    // Caller must ensure @p str is NUL-terminated; its lifetime is the
    // caller's responsibility. capacity()/bound() equal strlen(str).
    //
    // The spec takes `const char*` here (matching the common case of wrapping
    // a string literal for read-only use); this class stores it as a mutable
    // pointer for the sake of a uniform buffer()/operator[] API. Callers who
    // construct from a true read-only literal and then mutate through this
    // wrapper invoke undefined behavior at the OS/toolchain level — that
    // risk is inherent to the spec's own signature, not introduced here.
    // -----------------------------------------------------------------------
    explicit String(const char* str)
        : buf_(const_cast<char*>(str)),
          length_(str ? static_cast<UnsignedLong>(std::strlen(str)) : 0),
          capacity_(length_), bound_(length_), managed_(false), valid_(str != 0)
    {}

    // -----------------------------------------------------------------------
    // Managed copy constructor — deep copy; same bound as @p str.
    // -----------------------------------------------------------------------
    String(const String& str)
        : buf_(0), length_(0), capacity_(0),
          bound_(str.bound_), managed_(true), valid_(false)
    {
        if (!str.valid_) return;
        if (!allocate(str.length_)) return;
        if (str.length_ > 0) std::memcpy(buf_, str.buf_ ? str.buf_ : "", str.length_);
        buf_[str.length_] = '\0';
        length_ = str.length_;
        valid_  = true;
    }

    // -----------------------------------------------------------------------
    // Managed assignment operator — deep copy, destroying existing managed
    // data. Acquires bound from @p str.
    // -----------------------------------------------------------------------
    String& operator=(const String& str)
    {
        if (this == &str) return *this;
        if (managed_ && buf_) { delete[] buf_; }
        buf_      = 0;
        length_   = 0;
        capacity_ = 0;
        managed_  = true;
        bound_    = str.bound_;
        valid_    = false;

        if (!str.valid_) return *this;
        if (!allocate(str.length_)) return *this;  // leave in invalid state
        if (str.length_ > 0) std::memcpy(buf_, str.buf_ ? str.buf_ : "", str.length_);
        buf_[str.length_] = '\0';
        length_ = str.length_;
        valid_  = true;
        return *this;
    }

    // -----------------------------------------------------------------------
    // Managed C-string constructor — managed copy of @p str; bound() will
    // equal @p str's length.
    // Preconditions: str != NULL.
    // -----------------------------------------------------------------------
    String(const char* str, RETURN_CODE& return_code)
        : buf_(0), length_(0), capacity_(0),
          bound_(0), managed_(true), valid_(false)
    {
        if (!str) { return_code = PRECONDITION_VIOLATED; return; }
        UnsignedLong len = static_cast<UnsignedLong>(std::strlen(str));
        bound_ = len;
        if (!allocate(len)) { return_code = INSUFFICIENT_MEMORY; return; }
        if (len > 0) std::memcpy(buf_, str, len);
        buf_[len]   = '\0';
        length_     = len;
        valid_      = true;
        return_code = NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // Unmanaged constructor — wraps externally managed memory with explicit
    // length/bound. Caller-managed lifetime.
    // Preconditions: str != NULL, length <= bound, bound != 0,
    //                bound != UNBOUNDED_SENTINEL.
    // -----------------------------------------------------------------------
    String(char* str, UnsignedLong length, UnsignedLong bound,
           RETURN_CODE& return_code)
        : buf_(str), length_(length), capacity_(bound),
          bound_(bound), managed_(false), valid_(false)
    {
        if (!str || length > bound || bound == 0 || bound == UNBOUNDED_SENTINEL) {
            return_code = PRECONDITION_VIOLATED;
            return;
        }
        valid_      = true;
        return_code = NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // reserve — ensure capacity for at least @p capacity characters.
    // Preconditions: is_valid(), is_managed(), !is_bounded() (per spec —
    // reserve() exists to pre-allocate growth room for unbounded managed
    // strings; a bounded string's capacity is fixed by construction).
    // -----------------------------------------------------------------------
    RETURN_CODE reserve(UnsignedLong capacity)
    {
        if (!valid_ || !managed_ || is_bounded()) return PRECONDITION_VIOLATED;
        if (capacity <= capacity_) return NO_ERROR;
        if (!allocate(capacity, /*preserve=*/true)) return INSUFFICIENT_MEMORY;
        return NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // Destructor — frees the buffer only if managed.
    // -----------------------------------------------------------------------
    ~String()
    {
        if (managed_ && buf_) delete[] buf_;
    }

    // -----------------------------------------------------------------------
    // clear — sets length to 0 without releasing memory.
    // -----------------------------------------------------------------------
    void clear()
    {
        length_ = 0;
        if (buf_) buf_[0] = '\0';
    }

    // -----------------------------------------------------------------------
    // append(str) — appends all characters of @p str.
    // -----------------------------------------------------------------------
    RETURN_CODE append(const String& str)
    {
        if (!valid_) return PRECONDITION_VIOLATED;
        for (UnsignedLong i = 0; i < str.length_; ++i) {
            RETURN_CODE rc = append(str.buf_[i]);
            if (rc != NO_ERROR) return rc;
        }
        return NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // append(elem) — appends one character.
    // Grows managed capacity as needed (doubling strategy, capped at bound).
    // -----------------------------------------------------------------------
    RETURN_CODE append(const Char& elem)
    {
        if (!valid_) return PRECONDITION_VIOLATED;
        if (bound_ != UNBOUNDED_SENTINEL && length_ >= bound_)
            return INSUFFICIENT_BOUND;
        if (length_ >= capacity_) {
            UnsignedLong new_cap = (capacity_ == 0) ? 8u : capacity_ * 2u;
            if (bound_ != UNBOUNDED_SENTINEL && new_cap > bound_)
                new_cap = bound_;
            if (!managed_) return INSUFFICIENT_MEMORY;  // cannot grow unmanaged storage
            if (!allocate(new_cap, /*preserve=*/true)) return INSUFFICIENT_MEMORY;
        }
        buf_[length_++] = elem;
        buf_[length_]   = '\0';
        return NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // operator[] — character access by index.
    // If index is out of range, a reference to '\0' is returned (defined
    // behavior per spec, unlike FACE::Sequence's implementation-defined
    // out-of-range access).
    // -----------------------------------------------------------------------
    char& operator[](UnsignedLong index)
    {
        static char nul_char;
        nul_char = '\0';
        if (!valid_ || index >= length_) return nul_char;
        return buf_[index];
    }
    const char& operator[](UnsignedLong index) const
    {
        static const char nul_char = '\0';
        if (!valid_ || index >= length_) return nul_char;
        return buf_[index];
    }

    // -----------------------------------------------------------------------
    // buffer — pointer to NUL-terminated C-string representation.
    // Returns NULL only when the String is invalid; an empty-but-valid
    // String (no memory yet allocated) returns a pointer to "".
    // -----------------------------------------------------------------------
    char* buffer()
    {
        if (!valid_) return 0;
        if (buf_) return buf_;
        static char empty[1] = { '\0' };
        return empty;
    }
    const char* buffer() const
    {
        if (!valid_) return 0;
        if (buf_) return buf_;
        static const char empty[1] = { '\0' };
        return empty;
    }

    // -----------------------------------------------------------------------
    // Observers
    // -----------------------------------------------------------------------
    UnsignedLong length()   const { return valid_ ? length_   : 0u; }
    UnsignedLong capacity() const { return valid_ ? capacity_ : 0u; }
    UnsignedLong bound()    const { return valid_ ? bound_    : 0u; }

    Boolean is_managed() const { return valid_ ? managed_                       : false; }
    Boolean is_bounded() const { return valid_ ? (bound_ != UNBOUNDED_SENTINEL) : false; }
    Boolean is_valid()   const { return valid_; }

private:
    /// (Re)allocates managed storage for at least @p chars characters plus a
    /// NUL terminator, updating capacity_. When @p preserve is true, copies
    /// existing content (length_ characters) into the new buffer first.
    /// Leaves buf_/capacity_ unchanged and returns false on allocation failure.
    bool allocate(UnsignedLong chars, bool preserve = false)
    {
        char* newbuf = new(std::nothrow) char[chars + 1];
        if (!newbuf) return false;
        if (preserve && buf_) {
            std::memcpy(newbuf, buf_, length_);
            newbuf[length_] = '\0';
        } else {
            newbuf[0] = '\0';
        }
        if (buf_) delete[] buf_;
        buf_      = newbuf;
        capacity_ = chars;
        return true;
    }

    char*        buf_;       ///< NUL-terminated character storage (NULL if empty/invalid).
    UnsignedLong length_;    ///< Active character count (excluding NUL).
    UnsignedLong capacity_;  ///< Allocated character count (excluding NUL).
    UnsignedLong bound_;     ///< Maximum allowed length (UNBOUNDED_SENTINEL = unlimited).
    Boolean      managed_;   ///< true = this object owns buf_.
    Boolean      valid_;     ///< false = object is in the invalid state.
};

} // namespace FACE

#endif // FACE_STRING_HPP
