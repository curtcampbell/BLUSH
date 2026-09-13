// FACE/Configuration.hpp
// C++ mapping of FACE/Configuration.idl
// FACE Technical Standard Edition 3.2
//
// Provides the Configuration API: file/stream-style access to configuration
// resources identified by container name and set name.

#ifndef FACE_CONFIGURATION_HPP
#define FACE_CONFIGURATION_HPP

#include "Common.hpp"

namespace FACE {

/// The Configuration interface provides access to configuration data
/// organised into named containers and named sets within those containers.
class Configuration {
public:
    virtual ~Configuration() {}

    //------------------------------------------------------------------------
    // Nested types (scoped inside the interface per IDL)
    //------------------------------------------------------------------------

    /// Session handle returned by Open().
    typedef int32_t HANDLE_TYPE;

    /// Implementation-specific initialisation string.
    typedef STRING_TYPE INITIALIZATION_TYPE;

    /// Name of the configuration container to open.
    typedef STRING_TYPE CONTAINER_NAME_TYPE;

    /// Name of a configuration set within a container.
    typedef STRING_TYPE SET_NAME_TYPE;

    /// Buffer size / bytes-read count.
    typedef int32_t BUFFER_SIZE_TYPE;

    /// Seek offset (bytes).
    typedef int32_t OFFSET_TYPE;

    /// Seek reference point, mirroring POSIX seek origins.
    enum WHENCE_TYPE {
        SEEK_FROM_START,    ///< offset from beginning of the resource
        SEEK_FROM_CURRENT,  ///< offset from the current position
        SEEK_FROM_END       ///< offset from the end of the resource
    };

    //------------------------------------------------------------------------
    // Operations
    //------------------------------------------------------------------------

    /// Initialise the Configuration implementation.
    ///
    /// @param initialization_information  Implementation-specific init string.
    /// @param return_code                 Output: NO_ERROR on success.
    virtual void Initialize(
        const INITIALIZATION_TYPE& initialization_information,
        RETURN_CODE_TYPE&          return_code) = 0;

    /// Open a session with the named configuration container.
    ///
    /// @param container_name  Name of the container to open.
    /// @param handle          Output: session handle for subsequent calls.
    /// @param return_code     Output: NO_ERROR on success;
    ///                        INVALID_CONFIG if the container is unknown;
    ///                        INVALID_PARAM if handle pointer is bad;
    ///                        INVALID_MODE if caller lacks permission.
    virtual void Open(
        const CONTAINER_NAME_TYPE& container_name,
        HANDLE_TYPE&               handle,
        RETURN_CODE_TYPE&          return_code) = 0;

    /// Query the byte size of a named configuration set.
    ///
    /// @param handle       Session handle from Open().
    /// @param set_name     Name of the set ("" for streaming sources).
    /// @param size         Output: size in bytes.
    /// @param return_code  Output: NO_ERROR, INVALID_CONFIG, INVALID_PARAM,
    ///                     or NOT_AVAILABLE.
    virtual void Get_Size(
        HANDLE_TYPE       handle,
        const SET_NAME_TYPE& set_name,
        BUFFER_SIZE_TYPE& size,
        RETURN_CODE_TYPE& return_code) = 0;

    /// Read configuration data into a caller-supplied buffer.
    ///
    /// @param handle       Session handle from Open().
    /// @param set_name     Name of the set ("all" for streaming sources).
    /// @param buffer       Pointer to the destination buffer.
    /// @param buffer_size  Maximum bytes to read.
    /// @param bytes_read   Output: actual bytes read.
    /// @param return_code  Output: NO_ERROR, INVALID_CONFIG, INVALID_PARAM,
    ///                     or NOT_AVAILABLE (stream exhausted).
    virtual void Read(
        HANDLE_TYPE         handle,
        const SET_NAME_TYPE& set_name,
        SYSTEM_ADDRESS_TYPE buffer,
        BUFFER_SIZE_TYPE    buffer_size,
        BUFFER_SIZE_TYPE&   bytes_read,
        RETURN_CODE_TYPE&   return_code) = 0;

    /// Reposition the current position within the configuration session.
    ///
    /// @param handle       Session handle from Open().
    /// @param whence       Interpretation of @a offset.
    /// @param offset       Byte offset relative to @a whence.
    /// @param return_code  Output: NO_ERROR or INVALID_PARAM.
    virtual void Seek(
        HANDLE_TYPE       handle,
        WHENCE_TYPE       whence,
        OFFSET_TYPE       offset,
        RETURN_CODE_TYPE& return_code) = 0;

    /// Close the session and release associated resources.
    ///
    /// @param handle       Session handle to close.
    /// @param return_code  Output: NO_ERROR or INVALID_CONFIG.
    virtual void Close(
        HANDLE_TYPE       handle,
        RETURN_CODE_TYPE& return_code) = 0;
};

} // namespace FACE

#endif // FACE_CONFIGURATION_HPP
