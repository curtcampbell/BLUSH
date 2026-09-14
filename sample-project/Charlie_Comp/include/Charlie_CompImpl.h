#pragma once

// Charlie_CompImpl.h
//
// Charlie_Comp holds the RESPONDER (server) role of the NewStockAgent
// CLIENT_SERVER connection (Beta_Comp is the REQUESTER/client). This
// override demonstrates registerNewStockAgentResponseHandler(), whose
// handler receives both the inbound request and a response-sender
// reference to reply through -- see ResponderConnection.h for how that
// dispatch is wired once a real TSS is injected.

#include "Charlie_CompBase.h"

namespace Charlie_Comp {

class Charlie_CompImpl : public Charlie_CompBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace Charlie_Comp
