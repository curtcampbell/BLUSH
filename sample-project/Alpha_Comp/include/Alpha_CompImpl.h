#pragma once

// Alpha_CompImpl.h
//
// Example implementation of the Alpha_Comp UoP. Derives from the generated
// Alpha_CompBase, which already implements the full FACE LCM lifecycle
// (Configure/Initialize/Finalize/Framework_Connect/Framework_Disconnect)
// and the Injectable interfaces the executive uses to wire in a real TSS.
//
// This override adds no new lifecycle behavior of its own; it exists only
// to demonstrate calling the generated connection-table API
// (Alpha_CompEntityTSSConnectionTable) once the table exists. See
// Alpha_CompImpl.cpp.

#include "Alpha_CompBase.h"

namespace Alpha_Comp {

class Alpha_CompImpl : public Alpha_CompBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace Alpha_Comp
