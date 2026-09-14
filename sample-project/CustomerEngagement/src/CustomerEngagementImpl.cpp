#include "CustomerEngagementImpl.h"
#include "DemoUtil.h"

namespace CustomerEngagement {

void CustomerEngagementImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    CustomerEngagementBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    if (auto entityTable = GetEntityTSSConnectionTable()) {
        FACE::DM::CustomerGateway_Templates::Membership_Response msg{};
        SampleProject::LogSend("CustomerEngagement", Connections::MEMBERSHIP_RESPONSE_OUT,
                                entityTable->sendMembership_Response_Out(msg));
    }

    if (auto mainTable = GetMainTSSConnectionTable()) {
        FACE::DM::CORE_Templates::Promotion msg{};
        SampleProject::LogSend("CustomerEngagement", Connections::PROMOTION_OUT,
                                mainTable->sendPromotion_Out(msg));
    }

    if (auto thirdTable = GetThirdTSSConnectionTable()) {
        auto handle = thirdTable->registerCE_ReceiptSubHandler(
            [](const FACE::DM::CheckoutGateway_Templates::Receipt&) {
                // Invoked whenever a real TSS delivers a Receipt message.
            });
        SampleProject::LogRegister("CustomerEngagement", Connections::CE_RECEIPTSUB, handle);
    }
}

} // namespace CustomerEngagement
