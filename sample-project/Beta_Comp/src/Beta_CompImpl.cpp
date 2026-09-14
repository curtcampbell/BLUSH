#include "Beta_CompImpl.h"
#include "DemoUtil.h"

namespace Beta_Comp {

void Beta_CompImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    Beta_CompBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    if (auto entityTable = GetEntityTSSConnectionTable()) {
        auto handle = entityTable->registerMoneySubHandler(
            [](const FACE::DM::CORE_Templates::Money&) {
                // Invoked whenever a real TSS delivers a Money message.
            });
        SampleProject::LogRegister("Beta_Comp", Connections::MONEYSUB, handle);
    }

    if (auto mainTable = GetMainTSSConnectionTable()) {
        FACE::DM::InventoryGateway_Templates::NewStockAgent_Request request{};
        FACE::RETURN_CODE_TYPE rc = mainTable->sendNewStockAgentRequest(
            request,
            [](const FACE::DM::InventoryGateway_Templates::NewStockAgent_Response&) {
                // Invoked asynchronously once a real TSS delivers a reply.
            });
        SampleProject::LogSend("Beta_Comp", Connections::NEWSTOCKAGENTREQUEST, rc);
    }
}

} // namespace Beta_Comp
