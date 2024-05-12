/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/RefPtr.h"

namespace SE
{

enum class ShaderStageType : u8
{
    Vertex,
    Fragment,
};

struct ShaderStage
{
    ShaderStage(ShaderStageType in_type, String in_filepath)
        : type(in_type)
        , filepath(move(in_filepath))
    {}

    ShaderStageType type;
    String          filepath;
};

struct ShaderInfo
{
    Vector<ShaderStage> stages;
    String              debug_name;
};

class Shader : public RefCounted
{
public:
    Shader() = default;
    virtual ~Shader() override = default;

    static RefPtr<Shader> create(const ShaderInfo& info);

public:
};

} // namespace SE