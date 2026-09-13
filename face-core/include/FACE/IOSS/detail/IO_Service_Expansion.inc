// FACE/IOSS/detail/IO_Service_Expansion.inc
// Textual expansion of IDL:
//   module IO_Service_Module<struct PAYLOAD_DATA_MSG_TYPE> { ... }
// FACE Technical Standard Edition 3.2
//
// USAGE
// -----
// Include this file textually inside a service namespace AFTER defining:
//
//   typedef <YourPayloadType> PAYLOAD_DATA_MSG_TYPE;
//
// Example (from Generic.hpp):
//
//   namespace FACE { namespace IOSS { namespace Generic {
//     struct ReadWriteBuffer { ... };
//     typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
//     #include "detail/IO_Service_Expansion.inc"
//   }}}
//
// The preprocessor substitutes the file contents into the enclosing namespace,
// exactly modelling the IDL-level template module expansion:
//
//   module IO_Service_Module<Generic::ReadWriteBuffer> Generic;
//
// where every occurrence of PAYLOAD_DATA_MSG_TYPE in the template body is
// replaced by Generic::ReadWriteBuffer (here represented by the typedef).
//
// NOTE: No include guards — this file is intended to be included once per
//       service namespace and must NOT be included at file scope.
//
// Requires (already in scope via IOS.hpp → Common.hpp):
//   STRING_TYPE, GUID_TYPE, SYSTEM_TIME_TYPE, TIMEOUT_TYPE,
//   RETURN_CODE_TYPE, CONFIGURATION_RESOURCE,
//   FACE::IOSS::BUS_STATUS_TYPE, FACE::IOSS::IO_PARAMETER_ID_TYPE,
//   FACE::IOSS::CONNECTION_HANDLE_TYPE,
//   std::vector

// -- CONNECTION_NAME_TYPE ---------------------------------------------------

/// The unique name of the I/O connection (passed to Open_Connection).
typedef STRING_TYPE CONNECTION_NAME_TYPE;

// -- IO_CONNECTION_STATUS_TYPE ----------------------------------------------

/// Health of an I/O connection, independent of the underlying device.
enum IO_CONNECTION_STATUS_TYPE {
    NOT_OPEN,    ///< initial state prior to Open_Connection()
    CONNECTING,  ///< connection attempt is in progress
    READY,       ///< connection is open and ready for I/O
    BUSY,        ///< connection is congested; cannot accept more data
    DEGRADED     ///< connection is not fully operational (partial failure)
};

/// Connection health and message-availability snapshot.
struct CONNECTION_STATUS_TYPE {
    SYSTEM_TIME_TYPE          last_message_time; ///< time of last message
    bool                      message_available; ///< true if data is waiting
    IO_CONNECTION_STATUS_TYPE connection_status; ///< current connection health
};

// -- IO_PARAMETER_VALUE_TYPE ------------------------------------------------

/// Discriminant tag selecting the active member of IO_PARAMETER_VALUE_TYPE.
enum IO_PARAMETER_VALUE_TYPES_TYPE {
    FACE_SHORT,
    FACE_LONG,
    FACE_LONGLONG,
    FACE_USHORT,
    FACE_ULONG,
    FACE_ULONGLONG,
    FACE_FLOAT,
    FACE_DOUBLE,
    FACE_LONGDOUBLE,
    FACE_CHAR,
    FACE_BOOLEAN,
    FACE_OCTET
};

/// Discriminated union holding a single I/O parameter value.
struct IO_PARAMETER_VALUE_TYPE {
    IO_PARAMETER_VALUE_TYPES_TYPE type_tag;

    union {
        int16_t      short_value;
        int32_t      long_value;
        int64_t      longlong_value;
        uint16_t     ushort_value;
        uint32_t     ulong_value;
        uint64_t     ulonglong_value;
        float        float_value;
        double       double_value;
        long double  longdouble_value;
        char         char_value;
        bool         boolean_value;
        uint8_t      octet_value;
    } value;

    IO_PARAMETER_VALUE_TYPE() : type_tag(FACE_LONG) { value.long_value = 0; }
};

// -- IO_PARAMETER / IO_PARAMETER_LIST / IO_PARAMETER_TRANSACTION_TYPE -------

/// A single (ID, value) I/O parameter pair.
struct IO_PARAMETER {
    IO_PARAMETER_ID_TYPE    id;
    IO_PARAMETER_VALUE_TYPE value;
};

/// An ordered list of I/O parameters passed in a single transaction.
typedef std::vector<IO_PARAMETER> IO_PARAMETER_LIST;

/// Container for a parameter transaction, differentiated by GUID.
struct IO_PARAMETER_TRANSACTION_TYPE {
    GUID_TYPE         guid;  ///< differentiates concurrent transactions
    IO_PARAMETER_LIST items;
};

// -- Timeout sentinels ------------------------------------------------------

/// Return immediately without waiting for a remote response.
static constexpr TIMEOUT_TYPE NO_WAIT      = 0;

/// Wait indefinitely for a remote response.
static constexpr TIMEOUT_TYPE WAIT_FOREVER = static_cast<TIMEOUT_TYPE>(-1);

// -- NOTIFICATION_EVENT_TYPE ------------------------------------------------

/// Types of asynchronous notification events raised by an I/O Service.
enum NOTIFICATION_EVENT_TYPE {
    DATA_READ_EVENT,                ///< data available via Read()
    CONNECTION_CONFIG_CHANGE_EVENT, ///< call Get_Connection_Configuration()
    CONNECTION_STATUS_CHANGE_EVENT, ///< call Get_Connection_Status()
    BUS_CONFIG_CHANGE_EVENT,        ///< call Get_Bus_Configuration()
    BUS_STATUS_CHANGE_EVENT         ///< call Get_Bus_Status()
};

// -- IO_Callback ------------------------------------------------------------

/// Callback interface for asynchronous I/O event notification.
///
/// NOTE: 'handle' is IGNORED_CONNECTION_HANDLE when notification_event is a
///       bus-level event (BUS_CONFIG_CHANGE_EVENT or BUS_STATUS_CHANGE_EVENT).
class IO_Callback {
public:
    virtual ~IO_Callback() {}

    /// Called by the I/O Service when an event occurs on the registered
    /// connection (or bus).
    virtual void Process_Notification_Event(
        CONNECTION_HANDLE_TYPE  handle,
        NOTIFICATION_EVENT_TYPE notification_event) = 0;
};

// -- IO_Service -------------------------------------------------------------

/// Generic I/O Service interface for this service's payload type.
///
/// READ_PAYLOAD_TYPE and WRITE_PAYLOAD_TYPE are declared as nested structs
/// inside IO_Service, exactly as specified in IOS.idl:
///   interface IO_Service {
///     const SYSTEM_TIME_TYPE IGNORE_RECEIVED_TIME = 0;
///     struct READ_PAYLOAD_TYPE  { SYSTEM_TIME_TYPE received_time;
///                                 PAYLOAD_DATA_MSG_TYPE payload; };
///     struct WRITE_PAYLOAD_TYPE { PAYLOAD_DATA_MSG_TYPE payload; };
///     ...
///   }
class IO_Service {
public:
    virtual ~IO_Service() {}

    /// Sentinel: if READ_PAYLOAD_TYPE::received_time == IGNORE_RECEIVED_TIME,
    /// the timestamp should be disregarded.
    static constexpr SYSTEM_TIME_TYPE IGNORE_RECEIVED_TIME = 0;

    /// Payload returned by Read(), wrapping the bus-specific data with a
    /// receive timestamp.
    struct READ_PAYLOAD_TYPE {
        SYSTEM_TIME_TYPE      received_time; ///< time of last byte into buffer
        PAYLOAD_DATA_MSG_TYPE payload;       ///< bus-specific data
    };

    /// Payload supplied to Write(), wrapping the bus-specific data.
    struct WRITE_PAYLOAD_TYPE {
        PAYLOAD_DATA_MSG_TYPE payload; ///< bus-specific data
    };

    /// Initialise the I/O Service with a configuration resource.
    virtual void Initialize(
        const CONFIGURATION_RESOURCE& config_resource,
        RETURN_CODE_TYPE&             return_code) = 0;

    /// Open a connection to an I/O device; returns a handle.
    virtual void Open_Connection(
        const CONNECTION_NAME_TYPE& name,
        TIMEOUT_TYPE                timeout,
        CONNECTION_HANDLE_TYPE&     handle,
        RETURN_CODE_TYPE&           return_code) = 0;

    /// Close a connection and release the handle.
    virtual void Close_Connection(
        CONNECTION_HANDLE_TYPE handle,
        RETURN_CODE_TYPE&      return_code) = 0;

    /// Synchronously receive (or poll for) payload data.
    virtual void Read(
        CONNECTION_HANDLE_TYPE handle,
        TIMEOUT_TYPE           timeout,
        READ_PAYLOAD_TYPE&     payload,
        RETURN_CODE_TYPE&      return_code) = 0;

    /// Synchronously send payload data.
    virtual void Write(
        CONNECTION_HANDLE_TYPE    handle,
        TIMEOUT_TYPE              timeout,
        const WRITE_PAYLOAD_TYPE& payload,
        RETURN_CODE_TYPE&         return_code) = 0;

    /// Assign I/O parameters for a connection.
    virtual void Configure_Connection_Parameters(
        CONNECTION_HANDLE_TYPE               handle,
        TIMEOUT_TYPE                         timeout,
        const IO_PARAMETER_TRANSACTION_TYPE& parameters,
        RETURN_CODE_TYPE&                    return_code) = 0;

    /// Query I/O parameters for a connection.
    virtual void Get_Connection_Configuration(
        CONNECTION_HANDLE_TYPE         handle,
        TIMEOUT_TYPE                   timeout,
        IO_PARAMETER_TRANSACTION_TYPE& parameters,
        RETURN_CODE_TYPE&              return_code) = 0;

    /// Assign I/O parameters for the bus underlying a connection.
    virtual void Configure_Bus_Parameters(
        CONNECTION_HANDLE_TYPE               handle,
        TIMEOUT_TYPE                         timeout,
        const IO_PARAMETER_TRANSACTION_TYPE& parameters,
        RETURN_CODE_TYPE&                    return_code) = 0;

    /// Query I/O parameters for the bus underlying a connection.
    virtual void Get_Bus_Configuration(
        CONNECTION_HANDLE_TYPE         handle,
        TIMEOUT_TYPE                   timeout,
        IO_PARAMETER_TRANSACTION_TYPE& parameters,
        RETURN_CODE_TYPE&              return_code) = 0;

    /// Query connection status information.
    virtual void Get_Connection_Status(
        CONNECTION_HANDLE_TYPE  handle,
        CONNECTION_STATUS_TYPE& status,
        RETURN_CODE_TYPE&       return_code) = 0;

    /// Query bus status information.
    virtual void Get_Bus_Status(
        CONNECTION_HANDLE_TYPE handle,
        BUS_STATUS_TYPE&       status,
        RETURN_CODE_TYPE&      return_code) = 0;

    /// Register a callback for asynchronous notifications on a connection.
    ///
    /// NOTE: io_callback is semantically 'in' but mapped as inout to avoid
    ///       the C++ const-ref-to-abstract-class binding issue.
    virtual void Register_Notification_Event(
        CONNECTION_HANDLE_TYPE handle,
        IO_Callback&           io_callback,
        RETURN_CODE_TYPE&      return_code) = 0;

    /// Unregister the callback previously associated with a connection.
    virtual void Unregister_Notification_Event(
        CONNECTION_HANDLE_TYPE handle,
        RETURN_CODE_TYPE&      return_code) = 0;
};
