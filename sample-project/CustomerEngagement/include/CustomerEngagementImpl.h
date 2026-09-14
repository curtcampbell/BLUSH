#pragma once

// CustomerEngagementImpl.h
//
// CustomerEngagement has three Integration Contexts (EntityTSS, MainTSS,
// ThirdTSS). This override demonstrates one representative call per table
// rather than every connection each table exposes -- see the generated
// ConnectionTable/CustomerEngagement*ConnectionTable.h headers for the
// full set.

#include "CustomerEngagementBase.h"

namespace CustomerEngagement {

class CustomerEngagementImpl : public CustomerEngagementBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace CustomerEngagement
