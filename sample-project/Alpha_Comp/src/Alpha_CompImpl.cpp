#include "Alpha_CompImpl.h"
#include "DemoUtil.h"

namespace Alpha_Comp {

void Alpha_CompImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    // Base::Initialize() constructs the EntityTSS connection table (with no
    // live connections yet -- those are wired by Framework_Connect(), which
    // requires a real FACE::Configuration to be injected first).
    Alpha_CompBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    auto table = GetEntityTSSConnectionTable();
    if (!table) {
        return;
    }

    // Demonstrates both operations this UoP's connection table supports.
    // No real TSS is injected in this sample, so both report NOT_AVAILABLE
    // -- see DemoUtil.h and README.md.
    FACE::DM::CORE_Templates::Money money{};
    SampleProject::LogSend("Alpha_Comp", Connections::MONEYPUB, table->sendMoneyPub(money));

    FACE::DM::CustomerGateway_Templates::NewCustomerAgent_Request request{};
    FACE::RETURN_CODE_TYPE rc = table->sendNewCustomerAgentRequest(
        request,
        [](const FACE::DM::CustomerGateway_Templates::NewCustomerAgent_Response&) {
            // Invoked asynchronously once a real TSS delivers a reply.
        });
    SampleProject::LogSend("Alpha_Comp", Connections::NEWCUSTOMERAGENTREQUEST, rc);
}

} // namespace Alpha_Comp
