#pragma once

// PointOfSaleTerminalImpl.h
//
// PointOfSaleTerminal has three Integration Contexts (EntityTSS, MainTSS,
// ThirdTSS). This override demonstrates one send and one register call per
// table -- see the generated
// ConnectionTable/PointOfSaleTerminal*ConnectionTable.h headers for the
// full connection set.

#include "PointOfSaleTerminalBase.h"

namespace PointOfSaleTerminal {

class PointOfSaleTerminalImpl : public PointOfSaleTerminalBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace PointOfSaleTerminal
