// FACE/Injectable.hpp
// Documents the IDL template module FACE/Injectable.idl
// FACE Technical Standard Edition 3.2
//
// IDL TEMPLATE MODULE (never used directly; only through instantiations)
// -----------------------------------------------------------------------
// The IDL defines:
//   module FACE {
//     module Injectable<interface INTERFACE_TYPE> {
//       interface Injectable {
//         void Set_Reference(
//           in    STRING_TYPE    interface_name,
//           inout INTERFACE_TYPE interface_reference,
//           in    GUID_TYPE      id,
//           out   RETURN_CODE_TYPE return_code);
//       };
//     };
//   };
//
// Each instantiation in the IDL files (e.g.
//   module Injectable<FACE::Configuration> Configuration_Injectable;
// ) expands this template by substituting the concrete interface type for
// INTERFACE_TYPE and naming the result.  The C++ binding is then applied
// to the expanded (concrete) IDL, yielding a plain abstract class (NOT a
// C++ template) in the named namespace.
//
// See the individual *_Injectable.hpp files for the expanded C++ bindings.

#ifndef FACE_INJECTABLE_HPP
#define FACE_INJECTABLE_HPP

#include "Common.hpp"

// No C++ declarations here.  All code lives in the expanded files.

#endif // FACE_INJECTABLE_HPP
