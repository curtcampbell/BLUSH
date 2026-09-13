// FACE/IOSS/Analog_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::IOSS::Analog::IO_Service> IO_Service_Injectable;
//   (inside module FACE::IOSS::Analog)
// FACE Technical Standard Edition 3.2
//
// Expanded IDL:
//   module FACE { module IOSS { module Analog {
//     module IO_Service_Injectable {
//       interface Injectable {
//         void Set_Reference(
//           in    STRING_TYPE                  interface_name,
//           inout FACE::IOSS::Analog::IO_Service interface_reference,
//           in    GUID_TYPE                    id,
//           out   RETURN_CODE_TYPE             return_code);
//       };
//     };
//   }; }; };

#ifndef FACE_IOSS_ANALOG_INJECTABLE_HPP
#define FACE_IOSS_ANALOG_INJECTABLE_HPP

#include "../Common.hpp"
#include "Analog.hpp"

namespace FACE {
namespace IOSS {
namespace Analog {
namespace IO_Service_Injectable {

/// Expanded Injectable interface for FACE::IOSS::Analog::IO_Service.
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
} // namespace Analog
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_ANALOG_INJECTABLE_HPP
