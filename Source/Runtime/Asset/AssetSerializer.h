/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Asset/Asset.h"

namespace SE
{

class AssetSerializer
{
public:
    virtual bool serialize(AssetHandle handle) = 0;
    virtual RefPtr<Asset> deserialize(AssetMetadata& metadata) = 0;
};

} // namespace SE
