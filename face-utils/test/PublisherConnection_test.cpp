#include "PublisherConnection.h"
#include "support/FakeFaceModel.h"

#include <gtest/gtest.h>

using FaceUtilsTest::MockMsgTypedTS;
using FaceUtilsTest::Msg;
using ::testing::_;
using ::testing::Invoke;

namespace {
constexpr FACE::TSS::CONNECTION_ID_TYPE kConnectionId = 7;
}

TEST(PublisherConnectionTest, SendForwardsMessageAndReturnsReturnCode) {
    MockMsgTypedTS ts;
    PublisherConnection<Msg> conn(&ts, kConnectionId);

    FACE::TSS::CONNECTION_ID_TYPE   capturedId = -1;
    FACE::TIMEOUT_TYPE              capturedTimeout = 0;
    FACE::TSS::TRANSACTION_ID_TYPE  capturedTid = -1;
    Msg                             capturedMsg;

    EXPECT_CALL(ts, Send_Message(_, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE id, FACE::TIMEOUT_TYPE timeout,
                              FACE::TSS::TRANSACTION_ID_TYPE& tid, const Msg& msg,
                              FACE::RETURN_CODE_TYPE& rc) {
            capturedId      = id;
            capturedTimeout = timeout;
            capturedTid     = tid;
            capturedMsg     = msg;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    auto rc = conn.send(Msg{99});

    EXPECT_EQ(rc, FACE::RETURN_CODE_TYPE::NO_ERROR);
    EXPECT_EQ(capturedId, kConnectionId);
    EXPECT_EQ(capturedTimeout, FACE::INF_TIME_VALUE);
    EXPECT_EQ(capturedTid, FACE::TSS::TID_NOT_APPLICABLE);
    EXPECT_EQ(capturedMsg, Msg{99});
}

TEST(PublisherConnectionTest, SendForwardsCustomTimeout) {
    MockMsgTypedTS ts;
    PublisherConnection<Msg> conn(&ts, kConnectionId);

    FACE::TIMEOUT_TYPE capturedTimeout = 0;

    EXPECT_CALL(ts, Send_Message(_, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE timeout,
                              FACE::TSS::TRANSACTION_ID_TYPE&, const Msg&,
                              FACE::RETURN_CODE_TYPE& rc) {
            capturedTimeout = timeout;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    conn.send(Msg{1}, 12345);

    EXPECT_EQ(capturedTimeout, 12345);
}

TEST(PublisherConnectionTest, SendPropagatesFailureReturnCode) {
    MockMsgTypedTS ts;
    PublisherConnection<Msg> conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Send_Message(_, _, _, _, _))
        .WillOnce(Invoke([](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
                             FACE::TSS::TRANSACTION_ID_TYPE&, const Msg&,
                             FACE::RETURN_CODE_TYPE& rc) {
            rc = FACE::RETURN_CODE_TYPE::CONNECTION_CLOSED;
        }));

    EXPECT_EQ(conn.send(Msg{1}), FACE::RETURN_CODE_TYPE::CONNECTION_CLOSED);
}
