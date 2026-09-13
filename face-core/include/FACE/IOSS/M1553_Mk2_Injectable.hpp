// FACE/IOSS/M1553_Mk2_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::IOSS::M1553_Mk2::IO_Service> IO_Service_Injectable;
//   (inside module FACE::IOSS::M1553_Mk2)
// FACE Technical Standard Edition 3.2
//
// Expanded IDL:
//   module FACE { module IOSS { module M1553_Mk2 {
//     module IO_Service_Injectable {
//       interface Injectable {
//         void Set_Reference(
//           in    STRING_TYPE                  interface_name,
//           inout FACE::IOSS::M1553_Mk2::IO_Service interface_reference,
//           in    GUID_TYPE                    id,
//           out   RETURN_CODE_TYPE             return_code);
//       };
//     };
//   }; }; };

#ifndef FACE_IOSS_M1553_MK2_INJECTABLE_HPP
#define FACE_IOSS_M1553_MK2_INJECTABLE_HPP

#include "../Common.hpp"
#include "M1553_Mk2.hpp"

namespace FACE {
namespace IOSS {
namespace M1553_Mk2 {
namespace IO_Service_Injectable {

/// Expanded Injectable interface for FACE::IOSS::M1553_Mk2::IO_Service.
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
} // namespace M1553_Mk2
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_M1553_MK2_INJECTABLE_HPP
