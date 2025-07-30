// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>

#include <map>
#include <string>

namespace SE
{

enum class ShaderReflectionDescriptorType : uint16
{
    Unknown = 0,
    Image,
    Sampler,
    CombinedImageSampler,
};

struct ShaderReflectionDescriptorBinding
{
    std::string Name;
    ShaderReflectionDescriptorType DescriptorType { ShaderReflectionDescriptorType::Unknown };
    uint32 ArrayCount { 0 };
};

struct ShaderReflectionDescriptorSet
{
    std::map<uint32, ShaderReflectionDescriptorBinding> Bindings;
    NODISCARD FORCEINLINE ShaderReflectionDescriptorBinding& operator[](uint32 bindingIndex) { return Bindings[bindingIndex]; }
};

struct ShaderReflectionData
{
    std::map<uint32, ShaderReflectionDescriptorSet> DescriptorSets;
};

}
