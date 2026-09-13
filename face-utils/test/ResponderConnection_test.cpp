#include "ResponderConnection.h"
#include "support/FakeFaceModel.h"

#include <gtest/gtest.h>

using FaceUtilsTest::IResponseSender;
using FaceUtilsTest::MockReqRespTypedTS;
using FaceUtilsTest::Request;
using FaceUtilsTest::Response;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace {
constexpr FACE::TSS::CONNECTION_ID_TYPE  kConnectionId  = 21;
constexpr FACE::TSS::TRANSACTION_ID_TYPE kTransactionId = 5;

FACE::TSS::HEADER_TYPE    kHeader{};
FACE::TSS::QoS_EVENT_TYPE kQos{};

using Responder = ResponderConnection<Request, Response, IResponseSender>;

void DeliverRequest(Responder& conn, const Request& req) {
    FACE::RETURN_CODE_TYPE rc;
    conn.Callback_Handler(kConnectionId, kTransactionId, req, kHeader, kQos, rc);
}

} // namespace

TEST(ResponderConnectionTest, FirstRegisterHandlerRegistersCallbackOnce) {
    NiceMock<MockReqRespTypedTS> ts;
    Responder conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Register_Callback(kConnectionId, _, _)).Times(1);

    conn.registerHandler([](const Request&, IResponseSender&) {});
    conn.registerHandler([](const Request&, IResponseSender&) {});
}

TEST(ResponderConnectionTest, HandlerCanSendResponseThroughSender) {
    NiceMock<MockReqRespTypedTS> ts;
    Responder conn(&ts, kConnectionId);

    Response                       capturedResponse{};
    FACE::TSS::CONNECTION_ID_TYPE  capturedId  = -1;
    FACE::TSS::TRANSACTION_ID_TYPE capturedTid = -1;

    EXPECT_CALL(ts, Send_Message(_, _, _, _, _))
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
    NiceMock<MockReqRespTypedTS> ts;
    Responder conn(&ts, kConnectionId);

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
    NiceMock<MockReqRespTypedTS> ts;
    Responder conn(&ts, kConnectionId);

    int countA = 0;
    int countB = 0;
    conn.registerHandler([&](const Request&, IResponseSender&) { ++countA; });
    conn.registerHandler([&](const Request&, IResponseSender&) { ++countB; });

    DeliverRequest(conn, Request{1});

    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
}

TEST(ResponderConnectionTest, LastCancelUnregistersCallback) {
    NiceMock<MockReqRespTypedTS> ts;
    Responder conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Unregister_Callback(kConnectionId, _)).Times(1);

    auto regA = conn.registerHandler([](const Request&, IResponseSender&) {});
    auto regB = conn.registerHandler([](const Request&, IResponseSender&) {});

    regA->Cancel();
    regB->Cancel();
}

TEST(ResponderConnectionTest, DestructorUnregistersWhenHandlersRemain) {
    NiceMock<MockReqRespTypedTS> ts;

    EXPECT_CALL(ts, Unregister_Callback(kConnectionId, _)).Times(1);
    {
        Responder conn(&ts, kConnectionId);
        conn.registerHandler([](const Request&, IResponseSender&) {});
    }
}
