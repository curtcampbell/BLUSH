// FACE/TSS/MT_Utility_Injectable.hpp
// C++ mapping of FACE/TSS/MT_Utility_Injectable.idl
// FACE Technical Standard Edition 3.2
//
// Concrete expansion of:
//   module Injectable<FACE::TSS::MT_Utility_IF_Lookup>
//       MT_Utility_IF_Lookup_Injectable;
//
// IDL expansion process (3 steps):
//   Step 1 - IDL instantiation (MT_Utility_Injectable.idl):
//     module Injectable<FACE::TSS::MT_Utility_IF_Lookup>
//         MT_Utility_IF_Lookup_Injectable;
//
//   Step 2 - Expand Injectable<> template body substituting
//     MT_Utility_IF_Lookup for INTERFACE_TYPE throughout.
//
//   Step 3 - Apply C++ language binding:
//     namespace FACE { namespace TSS {
//       namespace MT_Utility_IF_Lookup_Injectable {
//         class Injectable {
//           virtual void Set_Reference(
//               const STRING_TYPE&, MT_Utility_IF_Lookup*&,
//               const GUID_TYPE&, RETURN_CODE_TYPE&) = 0;
//         };
//       }
//     }}

#ifndef FACE_TSS_MT_UTILITY_INJECTABLE_HPP
#define FACE_TSS_MT_UTILITY_INJECTABLE_HPP

#include "../Common.hpp"
#include "Message_Type_Utilities.hpp"

namespace FACE {
namespace TSS {

/// Concrete expansion of Injectable<MT_Utility_IF_Lookup>.
///
/// Implementations allow the platform to inject a MT_Utility_IF_Lookup
/// reference into a component by name and GUID.
namespace MT_Utility_IF_Lookup_Injectable {

class Injectable {
public:
    virtual ~Injectable() {}

    /// Inject a MT_Utility_IF_Lookup implementation.
    ///
    /// @param instance_name  Logical name identifying the interface instance.
    /// @param interface_ptr  Inout: set to the injected implementation pointer.
    /// @param instance_guid  GUID of the interface instance.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Set_Reference(
        const STRING_TYPE&      instance_name,
        MT_Utility_IF_Lookup*&  interface_ptr,
        const GUID_TYPE&        instance_guid,
        RETURN_CODE_TYPE&       return_code) = 0;
};

} // namespace MT_Utility_IF_Lookup_Injectable

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_MT_UTILITY_INJECTABLE_HPP
