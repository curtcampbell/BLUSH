// FACE/TSS/CSP.hpp
// C++ mapping of FACE/TSS/CSP.idl
// FACE Technical Standard Edition 3.2
//
// Component State Persistence (CSP) interface.
// Allows PCS, PSSS, and TSSS UoCs to persist private or checkpoint data.

#ifndef FACE_TSS_CSP_HPP
#define FACE_TSS_CSP_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {
namespace CSP {

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------

/// Kind of data store: private (local only) or checkpoint (restorable).
enum DATA_STORE_KIND_TYPE {
    PRIVATE_DATA_STORE,     ///< data private to the UoC
    CHECKPOINT_DATA_STORE   ///< data that can be checkpointed/restored
};

/// Opaque token referencing an open data store session.
typedef int64_t DATA_STORE_TOKEN_TYPE;

/// Identifier for a data entry within a data store.
typedef int64_t DATA_ID_TYPE;

//----------------------------------------------------------------------------
// CSP interface
//----------------------------------------------------------------------------

/// Component State Persistence interface.
///
/// Provides CRUD operations over named data stores, supporting both
/// in-memory private state and checkpoint-capable persistent state.
class CSP {
public:
    virtual ~CSP() {}

    /// Initialise the CSP interface.
    ///
    /// @param configuration  Resource locator for CSP configuration.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Initialize(
        const CONFIGURATION_RESOURCE& configuration,
        RETURN_CODE_TYPE&             return_code) = 0;

    /// Open a data store associated with a UoC.
    ///
    /// @param uop_id             GUID identifying the UoC (Unit of Portability).
    /// @param configuration_name Name of the data store configuration entry.
    /// @param type               Kind of data store (private or checkpoint).
    /// @param token              Output: session token for subsequent calls.
    /// @param return_code        Output: NO_ERROR on success.
    virtual void Open(
        GUID_TYPE               uop_id,
        const STRING_TYPE&      configuration_name,
        DATA_STORE_KIND_TYPE    type,
        DATA_STORE_TOKEN_TYPE&  token,
        RETURN_CODE_TYPE&       return_code) = 0;

    /// Close an open data store.
    ///
    /// @param uop_id      GUID of the UoC that owns the session.
    /// @param token       Session token from Open().
    /// @param return_code Output: NO_ERROR on success.
    virtual void Close(
        GUID_TYPE             uop_id,
        DATA_STORE_TOKEN_TYPE token,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Create a new entry in the data store.
    ///
    /// @param uop_id      GUID of the UoC.
    /// @param token       Session token.
    /// @param data_id     Identifier for the new entry.
    /// @param data        Buffer containing the data to store.
    /// @param return_code Output: NO_ERROR on success.
    virtual void Create(
        GUID_TYPE             uop_id,
        DATA_STORE_TOKEN_TYPE token,
        DATA_ID_TYPE          data_id,
        const DATA_BUFFER_TYPE& data,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Read an existing entry from the data store.
    ///
    /// @param uop_id      GUID of the UoC.
    /// @param token       Session token.
    /// @param data_id     Identifier of the entry to read.
    /// @param data        Inout: caller-supplied buffer; filled on success.
    /// @param return_code Output: NO_ERROR on success.
    virtual void Read(
        GUID_TYPE              uop_id,
        DATA_STORE_TOKEN_TYPE  token,
        DATA_ID_TYPE           data_id,
        DATA_BUFFER_TYPE&      data,
        RETURN_CODE_TYPE&      return_code) = 0;

    /// Update an existing entry in the data store.
    ///
    /// @param uop_id      GUID of the UoC.
    /// @param token       Session token.
    /// @param data_id     Identifier of the entry to update.
    /// @param data        New data for the entry.
    /// @param return_code Output: NO_ERROR on success.
    virtual void Update(
        GUID_TYPE             uop_id,
        DATA_STORE_TOKEN_TYPE token,
        DATA_ID_TYPE          data_id,
        const DATA_BUFFER_TYPE& data,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Delete an entry from the data store.
    ///
    /// @param uop_id      GUID of the UoC.
    /// @param token       Session token.
    /// @param data_id     Identifier of the entry to delete.
    /// @param return_code Output: NO_ERROR on success.
    virtual void Delete(
        GUID_TYPE             uop_id,
        DATA_STORE_TOKEN_TYPE token,
        DATA_ID_TYPE          data_id,
        RETURN_CODE_TYPE&     return_code) = 0;
};

} // namespace CSP
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_CSP_HPP
