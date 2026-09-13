// FACE/types.hpp
// Definitions of C++ types for IDL basic types to C++ mapping.
// FACE Technical Standard Edition 3.2, §K.2.1 and Table 14 (§4.14.8.7.1)
//
// IDL Basic Type → C++ Type mapping (Table 14):
//   short            → FACE::Short            (2 bytes,  -2^15 .. 2^15-1)
//   long             → FACE::Long             (4 bytes,  -2^31 .. 2^31-1)
//   long long        → FACE::LongLong         (8 bytes,  -2^63 .. 2^63-1)
//   unsigned short   → FACE::UnsignedShort    (2 bytes,  0 .. 2^16-1)
//   unsigned long    → FACE::UnsignedLong     (4 bytes,  0 .. 2^32-1)
//   unsigned long long → FACE::UnsignedLongLong (8 bytes, 0 .. 2^64-1)
//   float            → FACE::Float            (4 bytes,  IEEE 754-2008 single)
//   double           → FACE::Double           (8 bytes,  IEEE 754-2008 double)
//   long double      → FACE::LongDouble       (10 bytes, IEEE 754-2008 extended)
//   char             → FACE::Char             (1 byte)
//   boolean          → FACE::Boolean          (1 byte,   true/false)
//   octet            → FACE::Octet            (1 byte,   0 .. 2^8-1)

#ifndef FACE_TYPES_HPP
#define FACE_TYPES_HPP

#include <cstdint>   // int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t

namespace FACE
{

typedef int16_t   Short;
typedef int32_t   Long;
typedef int64_t   LongLong;

typedef uint16_t  UnsignedShort;
typedef uint32_t  UnsignedLong;
typedef uint64_t  UnsignedLongLong;

typedef float       Float;
typedef double      Double;
typedef long double LongDouble;

typedef char  Char;
typedef bool  Boolean;
typedef uint8_t Octet;

} // namespace FACE

#endif // FACE_TYPES_HPP
