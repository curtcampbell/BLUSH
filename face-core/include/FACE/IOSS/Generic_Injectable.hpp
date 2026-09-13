// FACE/IOSS/Generic_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::IOSS::Generic::IO_Service> IO_Service_Injectable;
//   (inside module FACE::IOSS::Generic)
// FACE Technical Standard Edition 3.2
//
// Expanded IDL:
//   module FACE { module IOSS { module Generic {
//     module IO_Service_Injectable {
//       interface Injectable {
//         void Set_Reference(
//           in    STRING_TYPE                  interface_name,
//           inout FACE::IOSS::Generic::IO_Service interface_reference,
//           in    GUID_TYPE                    id,
//           out   RETURN_CODE_TYPE             return_code);
//       };
//     };
//   }; }; };

#ifndef FACE_IOSS_GENERIC_INJECTABLE_HPP
#define FACE_IOSS_GENERIC_INJECTABLE_HPP

#include "../Common.hpp"
#include "Generic.hpp"

namespace FACE {
namespace IOSS {
namespace Generic {
namespace IO_Service_Injectable {

/// Expanded Injectable interface for FACE::IOSS::Generic::IO_Service.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        IO_Service*&       interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace IO_Service_Injectable
} // namespace Generic
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_GENERIC_INJECTABLE_HPP
