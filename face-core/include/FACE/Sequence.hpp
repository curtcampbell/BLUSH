// FACE/Sequence.hpp
// Template class representing a sequence of elements.
// FACE Technical Standard Edition 3.2, §K.2.2
//
// A FACE::Sequence<T> is defined by three characteristics:
//   length   — current number of elements in the Sequence
//   bound    — maximum number of elements the Sequence can ever hold;
//              fixed for the lifetime of the object.  An unbounded Sequence
//              has bound == FACE::Sequence<T>::UNBOUNDED_SENTINEL.
//   capacity — number of elements for which memory is currently allocated;
//              length <= capacity <= bound is always true.
//
// Managed vs. Unmanaged
//   A "managed" Sequence owns and manages the lifetime of its element buffer.
//   An "unmanaged" Sequence wraps a pointer whose lifetime is managed elsewhere.
//
// Invalid state
//   When a constructor encounters a precondition violation or allocation failure
//   the object is placed in the "invalid state":
//     length() == 0, bound() == 0, capacity() == 0
//     buffer() == NULL
//     is_managed() == false, is_bounded() == false
//     is_valid() == false
//   Methods on an invalid Sequence return PRECONDITION_VIOLATED / 0 / NULL.
//
// IDL mapping (§4.14.8.7.2):
//   IDL bounded:   sequence<T, N>  → typedef FACE::Sequence<T> Name;
//                                    #define Name_bound_value ((FACE::UnsignedLong) N)
//   IDL unbounded: sequence<T>     → typedef FACE::Sequence<T> Name;
//                                    #define Name_bound_value FACE::Sequence<T>::UNBOUNDED_SENTINEL
//
// Implementation note: this file provides a concrete, fully implemented
// version of the §K.2.2 specification skeleton.  It uses operator new
// (via std::nothrow) for managed storage, making it suitable for simulation
// and general-purpose FACE CE environments.  Environments that prohibit
// dynamic allocation should substitute a static-storage implementation.

#ifndef FACE_SEQUENCE_HPP
#define FACE_SEQUENCE_HPP

#include <FACE/types.hpp>
#include <limits.h>  // UINT_MAX  (per §K.2.2)
#include <new>       // std::nothrow

namespace FACE
{

template <typename T>
class Sequence
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

    /// Bound value representing an unbounded Sequence.
    static const UnsignedLong UNBOUNDED_SENTINEL = static_cast<UnsignedLong>(UINT_MAX);

    // -----------------------------------------------------------------------
    // Default constructor — managed, unbounded, empty.
    // -----------------------------------------------------------------------
    Sequence()
        : buf_(0), length_(0), capacity_(0),
          bound_(UNBOUNDED_SENTINEL), managed_(true), valid_(true)
    {}

    // -----------------------------------------------------------------------
    // Managed bounded constructor — managed, bounded to @p bound, empty.
    // If allocation is successful, the Sequence will be empty.
    // -----------------------------------------------------------------------
    Sequence(UnsignedLong bound, RETURN_CODE& return_code)
        : buf_(0), length_(0), capacity_(0),
          bound_(bound), managed_(true), valid_(true)
    {
        return_code = NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // Managed copy constructor — deep copy; same bound as @p seq.
    // -----------------------------------------------------------------------
    Sequence(const Sequence& seq)
        : buf_(0), length_(0), capacity_(0),
          bound_(seq.bound_), managed_(true), valid_(false)
    {
        if (!seq.valid_) return;
        if (seq.length_ > 0) {
            buf_ = new(std::nothrow) T[seq.length_];
            if (!buf_) return;  // invalid state
            for (UnsignedLong i = 0; i < seq.length_; ++i)
                buf_[i] = seq.buf_[i];
            capacity_ = seq.length_;
        }
        length_ = seq.length_;
        valid_  = true;
    }

    // -----------------------------------------------------------------------
    // Managed C-array constructor.
    // Creates a managed copy of @p length elements at @p arr.
    // bound() will equal @p length after construction.
    //
    // Preconditions: arr != NULL
    // On precondition violation: PRECONDITION_VIOLATED, invalid state.
    // On allocation failure:     INSUFFICIENT_MEMORY, invalid state.
    // -----------------------------------------------------------------------
    Sequence(const T* arr, UnsignedLong length, RETURN_CODE& return_code)
        : buf_(0), length_(0), capacity_(0),
          bound_(length), managed_(true), valid_(false)
    {
        if (!arr) { return_code = PRECONDITION_VIOLATED; return; }
        if (length > 0) {
            buf_ = new(std::nothrow) T[length];
            if (!buf_) { return_code = INSUFFICIENT_MEMORY; return; }
            for (UnsignedLong i = 0; i < length; ++i)
                buf_[i] = arr[i];
            capacity_ = length;
        }
        length_      = length;
        valid_       = true;
        return_code  = NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // Unmanaged constructor — wraps externally managed memory.
    // The caller is responsible for the lifetime of @p seq.
    // bound() == @p bound; capacity() == @p bound.
    //
    // Preconditions: seq != NULL
    // On precondition violation: PRECONDITION_VIOLATED, invalid state.
    // -----------------------------------------------------------------------
    Sequence(T* seq, UnsignedLong length, UnsignedLong bound,
             RETURN_CODE& return_code)
        : buf_(seq), length_(length), capacity_(bound),
          bound_(bound), managed_(false), valid_(false)
    {
        if (!seq) { return_code = PRECONDITION_VIOLATED; return; }
        valid_      = true;
        return_code = NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // Managed assignment operator — deep copy, destroying existing managed data.
    // Acquires bound from @p seq.
    // -----------------------------------------------------------------------
    Sequence& operator=(const Sequence& seq)
    {
        if (this == &seq) return *this;
        // Release current managed buffer.
        if (managed_ && buf_) { delete[] buf_; buf_ = 0; }
        length_   = 0;
        capacity_ = 0;
        managed_  = true;
        bound_    = seq.bound_;
        valid_    = false;

        if (!seq.valid_) return *this;
        if (seq.length_ > 0) {
            buf_ = new(std::nothrow) T[seq.length_];
            if (!buf_) return *this;  // leave in invalid state
            for (UnsignedLong i = 0; i < seq.length_; ++i)
                buf_[i] = seq.buf_[i];
            capacity_ = seq.length_;
        }
        length_ = seq.length_;
        valid_  = true;
        return *this;
    }

    // -----------------------------------------------------------------------
    // Destructor — frees the buffer only if managed.
    // -----------------------------------------------------------------------
    ~Sequence()
    {
        if (managed_ && buf_) delete[] buf_;
    }

    // -----------------------------------------------------------------------
    // reserve — ensure capacity for at least @p cap elements.
    // No-op if capacity already sufficient.  Unmanaged Sequences cannot be
    // resized (PRECONDITION_VIOLATED).
    // -----------------------------------------------------------------------
    RETURN_CODE reserve(UnsignedLong cap)
    {
        if (!valid_)    return PRECONDITION_VIOLATED;
        if (!managed_)  return PRECONDITION_VIOLATED;
        if (cap <= capacity_) return NO_ERROR;
        if (bound_ != UNBOUNDED_SENTINEL && cap > bound_)
            return INSUFFICIENT_BOUND;
        T* newbuf = new(std::nothrow) T[cap];
        if (!newbuf) return INSUFFICIENT_MEMORY;
        for (UnsignedLong i = 0; i < length_; ++i)
            newbuf[i] = buf_[i];
        if (buf_) delete[] buf_;
        buf_      = newbuf;
        capacity_ = cap;
        return NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // clear — sets length to 0 without releasing memory.
    // -----------------------------------------------------------------------
    void clear()
    {
        length_ = 0;
    }

    // -----------------------------------------------------------------------
    // append(seq) — appends all elements of @p seq.
    // Returns INSUFFICIENT_BOUND if the result would exceed this Sequence's
    // bound.  Returns INSUFFICIENT_MEMORY on allocation failure.
    // -----------------------------------------------------------------------
    RETURN_CODE append(const Sequence& seq)
    {
        if (!valid_) return PRECONDITION_VIOLATED;
        for (UnsignedLong i = 0; i < seq.length_; ++i) {
            RETURN_CODE rc = append(seq.buf_[i]);
            if (rc != NO_ERROR) return rc;
        }
        return NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // append(elem) — appends one element.
    // Grows managed capacity as needed (doubling strategy, capped at bound).
    // -----------------------------------------------------------------------
    RETURN_CODE append(const T& elem)
    {
        if (!valid_) return PRECONDITION_VIOLATED;
        if (bound_ != UNBOUNDED_SENTINEL && length_ >= bound_)
            return INSUFFICIENT_BOUND;
        if (length_ >= capacity_) {
            UnsignedLong new_cap = (capacity_ == 0) ? 4u : capacity_ * 2u;
            if (bound_ != UNBOUNDED_SENTINEL && new_cap > bound_)
                new_cap = bound_;
            RETURN_CODE rc = reserve(new_cap);
            if (rc != NO_ERROR) return rc;
        }
        buf_[length_++] = elem;
        return NO_ERROR;
    }

    // -----------------------------------------------------------------------
    // operator[] — element access by index.
    // If index is out of range, behavior is implementation-defined (§K.2.2).
    // -----------------------------------------------------------------------
    T& operator[](UnsignedLong index)             { return buf_[index]; }
    const T& operator[](UnsignedLong index) const { return buf_[index]; }

    // -----------------------------------------------------------------------
    // buffer — pointer to contiguous element storage.
    // Returns NULL when the Sequence is empty or invalid.
    // -----------------------------------------------------------------------
    T*       buffer()       { return buf_; }
    const T* buffer() const { return buf_; }

    // -----------------------------------------------------------------------
    // Observers
    // -----------------------------------------------------------------------
    UnsignedLong length()   const { return valid_ ? length_   : 0u; }
    UnsignedLong capacity() const { return valid_ ? capacity_ : 0u; }
    UnsignedLong bound()    const { return valid_ ? bound_    : 0u; }

    Boolean is_managed()  const { return valid_ ? managed_                       : false; }
    Boolean is_bounded()  const { return valid_ ? (bound_ != UNBOUNDED_SENTINEL) : false; }
    Boolean is_valid()    const { return valid_; }

private:
    T*           buf_;       ///< Pointer to element storage (NULL if empty/invalid).
    UnsignedLong length_;    ///< Active element count.
    UnsignedLong capacity_;  ///< Allocated element count.
    UnsignedLong bound_;     ///< Maximum allowed length (UNBOUNDED_SENTINEL = unlimited).
    Boolean      managed_;   ///< true = this object owns buf_.
    Boolean      valid_;     ///< false = object is in the invalid state.
};

} // namespace FACE

#endif // FACE_SEQUENCE_HPP
