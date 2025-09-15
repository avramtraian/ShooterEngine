// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Scene/Reflection/Fields/FieldType.h>

namespace SE
{

struct Field
{
    FieldType* Type { nullptr };
    String Name;
    usize ByteOffset { 0 };
    usize ByteCount { 0 };
};

}
