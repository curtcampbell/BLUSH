// FACE/Configuration_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::Configuration> Configuration_Injectable;
// FACE Technical Standard Edition 3.2
//
// Expanded IDL:
//   module FACE {
//     module Configuration_Injectable {
//       interface Injectable {
//         void Set_Reference(
//           in    STRING_TYPE         interface_name,
//           inout FACE::Configuration interface_reference,
//           in    GUID_TYPE           id,
//           out   RETURN_CODE_TYPE    return_code);
//       };
//     };
//   };

#ifndef FACE_CONFIGURATION_INJECTABLE_HPP
#define FACE_CONFIGURATION_INJECTABLE_HPP

#include "Common.hpp"
#include "Configuration.hpp"

namespace FACE {
namespace Configuration_Injectable {

/// Expanded Injectable interface for FACE::Configuration.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        Configuration*&    interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace Configuration_Injectable
} // namespace FACE

#endif // FACE_CONFIGURATION_INJECTABLE_HPP
