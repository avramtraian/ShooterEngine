/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/HashMap.h>
#include <Core/Containers/String.h>
#include <Core/Containers/Vector.h>
#include <Core/Misc/IterationDecision.h>
#include <Core/UUID.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

using PFN_ComponentConstruct = void (*)(void*, const EntityComponentInitializer&);
using PFN_RegisterModuleComponents = void (*)(class ComponentRegistry&);

enum class ComponentFieldType : u16
{
    Unknown = 0,
    // clang-format off
    UInt8, UInt16, UInt32, UInt64,
    Int8, Int16, Int32, Int64,
    Float32, Float64,
    Boolean,
    Vector2, Vector3, Vector4,
    Color3, Color4,
    // clang-format on
};

using ComponentFieldFlags = u64;

enum class ComponentFieldFlag : u8
{
    Angle,
};

struct ComponentField
{
public:
    ComponentFieldType type { ComponentFieldType::Unknown };
    String name;
    u32 byte_offset { 0 };
    u32 byte_count { 0 };
    // NOTE: Should always be manipulated using the register builder.
    ComponentFieldFlags flags { 0 };
    u8 default_value_buffer[32];

public:
    NODISCARD ALWAYS_INLINE bool has_flag(ComponentFieldFlag flag) const
    {
        const u8 flag_bit_index = static_cast<u8>(flag);
        if (flags & (static_cast<u64>(1) << flag_bit_index))
            return true;
        return false;
    }
};

class ComponentRegisterBuilder
{
public:
    ALWAYS_INLINE void set_type_uuid(UUID type_uuid) { m_type_uuid = type_uuid; }
    ALWAYS_INLINE void set_structure_byte_count(usize structure_byte_count) { m_structure_byte_count = structure_byte_count; }
    ALWAYS_INLINE void set_parent_type_uuid(UUID parent_type_uuid) { m_parent_type_uuid = parent_type_uuid; }
    ALWAYS_INLINE void set_name(String name) { m_name = move(name); }
    ALWAYS_INLINE void set_construct_function(PFN_ComponentConstruct construct_function) { m_construct_function = construct_function; }

    // NOTE: The order in which the fields are added using this function is also the order in which they will
    // apear in the editor inspector panel.
    SHOOTER_API void add_field(ComponentField field);

    SHOOTER_API void push_field_flag(ComponentFieldFlag flag);
    SHOOTER_API void pop_field_flag(ComponentFieldFlag flag);

    NODISCARD SHOOTER_API bool has_field_flag(ComponentFieldFlag flag) const;
    SHOOTER_API void clear_field_flags();

public:
    NODISCARD ALWAYS_INLINE UUID get_type_uuid() const { return m_type_uuid; }
    NODISCARD ALWAYS_INLINE usize get_structure_byte_count() const { return m_structure_byte_count; }
    NODISCARD ALWAYS_INLINE UUID get_parent_type_uuid() const { return m_parent_type_uuid; }
    NODISCARD ALWAYS_INLINE const String& get_name() const { return m_name; }
    NODISCARD ALWAYS_INLINE PFN_ComponentConstruct get_construct_function() const { return m_construct_function; }
    NODISCARD ALWAYS_INLINE const Vector<ComponentField>& get_fields() const { return m_fields; }

private:
    UUID m_type_uuid;
    usize m_structure_byte_count { 0 };
    UUID m_parent_type_uuid;
    String m_name;
    PFN_ComponentConstruct m_construct_function { nullptr };
    Vector<ComponentField> m_fields;
    ComponentFieldFlags m_field_pushed_flags;
};

class ComponentRegistry
{
public:
    struct ComponentRegisterData
    {
        UUID type_uuid;
        usize structure_byte_count;
        UUID parent_uuid;
        Vector<UUID> children_uuids;
        String name;
        PFN_ComponentConstruct construct_function;
        Vector<ComponentField> fields;
    };

public:
    SHOOTER_API bool initialize();
    SHOOTER_API void shutdown();

    SHOOTER_API void construct_register(const ComponentRegisterBuilder& builder);

public:
    NODISCARD ALWAYS_INLINE const ComponentRegisterData& get_component_register(UUID component_type_uuid) const
    {
        Optional<const ComponentRegisterData&> register_data = m_component_registry.get_if_exists(component_type_uuid);
        SE_ASSERT(register_data.has_value());
        return register_data.value();
    }

    NODISCARD ALWAYS_INLINE const String& get_component_name(UUID component_type_uuid) const
    {
        const ComponentRegisterData& register_data = get_component_register(component_type_uuid);
        return register_data.name;
    }

    NODISCARD ALWAYS_INLINE PFN_ComponentConstruct get_component_construct_function(UUID component_type_uuid) const
    {
        const ComponentRegisterData& register_data = get_component_register(component_type_uuid);
        return register_data.construct_function;
    }

    NODISCARD ALWAYS_INLINE const Vector<ComponentField>& get_component_fields(UUID component_type_uuid) const
    {
        const ComponentRegisterData& register_data = get_component_register(component_type_uuid);
        return register_data.fields;
    }

public:
    template<typename PredicateFunction>
    ALWAYS_INLINE void for_each_component_register(PredicateFunction predicate) const
    {
        for (auto it : m_component_registry)
        {
            const IterationDecision iteration_decision = predicate(it.value);
            if (iteration_decision == IterationDecision::Break)
                break;
        }
    }

private:
    void construct_engine_components_registers();

private:
    HashMap<UUID, ComponentRegisterData> m_component_registry;
};

} // namespace SE
