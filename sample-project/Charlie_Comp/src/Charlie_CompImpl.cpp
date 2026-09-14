#include "Charlie_CompImpl.h"
#include "DemoUtil.h"

namespace Charlie_Comp {

void Charlie_CompImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    Charlie_CompBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    if (auto table = GetMainTSSConnectionTable()) {
        auto handle = table->registerNewStockAgentResponseHandler(
            [](const FACE::DM::InventoryGateway_Templates::NewStockAgent_Request&,
               INewStockAgent_ResponseResponseSender& sender) {
                // Invoked when a real TSS delivers a request; reply via
                // sender.sendResponse(...) once the response is ready.
                FACE::DM::InventoryGateway_Templates::NewStockAgent_Response response{};
                sender.sendResponse(response);
            });
        SampleProject::LogRegister("Charlie_Comp", Connections::NEWSTOCKAGENTRESPONSE, handle);
    }
}

} // namespace Charlie_Comp
