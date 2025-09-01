// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

enum class ShaderReflectionDescriptorType : uint16
{
    Unknown = 0,
    Image,
    Sampler,
    CombinedImageSampler,
    UniformBuffer,
};

struct ShaderReflectionDescriptorBinding
{
    String Name;
    ShaderReflectionDescriptorType DescriptorType { ShaderReflectionDescriptorType::Unknown };
    uint32 ArrayCount { 0 };
};

struct ShaderReflectionDescriptorSet
{
    HashMap<uint32, ShaderReflectionDescriptorBinding> Bindings;
    NODISCARD FORCEINLINE ShaderReflectionDescriptorBinding& operator[](uint32 bindingIndex) { return Bindings[bindingIndex]; }
};

struct ShaderReflectionData
{
    HashMap<uint32, ShaderReflectionDescriptorSet> DescriptorSets;
};

}
