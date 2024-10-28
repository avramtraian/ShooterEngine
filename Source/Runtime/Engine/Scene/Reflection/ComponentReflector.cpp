/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Engine/Scene/Reflection/ComponentReflector.h>

namespace SE
{

StringView get_component_field_type_as_string(ComponentFieldType field_type)
{
    switch (field_type)
    {
        // clang-format off
#define FIELD_TYPE_TO_STRING(x) case ComponentFieldType::x: return #x##sv;
        SE_FOR_EACH_COMPONENT_FIELD_TYPE(FIELD_TYPE_TO_STRING)
#undef FIELD_TYPE_TO_STRING
        // clang-format on
    }

    SE_ASSERT("Invalid ComponentFieldType!");
    return {};
}

const char* get_component_field_type_as_raw_string(ComponentFieldType field_type)
{
    switch (field_type)
    {
        // clang-format off
#define FIELD_TYPE_TO_STRING(x) case ComponentFieldType::x: return #x;
        SE_FOR_EACH_COMPONENT_FIELD_TYPE(FIELD_TYPE_TO_STRING)
#undef FIELD_TYPE_TO_STRING
        // clang-format on
    }

    SE_ASSERT("Invalid ComponentFieldType!");
    return nullptr;
}

ComponentFieldType get_component_field_type_from_string(StringView field_type_as_string)
{
    // clang-format off
#define FIELD_TYPE_FROM_STRING(x) if (field_type_as_string == #x##sv) return ComponentFieldType::x;
    SE_FOR_EACH_COMPONENT_FIELD_TYPE(FIELD_TYPE_FROM_STRING)
#undef FIELD_TYPE_FROM_STRING
    // clang-format on

    SE_LOG_ERROR("Invalid string for getting a ComponentFieldType: '{}'!", field_type_as_string);
    return ComponentFieldType::Unknown;
}

StringView get_component_field_flag_as_string(ComponentFieldFlag field_flag)
{
    switch (field_flag)
    {
        // clang-format off
#define FIELD_FLAG_TO_STRING(x) case ComponentFieldFlag::x: return #x##sv;
        SE_FOR_EACH_COMPONENT_FIELD_FLAG(FIELD_FLAG_TO_STRING)
#undef FIELD_FLAG_TO_STRING
        // clang-format on
    }

    SE_ASSERT("Invalid ComponentFieldFlag!");
    return {};
}

const char* get_component_field_flag_as_raw_string(ComponentFieldFlag field_flag)
{
    switch (field_flag)
    {
        // clang-format off
#define FIELD_FLAG_TO_STRING(x) case ComponentFieldFlag::x: return #x;
        SE_FOR_EACH_COMPONENT_FIELD_FLAG(FIELD_FLAG_TO_STRING)
#undef FIELD_FLAG_TO_STRING
        // clang-format on
    }

    SE_ASSERT("Invalid ComponentFieldFlag!");
    return nullptr;
}

ComponentFieldFlag get_component_field_flag_from_string(StringView field_flag_as_string)
{
    // clang-format off
#define FIELD_FLAG_FROM_STRING(x) if (field_flag_as_string == #x##sv) return ComponentFieldFlag::x;
    SE_FOR_EACH_COMPONENT_FIELD_FLAG(FIELD_FLAG_FROM_STRING)
#undef FIELD_FLAG_FROM_STRING
    // clang-format on

    SE_LOG_ERROR("Invalid string for getting a ComponentFieldFlag: '{}'!", field_flag_as_string);
    return ComponentFieldFlag::None;
}

} // namespace SE
