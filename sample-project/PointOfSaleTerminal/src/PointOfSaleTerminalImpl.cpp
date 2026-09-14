#include "PointOfSaleTerminalImpl.h"
#include "DemoUtil.h"

namespace PointOfSaleTerminal {

void PointOfSaleTerminalImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    PointOfSaleTerminalBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    if (auto entityTable = GetEntityTSSConnectionTable()) {
        FACE::DM::CheckoutGateway_Templates::PaymentAuthorized msg{};
        SampleProject::LogSend("PointOfSaleTerminal", Connections::PAYMENTAUTHORIZED_OUT,
                                entityTable->sendPaymentAuthorized_Out(msg));

        auto handle = entityTable->registerProduct_Entity_InHandler(
            [](const FACE::DM::InventoryGateway_Templates::Product_Entity&) {
                // Invoked whenever a real TSS delivers a Product_Entity message.
            });
        SampleProject::LogRegister("PointOfSaleTerminal", Connections::PRODUCT_ENTITY_IN, handle);
    }

    if (auto mainTable = GetMainTSSConnectionTable()) {
        auto handle = mainTable->registerPromotion_InHandler(
            [](const FACE::DM::CORE_Templates::Promotion&) {
                // Invoked whenever a real TSS delivers a Promotion message.
            });
        SampleProject::LogRegister("PointOfSaleTerminal", Connections::PROMOTION_IN, handle);

        FACE::DM::CheckoutGateway_Templates::CheckoutSession_Event msg{};
        SampleProject::LogSend("PointOfSaleTerminal", Connections::CHECKOUTSESSION_EVENT_OUT,
                                mainTable->sendCheckoutSession_Event_Out(msg));
    }

    if (auto thirdTable = GetThirdTSSConnectionTable()) {
        FACE::DM::CheckoutGateway_Templates::Receipt msg{};
        SampleProject::LogSend("PointOfSaleTerminal", Connections::RECEIPTPUB,
                                thirdTable->sendReceiptPub(msg));

        auto handle = thirdTable->registerPriceChange_InHandler(
            [](const FACE::DM::CORE_Templates::PriceChange&) {
                // Invoked whenever a real TSS delivers a PriceChange message.
            });
        SampleProject::LogRegister("PointOfSaleTerminal", Connections::PRICECHANGE_IN, handle);
    }
}

} // namespace PointOfSaleTerminal
