/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/Vector.h>
#include <Core/Memory/Buffer.h>
#include <Core/String/String.h>
#include <Core/UUID.h>

namespace SE
{

// clang-format off
#define SE_FOR_EACH_COMPONENT_FIELD_TYPE(x) \
    x(Unknown)                              \
    x(UInt8)                                \
    x(UInt16)                               \
    x(UInt32)                               \
    x(UInt64)                               \
    x(Int8)                                 \
    x(Int16)                                \
    x(Int32)                                \
    x(Int64)                                \
    x(Float32)                              \
    x(Float64)                              \
    x(Boolean)                              \
    x(Vector2)                              \
    x(Vector3)                              \
    x(Vector4)                              \
    x(Color3)                               \
    x(Color4)                               \
    x(String)                               \
    x(AssetReferenceTexture)
// clang-format on

enum class ComponentFieldType : u16
{
#define DEFINE_ENUM_MEMBER(x) x,
    SE_FOR_EACH_COMPONENT_FIELD_TYPE(DEFINE_ENUM_MEMBER)
#undef DEFINE_ENUM_MEMBER
};

NODISCARD SHOOTER_API StringView get_component_field_type_as_string(ComponentFieldType field_type);
NODISCARD SHOOTER_API const char* get_component_field_type_as_raw_string(ComponentFieldType field_type);
NODISCARD SHOOTER_API ComponentFieldType get_component_field_type_from_string(StringView field_type_as_string);

// clang-format off
#define SE_FOR_EACH_COMPONENT_FIELD_FLAG(x) \
    x(None)                                 \
    x(DisplayInDegrees)
// clang-format on

enum class ComponentFieldFlag : u8
{
#define DEFINE_ENUM_MEMBER(x) x,
    SE_FOR_EACH_COMPONENT_FIELD_FLAG(DEFINE_ENUM_MEMBER)
#undef DEFINE_ENUM_MEMBER
};

NODISCARD SHOOTER_API StringView get_component_field_flag_as_string(ComponentFieldFlag field_flag);
NODISCARD SHOOTER_API const char* get_component_field_flag_as_raw_string(ComponentFieldFlag field_flag);
NODISCARD SHOOTER_API ComponentFieldFlag get_component_field_flag_from_string(StringView field_flag_as_string);

struct ComponentFieldMetadata
{
public:
    u64 flag_mask { 0 };

public:
    ALWAYS_INLINE void add_flag(ComponentFieldFlag flag)
    {
        SE_ASSERT(static_cast<u8>(flag) < sizeof(flag_mask) * 8);
        flag_mask |= static_cast<u64>(1) << static_cast<u8>(flag);
    }

    NODISCARD ALWAYS_INLINE bool has_flag(ComponentFieldFlag flag) const
    {
        SE_ASSERT(static_cast<u8>(flag) < sizeof(flag_mask) * 8);
        return (flag_mask & (static_cast<u64>(1) << static_cast<u8>(flag))) != 0;
    }
};

struct ComponentField
{
public:
    Vector<ComponentFieldType> type_stack;
    usize byte_offset { 0 };
    String name;
    ComponentFieldMetadata metadata;

public:
    template<typename FieldType>
    NODISCARD ALWAYS_INLINE FieldType& get_value(void* instance) const
    {
        return *reinterpret_cast<FieldType*>(static_cast<u8*>(instance) + byte_offset);
    }

    template<typename FieldType>
    NODISCARD ALWAYS_INLINE const FieldType& get_value(const void* instance) const
    {
        return *reinterpret_cast<const FieldType*>(static_cast<const u8*>(instance) + byte_offset);
    }
};

// Pointer to the function that will construct a component in the memory block
// provided by the first parameter.
using PFN_InstantiateComponent = void (*)(void*, const struct EntityComponentInitializer&);

struct ComponentReflector
{
public:
    UUID parent_type_uuid { UUID::invalid() };
    usize structure_byte_count { 0 };
    String name;
    PFN_InstantiateComponent instantiate_function { nullptr };
    Vector<ComponentField> fields;
    Buffer default_component_object_buffer;

public:
    template<typename ComponentType>
    NODISCARD ALWAYS_INLINE const ComponentType& get_default_component_object() const
    {
        SE_ASSERT(default_component_object_buffer.byte_count() == sizeof(ComponentType));
        return *reinterpret_cast<ComponentType*>(default_component_object_buffer.bytes());
    }

    template<typename FieldType>
    NODISCARD ALWAYS_INLINE const FieldType& get_default_field_value(u32 field_index) const
    {
        SE_ASSERT(field_index < fields.count());
        const ComponentField& field = fields[field_index];
        SE_ASSERT(field.byte_offset < default_component_object_buffer.byte_count());
        return *reinterpret_cast<const FieldType*>(default_component_object_buffer.bytes() + field.byte_offset);
    }
};

} // namespace SE
