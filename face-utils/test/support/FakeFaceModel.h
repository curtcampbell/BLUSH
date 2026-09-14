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
// FACE TS 3.2 Appendix E.3.2/E.3.3: the CLIENT (RequesterConnection) and
// SERVER (ResponderConnection) roles of a CLIENT_SERVER connection use
// DIFFERENT TypedTS interfaces, not one shared one:
//   - RequesterConnection uses the combined Extended TypedTS
//     (ReqRespTypedTS/ReqRespReadCallback, below) -- Send_Message_Blocking /
//     Send_Message_Async are client-only.
//   - ResponderConnection uses two ordinary Standard TypedTS connections
//     (RequestTypedTS/RequestReadCallback for receiving the request,
//     ResponseTypedTS for sending the response -- further below) -- "Servers
//     ... do not use Send_Message_Async(TS)".
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

// Extended TypedTS -- CLIENT role only (RequesterConnection). Real interface
// per FACE/TSS/Extended.idl has exactly Send_Message_Blocking and
// Send_Message_Async; no Register_Callback/Unregister_Callback (those belong
// to the Standard interface, below, which the RESPONDER role uses instead).
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
};

class MockReqRespTypedTS : public ReqRespTypedTS {
public:
    MOCK_METHOD(void, Send_Message_Async,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
         FACE::TSS::TRANSACTION_ID_TYPE&, const Request&, ReqRespReadCallback**,
         FACE::RETURN_CODE_TYPE&),
        (override));
};

// ---------------------------------------------------------------------------
// Standard TypedTS pair -- SERVER role only (ResponderConnection).
// FACE TS 3.2 Appendix E.3.2: the responder receives requests exactly like a
// pub/sub subscriber (RequestTypedTS/RequestReadCallback, shaped like
// MsgTypedTS/MsgReadCallback above) and sends responses exactly like a
// pub/sub publisher (ResponseTypedTS, Send_Message only -- Receive_Message /
// Register_Callback / Unregister_Callback exist on the real generated
// interface too but ResponderConnection never calls them on the response
// side, so they're omitted here).
// ---------------------------------------------------------------------------

class RequestReadCallback {
public:
    virtual ~RequestReadCallback() = default;
    virtual void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE    connection_id,
        FACE::TSS::TRANSACTION_ID_TYPE   transaction_id,
        const Request&                   message,
        const FACE::TSS::HEADER_TYPE&    header,
        const FACE::TSS::QoS_EVENT_TYPE& qos_parameters,
        FACE::RETURN_CODE_TYPE&          return_code) = 0;
};

class RequestTypedTS {
public:
    virtual ~RequestTypedTS() = default;

    virtual void Register_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        RequestReadCallback**         callback,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;

    virtual void Unregister_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;
};

class MockRequestTypedTS : public RequestTypedTS {
public:
    MOCK_METHOD(void, Register_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, RequestReadCallback**, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Unregister_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::RETURN_CODE_TYPE&),
        (override));
};

class ResponseTypedTS {
public:
    virtual ~ResponseTypedTS() = default;

    virtual void Send_Message(
        FACE::TSS::CONNECTION_ID_TYPE   connection_id,
        FACE::TIMEOUT_TYPE              timeout,
        FACE::TSS::TRANSACTION_ID_TYPE& transaction_id,
        const Response&                 message,
        FACE::RETURN_CODE_TYPE&         return_code) = 0;
};

class MockResponseTypedTS : public ResponseTypedTS {
public:
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
// Second Standard TypedTS request/response pair, distinct from
// Request/Response above -- needed only by ConnectionResolver_test.cpp's
// CreateRspConnection test, which (unlike ResponderConnection_test.cpp)
// exercises Traits<>-based resolution and therefore can't reuse Request/
// Response: Traits<Request> already means the combined Extended interface
// (RequesterConnection's meaning, used by this same test file's
// CreateReqConnection tests) -- a single C++ type can't carry two Traits<>
// specializations in one translation unit.
// ---------------------------------------------------------------------------

struct RspRequest {
    int value = 0;
};

struct RspResponse {
    int value = 0;
};

class RspRequestReadCallback {
public:
    virtual ~RspRequestReadCallback() = default;
    virtual void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE    connection_id,
        FACE::TSS::TRANSACTION_ID_TYPE   transaction_id,
        const RspRequest&                message,
        const FACE::TSS::HEADER_TYPE&    header,
        const FACE::TSS::QoS_EVENT_TYPE& qos_parameters,
        FACE::RETURN_CODE_TYPE&          return_code) = 0;
};

class RspRequestTypedTS {
public:
    virtual ~RspRequestTypedTS() = default;

    virtual void Register_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        RspRequestReadCallback**      callback,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;

    virtual void Unregister_Callback(
        FACE::TSS::CONNECTION_ID_TYPE connection_id,
        FACE::RETURN_CODE_TYPE&       return_code) = 0;
};

class MockRspRequestTypedTS : public RspRequestTypedTS {
public:
    MOCK_METHOD(void, Register_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, RspRequestReadCallback**, FACE::RETURN_CODE_TYPE&),
        (override));
    MOCK_METHOD(void, Unregister_Callback,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::RETURN_CODE_TYPE&),
        (override));
};

class RspResponseTypedTS {
public:
    virtual ~RspResponseTypedTS() = default;

    virtual void Send_Message(
        FACE::TSS::CONNECTION_ID_TYPE   connection_id,
        FACE::TIMEOUT_TYPE              timeout,
        FACE::TSS::TRANSACTION_ID_TYPE& transaction_id,
        const RspResponse&              message,
        FACE::RETURN_CODE_TYPE&         return_code) = 0;
};

class MockRspResponseTypedTS : public RspResponseTypedTS {
public:
    MOCK_METHOD(void, Send_Message,
        (FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
         FACE::TSS::TRANSACTION_ID_TYPE&, const RspResponse&, FACE::RETURN_CODE_TYPE&),
        (override));
};

class IRspResponseSender {
public:
    virtual ~IRspResponseSender() = default;
    virtual void sendResponse(const RspResponse& response) = 0;
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

template<>
struct Traits<FaceUtilsTest::RspRequest> {
    using TypedTS       = FaceUtilsTest::RspRequestTypedTS;
    using Read_Callback = FaceUtilsTest::RspRequestReadCallback;
};

template<>
struct Traits<FaceUtilsTest::RspResponse> {
    using TypedTS = FaceUtilsTest::RspResponseTypedTS;
};
