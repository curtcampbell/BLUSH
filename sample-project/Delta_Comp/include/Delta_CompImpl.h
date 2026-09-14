#pragma once

// Delta_CompImpl.h
//
// Delta_Comp has one Integration Context (EntityTSS) exposing two
// CLIENT_SERVER requester connections (CRUDRequest, SubRequest) and one
// pub/sub subscriber connection (EntityEvent). This override demonstrates
// all three, since a single table makes that cheap to show in full.

#include "Delta_CompBase.h"

namespace Delta_Comp {

class Delta_CompImpl : public Delta_CompBase {
public:
    void Initialize(FACE::RETURN_CODE_TYPE& return_code) override;
};

} // namespace Delta_Comp
