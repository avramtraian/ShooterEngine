/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/CoreTypes.h>

namespace SE
{

struct D3D11RendererDevice
{
    void* device_handle;
    void* device_context_handle;
};

union RendererDevice
{
    D3D11RendererDevice d3d11;
};

} // namespace SE
