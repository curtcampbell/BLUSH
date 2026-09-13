// FACE/IOSS/I2C_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::IOSS::I2C::Combined_RW_IO_Service>
//       Combined_RW_IO_Service_Injectable;
//   (inside module FACE::IOSS::I2C)
// FACE Technical Standard Edition 3.2
//
// NOTE: The I2C Injectable injects Combined_RW_IO_Service (the extended
// interface), not IO_Service -- as specified in I2C_Injectable.idl.
//
// Expanded IDL:
//   module FACE { module IOSS { module I2C {
//     module Combined_RW_IO_Service_Injectable {
//       interface Injectable {
//         void Set_Reference(
//           in    STRING_TYPE                              interface_name,
//           inout FACE::IOSS::I2C::Combined_RW_IO_Service interface_reference,
//           in    GUID_TYPE                               id,
//           out   RETURN_CODE_TYPE                        return_code);
//       };
//     };
//   }; }; };

#ifndef FACE_IOSS_I2C_INJECTABLE_HPP
#define FACE_IOSS_I2C_INJECTABLE_HPP

#include "../Common.hpp"
#include "I2C.hpp"

namespace FACE {
namespace IOSS {
namespace I2C {
namespace Combined_RW_IO_Service_Injectable {

/// Expanded Injectable interface for FACE::IOSS::I2C::Combined_RW_IO_Service.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE&       interface_name,
        Combined_RW_IO_Service*& interface_reference,
        GUID_TYPE                id,
        RETURN_CODE_TYPE&        return_code) = 0;
};

} // namespace Combined_RW_IO_Service_Injectable
} // namespace I2C
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_I2C_INJECTABLE_HPP
