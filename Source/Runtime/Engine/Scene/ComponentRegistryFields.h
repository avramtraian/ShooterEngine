/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Core/Containers/StringView.h>
#include <Core/Math/Color.h>
#include <Core/Math/Vector.h>
#include <Engine/Scene/ComponentRegistry.h>

namespace SE
{

#define SE_TRY_ADD_COMPONENT_FIELD(field)                                                                                                                \
    {                                                                                                                                                    \
        const Optional<ComponentField> field_description = get_field_for_type(#field##sv, SE_OFFSET_OF(RemoveReference<decltype(*this)>, field), field); \
        if (field_description.has_value())                                                                                                               \
        {                                                                                                                                                \
            register_builder.add_field(field_description.value());                                                                                       \
        }                                                                                                                                                \
    }

template<typename FieldType>
ALWAYS_INLINE static Optional<ComponentField> get_field_for_type(StringView name, u32 field_byte_offset, const FieldType& default_value)
{
    // By default, return an empty optional.
    // This tells the caller that the engine doesn't know how to interpret the provided FieldType.
    return {};
}

#define SE_GET_FIELD_FOR_TYPE_DECLARATION(type_name, field_type)                                                                             \
    template<>                                                                                                                               \
    ALWAYS_INLINE static Optional<ComponentField> get_field_for_type(StringView name, u32 field_byte_offset, const type_name& default_value) \
    {                                                                                                                                        \
        ComponentField field = {};                                                                                                           \
        field.type = ComponentFieldType::field_type;                                                                                         \
        field.name = name;                                                                                                                   \
        field.byte_offset = field_byte_offset;                                                                                               \
        field.byte_count = sizeof(type_name);                                                                                                \
        copy_memory(field.default_value_buffer, &default_value, sizeof(type_name));                                                          \
        return field;                                                                                                                        \
    }

SE_GET_FIELD_FOR_TYPE_DECLARATION(u8, UInt8)
SE_GET_FIELD_FOR_TYPE_DECLARATION(u16, UInt16)
SE_GET_FIELD_FOR_TYPE_DECLARATION(u32, UInt32)
SE_GET_FIELD_FOR_TYPE_DECLARATION(u64, UInt64)

SE_GET_FIELD_FOR_TYPE_DECLARATION(i8, Int8)
SE_GET_FIELD_FOR_TYPE_DECLARATION(i16, Int16)
SE_GET_FIELD_FOR_TYPE_DECLARATION(i32, Int32)
SE_GET_FIELD_FOR_TYPE_DECLARATION(i64, Int64)

SE_GET_FIELD_FOR_TYPE_DECLARATION(float, Float32)
SE_GET_FIELD_FOR_TYPE_DECLARATION(double, Float64)

SE_GET_FIELD_FOR_TYPE_DECLARATION(Vector2, Vector2)
SE_GET_FIELD_FOR_TYPE_DECLARATION(Vector3, Vector3)
SE_GET_FIELD_FOR_TYPE_DECLARATION(Vector4, Vector4)

SE_GET_FIELD_FOR_TYPE_DECLARATION(Color3, Color3)
SE_GET_FIELD_FOR_TYPE_DECLARATION(Color4, Color4)

#undef SE_GET_FIELD_FOR_TYPE_DECLARATION

} // namespace SE
