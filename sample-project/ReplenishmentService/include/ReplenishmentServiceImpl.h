#pragma once

// ReplenishmentServiceImpl.h
//
// ReplenishmentService has three Integration Contexts (EntityTSS, MainTSS,
// ThirdTSS). This override demonstrates one representative call per
// table -- see the generated
// ConnectionTable/ReplenishmentService*ConnectionTable.h headers for the
// full connection set (EntityTSS alone exposes nine connections).

#include "ReplenishmentServiceBase.h"

namespace ReplenishmentService {

class ReplenishmentServiceImpl : public ReplenishmentServiceBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace ReplenishmentService
