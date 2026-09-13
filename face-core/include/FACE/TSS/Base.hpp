// FACE/TSS/Base.hpp
// C++ mapping of FACE/TSS/Base.idl
// FACE Technical Standard Edition 3.2
//
// Base interface provides the common TSS functions used by PCS and PSSS UoCs
// to manage TS connections.

#ifndef FACE_TSS_BASE_HPP
#define FACE_TSS_BASE_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {

/// Base Transport Services interface.
///
/// Provides connection lifecycle operations (Initialize, Create_Connection,
/// Destroy_Connection) common to all TS implementations.
class Base {
public:
    virtual ~Base() {}

    /// Initialise the TS interface.
    ///
    /// Called by PCS and PSSS UoCs to trigger TS initialisation.
    ///
    /// @param configuration  Resource locator for TS configuration data.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Initialize(
        const CONFIGURATION_RESOURCE& configuration,
        RETURN_CODE_TYPE&             return_code) = 0;

    /// Create a new TS connection.
    ///
    /// Supports DDS, CORBA, ARINC 653, and POSIX connection types as
    /// determined by configuration.
    ///
    /// @param connection_name   Name of the connection to create.
    /// @param timeout           Maximum wait time (INF_TIME_VALUE to block).
    /// @param connection_id     Output: unique ID for the new connection.
    /// @param max_message_size  Output: maximum message size for this connection.
    /// @param return_code       Output: NO_ERROR on success.
    virtual void Create_Connection(
        const CONNECTION_NAME_TYPE& connection_name,
        TIMEOUT_TYPE                timeout,
        CONNECTION_ID_TYPE&         connection_id,
        MESSAGE_SIZE_TYPE&          max_message_size,
        RETURN_CODE_TYPE&           return_code) = 0;

    /// Destroy an existing TS connection.
    ///
    /// @param connection_id  ID of the connection to destroy.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Destroy_Connection(
        CONNECTION_ID_TYPE connection_id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_BASE_HPP
