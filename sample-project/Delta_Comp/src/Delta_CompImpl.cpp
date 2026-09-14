#include "Delta_CompImpl.h"
#include "DemoUtil.h"

namespace Delta_Comp {

void Delta_CompImpl::Initialize(FACE::RETURN_CODE_TYPE& return_code)
{
    Delta_CompBase::Initialize(return_code);
    if (return_code != FACE::RETURN_CODE_TYPE::NO_ERROR) {
        return;
    }

    auto table = GetEntityTSSConnectionTable();
    if (!table) {
        return;
    }

    FACE::DM::EntityReactorMessages_Templates::EntityCrud_Request crudRequest{};
    FACE::RETURN_CODE_TYPE crudRc = table->sendCRUDRequest(
        crudRequest,
        [](const FACE::DM::EntityReactorMessages_Templates::EntityCrud_Response&) {
            // Invoked asynchronously once a real TSS delivers a reply.
        });
    SampleProject::LogSend("Delta_Comp", Connections::CRUDREQUEST, crudRc);

    FACE::DM::EntityReactorMessages_Templates::Subscription_Request subRequest{};
    FACE::RETURN_CODE_TYPE subRc = table->sendSubRequest(
        subRequest,
        [](const FACE::DM::EntityReactorMessages_Templates::SubscriptionResponse&) {
            // Invoked asynchronously once a real TSS delivers a reply.
        });
    SampleProject::LogSend("Delta_Comp", Connections::SUBREQUEST, subRc);

    auto handle = table->registerEntityEventHandler(
        [](const FACE::DM::EntityReactorMessages_Templates::EntityEvent&) {
            // Invoked whenever a real TSS delivers an EntityEvent message.
        });
    SampleProject::LogRegister("Delta_Comp", Connections::ENTITYEVENT, handle);
}

} // namespace Delta_Comp
