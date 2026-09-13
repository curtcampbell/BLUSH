#include "SubscriberConnection.h"
#include "support/FakeFaceModel.h"

#include <gtest/gtest.h>

using FaceUtilsTest::MockMsgTypedTS;
using FaceUtilsTest::Msg;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace {
constexpr FACE::TSS::CONNECTION_ID_TYPE kConnectionId = 3;

FACE::TSS::HEADER_TYPE    kHeader{};
FACE::TSS::QoS_EVENT_TYPE kQos{};

void Deliver(SubscriberConnection<Msg>& conn, const Msg& msg) {
    FACE::RETURN_CODE_TYPE rc;
    conn.Callback_Handler(kConnectionId, 0, msg, kHeader, kQos, rc);
}

} // namespace

TEST(SubscriberConnectionTest, FirstRegisterHandlerRegistersCallbackOnce) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Register_Callback(kConnectionId, _, _))
        .Times(1)
        .WillOnce(Invoke([](FACE::TSS::CONNECTION_ID_TYPE, FaceUtilsTest::MsgReadCallback&,
                             FACE::RETURN_CODE_TYPE& rc) {
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    auto reg1 = conn.registerHandler([](const Msg&) {});
    auto reg2 = conn.registerHandler([](const Msg&) {});

    (void)reg1;
    (void)reg2;
}

TEST(SubscriberConnectionTest, DeliveredMessageInvokesHandler) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    Msg received{};
    int callCount = 0;
    conn.registerHandler([&](const Msg& m) {
        received = m;
        ++callCount;
    });

    Deliver(conn, Msg{55});

    EXPECT_EQ(callCount, 1);
    EXPECT_EQ(received, Msg{55});
}

TEST(SubscriberConnectionTest, PredicateFiltersDeliveredMessages) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    int callCount = 0;
    conn.registerHandler(
        [&](const Msg&) { ++callCount; },
        [](const Msg& m) { return m.value > 10; });

    Deliver(conn, Msg{5});
    EXPECT_EQ(callCount, 0);

    Deliver(conn, Msg{20});
    EXPECT_EQ(callCount, 1);
}

TEST(SubscriberConnectionTest, AllHandlersInvokedOnDelivery) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    int countA = 0;
    int countB = 0;
    conn.registerHandler([&](const Msg&) { ++countA; });
    conn.registerHandler([&](const Msg&) { ++countB; });

    Deliver(conn, Msg{1});

    EXPECT_EQ(countA, 1);
    EXPECT_EQ(countB, 1);
}

TEST(SubscriberConnectionTest, CancellingOneHandlerLeavesOthersActive) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    int countA = 0;
    int countB = 0;
    auto regA = conn.registerHandler([&](const Msg&) { ++countA; });
    conn.registerHandler([&](const Msg&) { ++countB; });

    regA->Cancel();
    Deliver(conn, Msg{1});

    EXPECT_EQ(countA, 0);
    EXPECT_EQ(countB, 1);
}

TEST(SubscriberConnectionTest, LastCancelUnregistersCallback) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Unregister_Callback(kConnectionId, _)).Times(1);

    auto regA = conn.registerHandler([](const Msg&) {});
    auto regB = conn.registerHandler([](const Msg&) {});

    regA->Cancel();
    regB->Cancel();
}

TEST(SubscriberConnectionTest, CancelIsIdempotentAndUnregistersOnce) {
    NiceMock<MockMsgTypedTS> ts;
    SubscriberConnection<Msg> conn(&ts, kConnectionId);

    EXPECT_CALL(ts, Unregister_Callback(kConnectionId, _)).Times(1);

    auto reg = conn.registerHandler([](const Msg&) {});
    reg->Cancel();
    reg->Cancel();
}

TEST(SubscriberConnectionTest, DestructorUnregistersWhenHandlersRemain) {
    NiceMock<MockMsgTypedTS> ts;

    EXPECT_CALL(ts, Unregister_Callback(kConnectionId, _)).Times(1);
    {
        SubscriberConnection<Msg> conn(&ts, kConnectionId);
        conn.registerHandler([](const Msg&) {});
    }
}

TEST(SubscriberConnectionTest, DestructorSkipsUnregisterWhenNoHandlersRemain) {
    NiceMock<MockMsgTypedTS> ts;

    EXPECT_CALL(ts, Unregister_Callback(_, _)).Times(0);
    {
        SubscriberConnection<Msg> conn(&ts, kConnectionId);
    }
}
