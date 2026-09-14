#include "ConnectionResolver.h"
#include "support/FakeFaceModel.h"

#include <gtest/gtest.h>

using FaceUtilsTest::IResponseSender;
using FaceUtilsTest::IRspResponseSender;
using FaceUtilsTest::MockBase;
using FaceUtilsTest::MockMsgTypedTS;
using FaceUtilsTest::MockReqRespTypedTS;
using FaceUtilsTest::MockRspRequestTypedTS;
using FaceUtilsTest::MockRspResponseTypedTS;
using FaceUtilsTest::Msg;
using FaceUtilsTest::Request;
using FaceUtilsTest::Response;
using FaceUtilsTest::RspRequest;
using FaceUtilsTest::RspResponse;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

namespace {

// Inject() is protected on UopBase; expose it for the test harness.
class InjectableUopBase : public UopBase {
public:
    using UopBase::Inject;
};

// Configures a MockBase so Create_Connection always succeeds with `id`.
void ExpectCreateConnectionSucceeds(NiceMock<MockBase>& base,
                                    FACE::TSS::CONNECTION_ID_TYPE id) {
    ON_CALL(base, Create_Connection(_, _, _, _, _))
        .WillByDefault(Invoke([id](const FACE::TSS::CONNECTION_NAME_TYPE&, FACE::TIMEOUT_TYPE,
                                    FACE::TSS::CONNECTION_ID_TYPE& outId,
                                    FACE::TSS::MESSAGE_SIZE_TYPE&,
                                    FACE::RETURN_CODE_TYPE& rc) {
            outId = id;
            rc    = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));
}

} // namespace

// ---------------------------------------------------------------------------
// Name resolution: manual setters (uniform + per-connection)
// ---------------------------------------------------------------------------

TEST(ConnectionResolverTest, CreatePubConnectionSucceedsWithUniformManualNames) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockMsgTypedTS> ts;
    ExpectCreateConnectionSucceeds(base, 42);

    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    resolver.setUniformTypedTsName("ts");

    auto conn = resolver.CreatePubConnection<Msg>("myConn");

    ASSERT_NE(conn, nullptr);

    EXPECT_CALL(ts, Send_Message(_, _, _, _, _))
        .WillOnce(Invoke([](FACE::TSS::CONNECTION_ID_TYPE id, FACE::TIMEOUT_TYPE,
                             FACE::TSS::TRANSACTION_ID_TYPE&, const Msg&,
                             FACE::RETURN_CODE_TYPE& rc) {
            EXPECT_EQ(id, 42);
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));
    EXPECT_EQ(conn->send(Msg{1}), FACE::RETURN_CODE_TYPE::NO_ERROR);
}

TEST(ConnectionResolverTest, PerConnectionNameOverridesUniformName) {
    InjectableUopBase uop;
    NiceMock<MockBase> uniformBase;
    NiceMock<MockBase> specificBase;
    NiceMock<MockMsgTypedTS> ts;
    ExpectCreateConnectionSucceeds(uniformBase, 1);
    ExpectCreateConnectionSucceeds(specificBase, 2);

    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("uniformBase"), &uniformBase);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("specificBase"), &specificBase);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("uniformBase");
    resolver.setUniformTypedTsName("ts");
    resolver.setConnectionBaseName("myConn", "specificBase");

    EXPECT_CALL(specificBase, Create_Connection(_, _, _, _, _)).Times(1);
    EXPECT_CALL(uniformBase, Create_Connection(_, _, _, _, _)).Times(0);

    auto conn = resolver.CreatePubConnection<Msg>("myConn");
    EXPECT_NE(conn, nullptr);
}

TEST(ConnectionResolverTest, CreatePubConnectionFailsWhenBaseNameUnresolved) {
    InjectableUopBase uop;
    NiceMock<MockMsgTypedTS> ts;
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.setUniformTypedTsName("ts");
    // No base name configured at all.

    EXPECT_EQ(resolver.CreatePubConnection<Msg>("myConn"), nullptr);
}

TEST(ConnectionResolverTest, CreatePubConnectionFailsWhenTypedTsNotInjected) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    ExpectCreateConnectionSucceeds(base, 1);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    // "ts" name is configured but never injected.

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    resolver.setUniformTypedTsName("ts");

    EXPECT_EQ(resolver.CreatePubConnection<Msg>("myConn"), nullptr);
}

TEST(ConnectionResolverTest, CreatePubConnectionFailsWhenCreateConnectionErrors) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockMsgTypedTS> ts;
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ON_CALL(base, Create_Connection(_, _, _, _, _))
        .WillByDefault(Invoke([](const FACE::TSS::CONNECTION_NAME_TYPE&, FACE::TIMEOUT_TYPE,
                                  FACE::TSS::CONNECTION_ID_TYPE&, FACE::TSS::MESSAGE_SIZE_TYPE&,
                                  FACE::RETURN_CODE_TYPE& rc) {
            rc = FACE::RETURN_CODE_TYPE::RESOURCE_LIMIT_REACHED;
        }));

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    resolver.setUniformTypedTsName("ts");

    EXPECT_EQ(resolver.CreatePubConnection<Msg>("myConn"), nullptr);
}

// ---------------------------------------------------------------------------
// Initialize() -- XML configuration
// ---------------------------------------------------------------------------

TEST(ConnectionResolverTest, InitializeUniformModeParsesAllConnectionsElement) {
    static const char* kXml = R"(
        <uop-connection-config>
            <connection-table uop_name="myUop">
                <all-connections base="xmlBase" typed_ts="xmlTs"/>
            </connection-table>
        </uop-connection-config>
    )";

    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockMsgTypedTS> ts;
    ExpectCreateConnectionSucceeds(base, 1);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("xmlBase"), &base);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("xmlTs"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.Initialize(kXml, "myUop");

    EXPECT_NE(resolver.CreatePubConnection<Msg>("anyConnectionName"), nullptr);
}

TEST(ConnectionResolverTest, InitializePerConnectionModeParsesConnectionList) {
    static const char* kXml = R"(
        <uop-connection-config>
            <connection-table uop_name="myUop">
                <connection_list>
                    <connection name="connA" base="baseA" typed_ts="tsA"/>
                    <connection name="connB" base="baseB" typed_ts="tsB"/>
                </connection_list>
            </connection-table>
        </uop-connection-config>
    )";

    InjectableUopBase uop;
    NiceMock<MockBase> baseA, baseB;
    NiceMock<MockMsgTypedTS> tsA, tsB;
    ExpectCreateConnectionSucceeds(baseA, 1);
    ExpectCreateConnectionSucceeds(baseB, 2);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("baseA"), &baseA);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("baseB"), &baseB);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("tsA"), &tsA);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("tsB"), &tsB);

    ConnectionResolver resolver(&uop);
    resolver.Initialize(kXml, "myUop");

    EXPECT_CALL(baseA, Create_Connection(_, _, _, _, _)).Times(1);
    EXPECT_CALL(baseB, Create_Connection(_, _, _, _, _)).Times(1);

    EXPECT_NE(resolver.CreatePubConnection<Msg>("connA"), nullptr);
    EXPECT_NE(resolver.CreatePubConnection<Msg>("connB"), nullptr);
}

TEST(ConnectionResolverTest, ManualSetterAfterInitializeOverridesXmlValue) {
    static const char* kXml = R"(
        <uop-connection-config>
            <connection-table uop_name="myUop">
                <connection_list>
                    <connection name="connA" base="xmlBase" typed_ts="xmlTs"/>
                </connection_list>
            </connection-table>
        </uop-connection-config>
    )";

    InjectableUopBase uop;
    NiceMock<MockBase> overrideBase;
    NiceMock<MockMsgTypedTS> ts;
    ExpectCreateConnectionSucceeds(overrideBase, 1);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("overrideBase"), &overrideBase);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("xmlTs"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.Initialize(kXml, "myUop");
    resolver.setConnectionBaseName("connA", "overrideBase");

    EXPECT_CALL(overrideBase, Create_Connection(_, _, _, _, _)).Times(1);
    EXPECT_NE(resolver.CreatePubConnection<Msg>("connA"), nullptr);
}

TEST(ConnectionResolverTest, InitializeIgnoresNonMatchingUopName) {
    static const char* kXml = R"(
        <uop-connection-config>
            <connection-table uop_name="otherUop">
                <all-connections base="xmlBase" typed_ts="xmlTs"/>
            </connection-table>
        </uop-connection-config>
    )";

    InjectableUopBase uop;
    ConnectionResolver resolver(&uop);
    resolver.Initialize(kXml, "myUop");

    EXPECT_EQ(resolver.CreatePubConnection<Msg>("anyName"), nullptr);
}

// ---------------------------------------------------------------------------
// CreateSubConnection
// ---------------------------------------------------------------------------

TEST(ConnectionResolverTest, CreateSubConnectionSucceedsAndHandlerReceivesMessages) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockMsgTypedTS> ts;
    ExpectCreateConnectionSucceeds(base, 3);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    uop.Inject<FaceUtilsTest::MsgTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    resolver.setUniformTypedTsName("ts");

    EXPECT_CALL(ts, Register_Callback(3, _, _)).Times(1);

    auto conn = resolver.CreateSubConnection<Msg>("myConn");
    ASSERT_NE(conn, nullptr);

    int callCount = 0;
    conn->registerHandler([&](const Msg&) { ++callCount; });

    FACE::TSS::HEADER_TYPE    header{};
    FACE::TSS::QoS_EVENT_TYPE qos{};
    FACE::RETURN_CODE_TYPE    rc;
    conn->Callback_Handler(3, 0, Msg{1}, header, qos, rc);

    EXPECT_EQ(callCount, 1);
}

// ---------------------------------------------------------------------------
// CreateReqConnection / CreateRspConnection (CLIENT_SERVER)
// ---------------------------------------------------------------------------

TEST(ConnectionResolverTest, CreateReqConnectionSucceedsEndToEnd) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockReqRespTypedTS> ts;
    ExpectCreateConnectionSucceeds(base, 4);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    uop.Inject<FaceUtilsTest::ReqRespTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    resolver.setUniformTypedTsName("ts");

    FaceUtilsTest::ReqRespReadCallback* capturedCallback = nullptr;
    EXPECT_CALL(ts, Send_Message_Async(_, _, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
                              FACE::TSS::TRANSACTION_ID_TYPE&, const Request&,
                              FaceUtilsTest::ReqRespReadCallback** cb,
                              FACE::RETURN_CODE_TYPE& rc) {
            capturedCallback = *cb;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    auto conn = resolver.CreateReqConnection<Request, Response>("myConn");
    ASSERT_NE(conn, nullptr);

    int callCount = 0;
    conn->send(Request{1}, [&](const Response&) { ++callCount; });

    ASSERT_NE(capturedCallback, nullptr);
    FACE::TSS::HEADER_TYPE    header{};
    FACE::TSS::QoS_EVENT_TYPE qos{};
    FACE::RETURN_CODE_TYPE    rc;
    capturedCallback->Callback_Handler(4, 0, Response{}, header, qos, rc);

    EXPECT_EQ(callCount, 1);
}

TEST(ConnectionResolverTest, CreateReqConnectionFailsWhenBaseMissing) {
    InjectableUopBase uop;
    NiceMock<MockReqRespTypedTS> ts;
    uop.Inject<FaceUtilsTest::ReqRespTypedTS*>(FACE::STRING_TYPE("ts"), &ts);

    ConnectionResolver resolver(&uop);
    resolver.setUniformTypedTsName("ts");

    EXPECT_EQ((resolver.CreateReqConnection<Request, Response>("myConn")), nullptr);
}

TEST(ConnectionResolverTest, CreateRspConnectionSucceedsEndToEnd) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockRspRequestTypedTS>  requestTs;
    NiceMock<MockRspResponseTypedTS> responseTs;
    ExpectCreateConnectionSucceeds(base, 6);
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    uop.Inject<FaceUtilsTest::RspRequestTypedTS*>(FACE::STRING_TYPE("reqTs"), &requestTs);
    uop.Inject<FaceUtilsTest::RspResponseTypedTS*>(FACE::STRING_TYPE("respTs"), &responseTs);

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    // No uniform-mode fallback for the response name (see
    // resolveTypedTsResponseName); per-connection setters for both.
    resolver.setConnectionTypedTsName("myConn", "reqTs");
    resolver.setConnectionTypedTsResponseName("myConn", "respTs");

    EXPECT_CALL(requestTs, Register_Callback(6, _, _)).Times(1);

    auto conn = resolver.CreateRspConnection<RspRequest, RspResponse, IRspResponseSender>("myConn");
    ASSERT_NE(conn, nullptr);

    RspResponse capturedResponse{};
    EXPECT_CALL(responseTs, Send_Message(6, _, _, _, _))
        .WillOnce(Invoke([&](FACE::TSS::CONNECTION_ID_TYPE, FACE::TIMEOUT_TYPE,
                              FACE::TSS::TRANSACTION_ID_TYPE&, const RspResponse& resp,
                              FACE::RETURN_CODE_TYPE& rc) {
            capturedResponse = resp;
            rc = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }));

    conn->registerHandler([](const RspRequest& req, IRspResponseSender& sender) {
        sender.sendResponse(RspResponse{req.value + 1});
    });

    FACE::TSS::HEADER_TYPE    header{};
    FACE::TSS::QoS_EVENT_TYPE qos{};
    FACE::RETURN_CODE_TYPE    rc;
    conn->Callback_Handler(6, 9, RspRequest{41}, header, qos, rc);

    EXPECT_EQ(capturedResponse.value, 42);
}

TEST(ConnectionResolverTest, CreateRspConnectionFailsWhenCreateConnectionErrors) {
    InjectableUopBase uop;
    NiceMock<MockBase> base;
    NiceMock<MockRspRequestTypedTS>  requestTs;
    NiceMock<MockRspResponseTypedTS> responseTs;
    uop.Inject<FACE::TSS::Base*>(FACE::STRING_TYPE("base"), &base);
    uop.Inject<FaceUtilsTest::RspRequestTypedTS*>(FACE::STRING_TYPE("reqTs"), &requestTs);
    uop.Inject<FaceUtilsTest::RspResponseTypedTS*>(FACE::STRING_TYPE("respTs"), &responseTs);

    ON_CALL(base, Create_Connection(_, _, _, _, _))
        .WillByDefault(Invoke([](const FACE::TSS::CONNECTION_NAME_TYPE&, FACE::TIMEOUT_TYPE,
                                  FACE::TSS::CONNECTION_ID_TYPE&, FACE::TSS::MESSAGE_SIZE_TYPE&,
                                  FACE::RETURN_CODE_TYPE& rc) {
            rc = FACE::RETURN_CODE_TYPE::RESOURCE_LIMIT_REACHED;
        }));

    ConnectionResolver resolver(&uop);
    resolver.setUniformBaseName("base");
    resolver.setConnectionTypedTsName("myConn", "reqTs");
    resolver.setConnectionTypedTsResponseName("myConn", "respTs");

    EXPECT_EQ((resolver.CreateRspConnection<RspRequest, RspResponse, IRspResponseSender>("myConn")),
              nullptr);
}
