#pragma once

// Beta_CompImpl.h
//
// Beta_Comp has two Integration Contexts (EntityTSS, MainTSS), so its
// generated base class exposes two connection-table getters. This
// override demonstrates one representative call on each: a register on
// EntityTSS, a send on MainTSS.

#include "Beta_CompBase.h"

namespace Beta_Comp {

class Beta_CompImpl : public Beta_CompBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace Beta_Comp
