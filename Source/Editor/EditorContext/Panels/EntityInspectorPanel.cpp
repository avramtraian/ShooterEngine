/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Color.h>
#include <Core/Math/Vector3.h>
#include <EditorContext/Panels/EntityInspectorPanel.h>
#include <Engine/Scene/ComponentRegistry.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Scene.h>
#include <imgui.h>

namespace SE
{

bool EntityInspectorPanel::initialize()
{
    return true;
}

void EntityInspectorPanel::shutdown()
{}

void EntityInspectorPanel::on_update(float delta_time)
{}

void EntityInspectorPanel::on_render_imgui()
{
    ImGui::Begin("EntityInspector");

    if (has_entity_uuid_context() && has_scene_context() && has_component_registry_context())
    {
        Entity* entity_context = m_scene_context->get_entity_from_uuid(m_entity_uuid_context.value());
        SE_ASSERT(entity_context);

        draw_entity_name(*entity_context);
        draw_entity_uuid(entity_context->uuid());
        ImGui::Separator();

        for (EntityComponent* component : entity_context->get_components())
        {
            draw_component(component);
        }
    }

    ImGui::End();
}

void EntityInspectorPanel::set_entity_uuid_context(UUID entity_uuid_context)
{
    SE_ASSERT(entity_uuid_context != UUID::invalid());
    m_entity_uuid_context = entity_uuid_context;
}

void EntityInspectorPanel::clear_entity_uuid_context()
{
    m_entity_uuid_context.clear();
}

void EntityInspectorPanel::set_scene_context(Scene* scene_context)
{
    SE_ASSERT(scene_context != nullptr);
    m_scene_context = scene_context;
    clear_entity_uuid_context();
}

void EntityInspectorPanel::clear_scene_context()
{
    m_scene_context = nullptr;
    clear_entity_uuid_context();
}

void EntityInspectorPanel::set_component_registry_context(ComponentRegistry* component_registry_context)
{
    SE_ASSERT(component_registry_context != nullptr);
    m_component_registry_context = component_registry_context;
    clear_entity_uuid_context();
}

void EntityInspectorPanel::clear_component_registry_context()
{
    m_component_registry_context = nullptr;
    clear_entity_uuid_context();
}

void EntityInspectorPanel::draw_entity_name(Entity& entity_context)
{
    // NOTE: Entities names are thus capped to 256 bytes.
    char entity_name_buffer[256] = {};

    SE_ASSERT(entity_context.name().byte_span_with_null_termination().count() <= sizeof(entity_name_buffer));
    copy_memory_from_span(entity_name_buffer, entity_context.name().byte_span_with_null_termination());

    if (ImGui::InputText("Entity Name", entity_name_buffer, sizeof(entity_name_buffer)))
    {
        const StringView new_name = StringView::create_from_utf8(entity_name_buffer);
        entity_context.set_name(new_name);
    }
}

void EntityInspectorPanel::draw_entity_uuid(UUID uuid)
{
    // Because a UUID is only a 64-bit unsigned integer, we can be sure that it will
    // fit in a 16-long character array (written in hexadecimal).
    char entity_uuid_buffer[16 + 1] = {};
    set_memory(entity_uuid_buffer, '0', 16);

    u64 entity_uuid_value = uuid.value();
    usize buffer_offset = 0;
    while (entity_uuid_value > 0)
    {
        const u8 digit = entity_uuid_value % 16;
        entity_uuid_value /= 16;

        if (digit <= 9)
            entity_uuid_buffer[buffer_offset++] = '0' + digit;
        else
            entity_uuid_buffer[buffer_offset++] = 'A' + (digit - 10);
    }

    // Flip the modified characters.
    for (usize offset = 0; offset < buffer_offset / 2; ++offset)
    {
        const char temporary = entity_uuid_buffer[offset];
        entity_uuid_buffer[offset] = entity_uuid_buffer[buffer_offset - offset - 1];
        entity_uuid_buffer[buffer_offset - offset - 1] = temporary;
    }

    // We display the UUID in an input text field so it can be easily copied to the clipbord.
    ImGui::InputText("Entity UUID", entity_uuid_buffer, sizeof(entity_uuid_buffer), ImGuiInputTextFlags_ReadOnly);
}

void EntityInspectorPanel::draw_component(EntityComponent* component)
{
    SE_ASSERT(component != nullptr);
    const auto& component_register_data = m_component_registry_context->get_component_register(component->get_component_type_uuid());

    const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
    if (ImGui::TreeNodeEx(component_register_data.name.characters(), tree_node_flags))
    {
        for (const ComponentField& field : component_register_data.fields)
        {
            if (field.type == ComponentFieldType::Vector3)
            {
                SE_ASSERT(field.byte_count == sizeof(Vector3));
                Vector3& field_value = *reinterpret_cast<Vector3*>(reinterpret_cast<u8*>(component) + field.byte_offset);
                ImGui::DragFloat3(field.name.characters(), field_value.value_ptr(), 0.1F);
            }
            if (field.type == ComponentFieldType::Color4)
            {
                SE_ASSERT(field.byte_count == sizeof(Color4));
                Color4& field_value = *reinterpret_cast<Color4*>(reinterpret_cast<u8*>(component) + field.byte_offset);
                ImGui::ColorEdit4(field.name.characters(), &field_value.r);
            }
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
}

} // namespace SE
