#include "ResponderConnection.h"
#include "support/FakeFaceModel.h"

#include <gtest/gtest.h>

using FaceUtilsTest::IResponseSender;
using FaceUtilsTest::MockRequestTypedTS;
using FaceUtilsTest::MockResponseTypedTS;
using FaceUtilsTest::Request;
using FaceUtilsTest::RequestReadCallback;
using FaceUtilsTest::RequestTypedTS;
using FaceUtilsTest::Response;
using FaceUtilsTest::ResponseTypedTS;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace {
constexpr FACE::TSS::CONNECTION_ID_TYPE  kConnectionId  = 21;
constexpr FACE::TSS::TRANSACTION_ID_TYPE kTransactionId = 5;

FACE::TSS::HEADER_TYPE    kHeader{};
FACE::TSS::QoS_EVENT_TYPE kQos{};

// FACE TS 3.2 Appendix E.3.2/E.3.3: the responder uses two ordinary Standard
// TypedTS connections (request-receive, response-send), NOT the combined
// Extended interface RequesterConnection uses -- so, unlike RequesterConnection,
// the template args here can't default off Traits<Request>/Traits<Response>
// (those are already claimed by RequesterConnection_test's combined-interface
// meaning); pass all six explicitly instead.
using Responder = ResponderConnection<
    Request, Response, IResponseSender,
    RequestTypedTS, RequestReadCallback, ResponseTypedTS>;

void DeliverRequest(Responder& conn, const Request& req) {
    FACE::RETURN_CODE_TYPE rc;
    conn.Callback_Handler(kConnectionId, kTransactionId, req, kHeader, kQos, rc);
}

} // namespace

TEST(ResponderConnectionTest, FirstRegisterHandlerRegistersCallbackOnce) {
    NiceMock<MockRequestTypedTS>  requestTs;
    NiceMock<MockResponseTypedTS> responseTs;
    Responder conn(&requestTs, &responseTs, kConnectionId);

    EXPECT_CALL(requestTs, Register_Callback(kConnectionId, _, _)).Times(1);

    conn.registerHandler([](const Request&, IResponseSender&) {});
    conn.registerHandler([](const Request&, IResponseSender&) {});
}

TEST(ResponderConnectionTest, HandlerCanSendResponseThroughSender) {
    NiceMock<MockRequestTypedTS>  requestTs;
    NiceMock<MockResponseTypedTS> responseTs;
    Responder conn(&requestTs, &responseTs, kConnectionId);

    Response                       capturedResponse{};
    FACE::TSS::CONNECTION_ID_TYPE  capturedId  = -1;
    FACE::TSS::TRANSACTION_ID_TYPE capturedTid = -1;

    EXPECT_CALL(responseTs, Send_Message(_, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE id, FACE::TIMEOUT_TYPE,
                              FACE::TSS::TRANSACTION_ID_TYPE& tid, const Response& resp,
                              FACE::RETURN_CODE_TYPE& rc) {
            capturedId       = id;
            capturedTid      = tid;
            capturedResponse = resp;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    conn.registerHandler([](const Request& req, IResponseSender& sender) {
        sender.sendResponse(Response{req.value * 2});
    });

    DeliverRequest(conn, Request{21});

    EXPECT_EQ(capturedId, kConnectionId);
    EXPECT_EQ(capturedTid, kTransactionId);
    EXPECT_EQ(capturedResponse.value, 42);
}

TEST(ResponderConnectionTest, PredicateFiltersDeliveredRequests) {
    NiceMock<MockRequestTypedTS>  requestTs;
    NiceMock<MockResponseTypedTS> responseTs;
    Responder conn(&requestTs, &responseTs, kConnectionId);

    int callCount = 0;
    conn.registerHandler(
        [&](const Request&, IResponseSender&) { ++callCount; },
        [](const Request& r) { return r.value > 10; });

    DeliverRequest(conn, Request{5});
    EXPECT_EQ(callCount, 0);

    DeliverRequest(conn, Request{20});
    EXPECT_EQ(callCount, 1);
}

TEST(ResponderConnectionTest, AllHandlersInvokedOnDelivery) {
    NiceMock<MockRequestTypedTS>  requestTs;
    NiceMock<MockResponseTypedTS> responseTs;
    Responder conn(&requestTs, &responseTs, kConnectionId);

    int countA = 0;
    int countB = 0;
    conn.registerHandler([&](const Request&, IResponseSender&) { ++countA; });
    conn.registerHandler([&](const Request&, IResponseSender&) { ++countB; });

    DeliverRequest(conn, Request{1});

    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
}

TEST(ResponderConnectionTest, LastCancelUnregistersCallback) {
    NiceMock<MockRequestTypedTS>  requestTs;
    NiceMock<MockResponseTypedTS> responseTs;
    Responder conn(&requestTs, &responseTs, kConnectionId);

    EXPECT_CALL(requestTs, Unregister_Callback(kConnectionId, _)).Times(1);

    auto regA = conn.registerHandler([](const Request&, IResponseSender&) {});
    auto regB = conn.registerHandler([](const Request&, IResponseSender&) {});

    regA->Cancel();
    regB->Cancel();
}

TEST(ResponderConnectionTest, DestructorUnregistersWhenHandlersRemain) {
    NiceMock<MockRequestTypedTS>  requestTs;
    NiceMock<MockResponseTypedTS> responseTs;

    EXPECT_CALL(requestTs, Unregister_Callback(kConnectionId, _)).Times(1);
    {
        Responder conn(&requestTs, &responseTs, kConnectionId);
        conn.registerHandler([](const Request&, IResponseSender&) {});
    }
}
