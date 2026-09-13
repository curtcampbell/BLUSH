#include "RequesterConnection.h"
#include "support/FakeFaceModel.h"

#include <gtest/gtest.h>
#include <vector>

using FaceUtilsTest::MockReqRespTypedTS;
using FaceUtilsTest::ReqRespReadCallback;
using FaceUtilsTest::Request;
using FaceUtilsTest::Response;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace {
constexpr FACE::TSS::CONNECTION_ID_TYPE kConnectionId = 11;

FACE::TSS::HEADER_TYPE    kHeader{};
FACE::TSS::QoS_EVENT_TYPE kQos{};
} // namespace

TEST(RequesterConnectionTest, SendCallsSendMessageAsyncWithRequest) {
    NiceMock<MockReqRespTypedTS> ts;
    RequesterConnection<Request, Response> conn(&ts, kConnectionId);

    FACE::TSS::CONNECTION_ID_TYPE  capturedId = -1;
    FACE::TSS::TRANSACTION_ID_TYPE capturedTid = -1;
    Request                        capturedRequest{};

    EXPECT_CALL(ts, Send_Message_Async(_, _, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE id, FACE::TIMEOUT_TYPE,
                              FACE::TSS::TRANSACTION_ID_TYPE& tid, const Request& req,
                              ReqRespReadCallback**, FACE::RETURN_CODE_TYPE& rc) {
            capturedId      = id;
            capturedTid     = tid;
            capturedRequest = req;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    auto rc = conn.send(Request{7}, [](const Response&) {});

    EXPECT_EQ(rc, FACE::RETURN_CODE_TYPE::NO_ERROR);
    EXPECT_EQ(capturedId, kConnectionId);
    EXPECT_EQ(capturedTid, FACE::TSS::CALLEE_PROVIDES_TID);
    EXPECT_EQ(capturedRequest.value, 7);
}

TEST(RequesterConnectionTest, SendPropagatesFailureReturnCode) {
    NiceMock<MockReqRespTypedTS> ts;
    RequesterConnection<Request, Response> conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Send_Message_Async(_, _, _, _, _, _))
        .WillOnce(Invoke([](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
                             FACE::TSS::TRANSACTION_ID_TYPE&, const Request&,
                             ReqRespReadCallback**, FACE::RETURN_CODE_TYPE& rc) {
            rc = FACE::RETURN_CODE_TYPE::INVALID_PARAM;
        }));

    EXPECT_EQ(conn.send(Request{1}, [](const Response&) {}),
              FACE::RETURN_CODE_TYPE::INVALID_PARAM);
}

TEST(RequesterConnectionTest, ResponseArrivalInvokesUserHandler) {
    NiceMock<MockReqRespTypedTS> ts;
    RequesterConnection<Request, Response> conn(&ts, kConnectionId);

    ReqRespReadCallback* capturedCallback = nullptr;
    EXPECT_CALL(ts, Send_Message_Async(_, _, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
                              FACE::TSS::TRANSACTION_ID_TYPE&, const Request&,
                              ReqRespReadCallback** callback, FACE::RETURN_CODE_TYPE& rc) {
            capturedCallback = *callback;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    Response received{};
    int      callCount = 0;
    conn.send(Request{1}, [&](const Response& r) {
        received = r;
        ++callCount;
    });

    ASSERT_NE(capturedCallback, nullptr);

    FACE::RETURN_CODE_TYPE rc;
    capturedCallback->Callback_Handler(kConnectionId, 0, Response{99}, kHeader, kQos, rc);

    EXPECT_EQ(callCount, 1);
    EXPECT_EQ(received.value, 99);
}

TEST(RequesterConnectionTest, EachSendGetsItsOwnCallbackAdapter) {
    NiceMock<MockReqRespTypedTS> ts;
    RequesterConnection<Request, Response> conn(&ts, kConnectionId);

    std::vector<ReqRespReadCallback*> capturedCallbacks;
    EXPECT_CALL(ts, Send_Message_Async(_, _, _, _, _, _))
        .Times(2)
        .WillRepeatedly(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
                                    FACE::TSS::TRANSACTION_ID_TYPE& tid, const Request&,
                                    ReqRespReadCallback** callback, FACE::RETURN_CODE_TYPE& rc) {
            tid = static_cast<FACE::TSS::TRANSACTION_ID_TYPE>(capturedCallbacks.size() + 1);
            capturedCallbacks.push_back(*callback);
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    int countA = 0;
    int countB = 0;
    conn.send(Request{1}, [&](const Response&) { ++countA; });
    conn.send(Request{2}, [&](const Response&) { ++countB; });

    ASSERT_EQ(capturedCallbacks.size(), 2u);
    EXPECT_NE(capturedCallbacks[0], capturedCallbacks[1]);

    FACE::RETURN_CODE_TYPE rc;
    capturedCallbacks[0]->Callback_Handler(kConnectionId, 1, Response{}, kHeader, kQos, rc);
    capturedCallbacks[1]->Callback_Handler(kConnectionId, 2, Response{}, kHeader, kQos, rc);

    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
}
