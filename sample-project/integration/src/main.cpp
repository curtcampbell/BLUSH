// integration/src/main.cpp
//
// Constructs one instance of all seven UoPs in GROCERY_with_IM.face, drives
// each through the full FACE LCM sequence (Configure -> Initialize ->
// Framework_Connect -> Framework_Disconnect -> Finalize), and exits.
//
// Charlie_Comp and Beta_Comp are the responder and requester sides of the
// same CLIENT_SERVER connection (NewStockAgentRequest/Response); they used
// to be unable to link into one binary together -- see "Why Charlie_Comp is
// back in the integration binary" in README.md and
// session-docs/TRAITS-ROLE-CONFLICT.md for how the Traits<T, Role> refactor
// fixed that.
//
// No real TSS or FACE::Configuration service is injected anywhere in this
// program -- see README.md for exactly what that means for the output
// below. In short: Initialize() always succeeds (it only constructs local
// connection-table objects), while Framework_Connect() always reports
// NOT_AVAILABLE (it needs an injected FACE::Configuration to read
// uop-connections.xml and wire live connections). Both are the correct,
// documented behavior of a UoP that hasn't been wired into a real
// executive yet, not a bug in this demo.

#include "Alpha_CompImpl.h"
#include "Beta_CompImpl.h"
#include "Charlie_CompImpl.h"
#include "CustomerEngagementImpl.h"
#include "Delta_CompImpl.h"
#include "PointOfSaleTerminalImpl.h"
#include "ReplenishmentServiceImpl.h"

#include "DemoUtil.h"

#include <cstdio>

namespace {

template <typename UoPImpl>
void RunLifecycle(const char* uopName)
{
    std::printf("=== %s ===\n", uopName);

    UoPImpl uop;
    FACE::RETURN_CODE_TYPE rc = FACE::RETURN_CODE_TYPE::NO_ERROR;

    uop.Configure(FACE::CONFIGURATION_RESOURCE(), rc);
    std::printf("  Configure            -> %s\n", SampleProject::RcName(rc));

    // Initialize() constructs each Integration Context's connection table
    // (with no live connections yet) and, in this sample's override, calls
    // one representative send___()/register___Handler() on each table --
    // see <UoPName>Impl.cpp for exactly which ones.
    uop.Initialize(rc);
    std::printf("  Initialize           -> %s\n", SampleProject::RcName(rc));

    uop.Framework_Connect(FACE::CONFIGURATION_RESOURCE(), rc);
    std::printf("  Framework_Connect    -> %s (expected: no FACE::Configuration injected)\n",
                SampleProject::RcName(rc));

    uop.Framework_Disconnect(rc);
    std::printf("  Framework_Disconnect -> %s\n", SampleProject::RcName(rc));

    uop.Finalize(rc);
    std::printf("  Finalize             -> %s\n\n", SampleProject::RcName(rc));
}

} // namespace

int main()
{
    RunLifecycle<Alpha_Comp::Alpha_CompImpl>("Alpha_Comp");
    RunLifecycle<Beta_Comp::Beta_CompImpl>("Beta_Comp");
    RunLifecycle<Charlie_Comp::Charlie_CompImpl>("Charlie_Comp");
    RunLifecycle<CustomerEngagement::CustomerEngagementImpl>("CustomerEngagement");
    RunLifecycle<Delta_Comp::Delta_CompImpl>("Delta_Comp");
    RunLifecycle<PointOfSaleTerminal::PointOfSaleTerminalImpl>("PointOfSaleTerminal");
    RunLifecycle<ReplenishmentService::ReplenishmentServiceImpl>("ReplenishmentService");

    return 0;
}
