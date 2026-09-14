#pragma once

// Test-only stand-ins for FACE-IDL-generated bindings.
//
// Real consumers get TypedTS / Read_Callback / Base types from IDL generated
// by FACE-IDL-Parser (https://github.com/curtcampbell/FACE-IDL-Parser) against
// an application data model. This repo has no generated model, so these fakes
// reproduce exactly the method shapes that PublisherConnection,
// SubscriberConnection, RequesterConnection, ResponderConnection, and
// ConnectionResolver call, so those classes can be exercised without codegen.

#include "FaceTypeTraits.h"
#include "FACE/Common.hpp"
#include "FACE/TSS/Base.hpp"
#include "FACE/TSS/Common.hpp"

#include <gmock/gmock.h>

namespace FaceUtilsTest {

// ---------------------------------------------------------------------------
// Pub/Sub message type -- single-DataType connections
// (mirrors the shape of FACE::TSS::Typed<DATATYPE_TYPE> in TypedTS.hpp)
// ---------------------------------------------------------------------------

struct Msg {
    int value = 0;
};
inline bool operator==(const Msg& a, const Msg& b) { return a.value == b.value; }

class MsgReadCallback {
public:
    virtual ~MsgReadCallback() = default;
    virtual void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE    connection_id,
        FACE::TSS::TRANSACTION_ID_TYPE   transaction_id,
        const Msg&                       message,
        const FACE::TSS::HEADER_TYPE&    header,
        const FACE::TSS::QoS_EVENT_TYPE& qos_parameters,
        FACE::RETURN_CODE_TYPE&          return_code) = 0;
};

class MsgTypedTS {
public:
    virtual ~MsgTypedTS() = default;

    virtual void Receive_Message(
        FACE::TSS::CONNECTION_ID_TYPE   connection_id,
        FACE::TIMEOUT_TYPE              timeout,
        FACE::TSS::TRANSACTION_ID_TYPE& transaction_id,
        Msg&                            message,
        FACE::TSS::HEADER_TYPE&         header,
        FACE::TSS::QoS_EVENT_TYPE&      qos_parameters,
        FACE::RETURN_CODE_TYPE&         return_code) = 0;

    virtual void Send_Message(
        FACE::TSS::CONNECTION_ID_TYPE   connection_id,
        FACE::TIMEOUT_TYPE              timeout,
        FACE::TSS::TRANSACTION_ID_TYPE& transaction_id,
        const Msg&                      message,
        FACE::RETURN_CODE_TYPE&         return_code) = 0;

    // "inout Read_Callback" where Read_Callback is a LOCAL interface (declared
    // alongside TypedTS in the same template body) maps to Read_Callback**,
    // not a reference -- matches real generated TypedTS.hpp (confirmed
    // against FACE/TSS/CORE_Templates/Money/TypedTS.hpp).
    virtual void Register_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        MsgReadCallback**             callback,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;

    virtual void Unregister_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;
};

class MockMsgTypedTS : public MsgTypedTS {
public:
    MOCK_METHOD(void, Receive_Message,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
         FACE::TSS::TRANSACTION_ID_TYPE&, Msg&, FACE::TSS::HEADER_TYPE&,
         FACE::TSS::QoS_EVENT_TYPE&, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Send_Message,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
         FACE::TSS::TRANSACTION_ID_TYPE&, const Msg&, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Register_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, MsgReadCallback**, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Unregister_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::RETURN_CODE_TYPE&),
        (override));
};

// ---------------------------------------------------------------------------
// Request/Response types -- CLIENT_SERVER connections
//
// A real generated CLIENT_SERVER module serves both the client role
// (RequesterConnection: send a request, receive the response via callback)
// and the server role (ResponderConnection: receive a request via callback,
// send the response) on one connection id. ReqRespReadCallback therefore
// overloads Callback_Handler for both message directions; each concrete user
// (AsyncCallbackAdapter, ResponderConnection) overrides only the overload it
// needs and inherits the other as a no-op.
// ---------------------------------------------------------------------------

struct Request {
    int value = 0;
};

struct Response {
    int value = 0;
};

class ReqRespReadCallback {
public:
    virtual ~ReqRespReadCallback() = default;

    virtual void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE, FACE::TSS::TRANSACTION_ID_TYPE,
        const Request&, const FACE::TSS::HEADER_TYPE&,
        const FACE::TSS::QoS_EVENT_TYPE&, FACE::RETURN_CODE_TYPE& return_code)
    {
        return_code = FACE::RETURN_CODE_TYPE::NOT_AVAILABLE;
    }

    virtual void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE, FACE::TSS::TRANSACTION_ID_TYPE,
        const Response&, const FACE::TSS::HEADER_TYPE&,
        const FACE::TSS::QoS_EVENT_TYPE&, FACE::RETURN_CODE_TYPE& return_code)
    {
        return_code = FACE::RETURN_CODE_TYPE::NOT_AVAILABLE;
    }
};

class ReqRespTypedTS {
public:
    virtual ~ReqRespTypedTS() = default;

    virtual void Send_Message_Async(
        FACE::TSS::CONNECTION_ID_TYPE   connection_id,
        FACE::TIMEOUT_TYPE              timeout,
        FACE::TSS::TRANSACTION_ID_TYPE& transaction_id,
        const Request&                  message,
        ReqRespReadCallback**           callback,
        FACE::RETURN_CODE_TYPE&         return_code) = 0;

    virtual void Register_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        ReqRespReadCallback&          callback,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;

    virtual void Unregister_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;

    virtual void Send_Message(
        FACE::TSS::CONNECTION_ID_TYPE   connection_id,
        FACE::TIMEOUT_TYPE              timeout,
        FACE::TSS::TRANSACTION_ID_TYPE& transaction_id,
        const Response&                 message,
        FACE::RETURN_CODE_TYPE&         return_code) = 0;
};

class MockReqRespTypedTS : public ReqRespTypedTS {
public:
    MOCK_METHOD(void, Send_Message_Async,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
         FACE::TSS::TRANSACTION_ID_TYPE&, const Request&, ReqRespReadCallback**,
         FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Register_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, ReqRespReadCallback&, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Unregister_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Send_Message,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
         FACE::TSS::TRANSACTION_ID_TYPE&, const Response&, FACE::RETURN_CODE_TYPE&),
        (override));
};

// Generated I{TypeName}ResponseSender interface for ResponderConnection.
class IResponseSender {
public:
    virtual ~IResponseSender() = default;
    virtual void sendResponse(const Response& response) = 0;
};

// ---------------------------------------------------------------------------
// FACE::TSS::Base fake, for ConnectionResolver
// ---------------------------------------------------------------------------

class MockBase : public FACE::TSS::Base {
public:
    MOCK_METHOD(void, Initialize,
        (const FACE::CONFIGURATION_RESOURCE&, FACE::RETURN_CODE_TYPE&), (override));
    MOCK_METHOD(void, Create_Connection,
        (const FACE::TSS::CONNECTION_NAME_TYPE&, FACE::TIMEOUT_TYPE,
         FACE::TSS::CONNECTION_ID_TYPE&, FACE::TSS::MESSAGE_SIZE_TYPE&,
         FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Destroy_Connection,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::RETURN_CODE_TYPE&), (override));
};

} // namespace FaceUtilsTest

template<>
struct Traits<FaceUtilsTest::Msg> {
    using TypedTS       = FaceUtilsTest::MsgTypedTS;
    using Read_Callback = FaceUtilsTest::MsgReadCallback;
};

template<>
struct Traits<FaceUtilsTest::Request> {
    using TypedTS       = FaceUtilsTest::ReqRespTypedTS;
    using Read_Callback = FaceUtilsTest::ReqRespReadCallback;
};
