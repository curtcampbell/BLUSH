#include "ReplenishmentServiceImpl.h"
#include "DemoUtil.h"

namespace ReplenishmentService {

void ReplenishmentServiceImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    ReplenishmentServiceBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    if (auto entityTable = GetEntityTSSConnectionTable()) {
        FACE::DM::InventoryGateway_Templates::Product_Entity msg{};
        SampleProject::LogSend("ReplenishmentService", Connections::PRODUCT_ENTITY_OUT,
                                entityTable->sendProduct_Entity_Out(msg));

        auto handle = entityTable->registerInventory_Event_InHandler(
            [](const FACE::DM::InventoryGateway_Templates::Inventory_Event&) {
                // Invoked whenever a real TSS delivers an Inventory_Event message.
            });
        SampleProject::LogRegister("ReplenishmentService", Connections::INVENTORY_EVENT_IN, handle);
    }

    if (auto mainTable = GetMainTSSConnectionTable()) {
        auto handle = mainTable->registerCheckoutSession_Event_InHandler(
            [](const FACE::DM::CheckoutGateway_Templates::CheckoutSession_Event&) {
                // Invoked whenever a real TSS delivers a CheckoutSession_Event message.
            });
        SampleProject::LogRegister("ReplenishmentService", Connections::CHECKOUTSESSION_EVENT_IN, handle);
    }

    if (auto thirdTable = GetThirdTSSConnectionTable()) {
        FACE::DM::InventoryGateway_Templates::Storage_Command msg{};
        SampleProject::LogSend("ReplenishmentService", Connections::STORAGE_COMMAND_OUT,
                                thirdTable->sendStorage_Command_Out(msg));

        auto handle = thirdTable->registerRS_ReceiptSubHandler(
            [](const FACE::DM::CheckoutGateway_Templates::Receipt&) {
                // Invoked whenever a real TSS delivers a Receipt message.
            });
        SampleProject::LogRegister("ReplenishmentService", Connections::RS_RECEIPTSUB, handle);
    }
}

} // namespace ReplenishmentService
