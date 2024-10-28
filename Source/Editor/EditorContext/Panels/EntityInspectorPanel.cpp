/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Color.h>
#include <Core/Math/Vector.h>
#include <Core/String/Format.h>
#include <EditorContext/Panels/EntityInspectorPanel.h>
#include <Engine/Scene/Entity.h>
#include <Engine/Scene/Reflection/ComponentReflectorRegistry.h>
#include <Engine/Scene/Scene.h>
#include <imgui.h>
#include <imgui_internal.h>

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

    if (has_entity_uuid_context() && has_scene_context() && has_component_reflector_registry_context())
    {
        Entity* entity_context = m_scene_context->get_entity_from_uuid(m_entity_uuid_context.value());
        SE_ASSERT(entity_context);

        draw_entity_name(*entity_context);
        draw_entity_uuid(entity_context->uuid());
        ImGui::Separator();

        draw_add_component(*entity_context);
        ImGui::Separator();

        for (EntityComponent* component : entity_context->get_components())
        {
            ImGui::PushID(reinterpret_cast<const void*>(component->parent_entity()->uuid().value()));
            draw_component(component);
            ImGui::PopID();
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

void EntityInspectorPanel::set_component_reflector_registry_context(ComponentReflectorRegistry* component_reflector_registry_context)
{
    SE_ASSERT(component_reflector_registry_context != nullptr);
    m_component_reflector_registry_context = component_reflector_registry_context;
    clear_entity_uuid_context();
}

void EntityInspectorPanel::clear_component_reflector_registry_context()
{
    m_component_reflector_registry_context = nullptr;
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
    Optional<String> uuid_string = format("{}"sv, uuid);
    SE_ASSERT(uuid_string.has_value() && uuid_string->byte_count() == 2 * sizeof(u64));

    char uuid_string_buffer[2 * sizeof(u64) + 1] = {};
    copy_memory_from_span(uuid_string_buffer, uuid_string->byte_span());

    // We display the UUID in an input text field so it can be easily copied to the clipbord.
    ImGui::InputText("Entity UUID", uuid_string_buffer, sizeof(uuid_string_buffer), ImGuiInputTextFlags_ReadOnly);
}

void EntityInspectorPanel::draw_add_component(Entity& entity_context)
{
    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    Optional<UUID> component_uuid;
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        ImGui::SeparatorText("Select a component");

        m_component_reflector_registry_context->for_each_reflector(
            [&](UUID type_uuid, const ComponentReflector& reflector)
            {
                if (!entity_context.has_component(type_uuid))
                {
                    if (ImGui::MenuItem(reflector.name.characters()))
                        component_uuid = type_uuid;
                }
                return IterationDecision::Continue;
            }
        );

        ImGui::EndPopup();
    }

    if (component_uuid.has_value())
    {
        const auto& reflector = m_component_reflector_registry_context->get_reflector(component_uuid.value());
        void* component_memory = ::operator new(reflector.structure_byte_count);

        EntityComponentInitializer initializer = {};
        initializer.parent_entity = &entity_context;
        initializer.scene_context = m_scene_context;
        reflector.instantiate_function(component_memory, initializer);

        EntityComponent* component = static_cast<EntityComponent*>(component_memory);
        entity_context.add_component(component);
    }
}

void EntityInspectorPanel::draw_component(EntityComponent* component)
{
    SE_ASSERT(component != nullptr);
    const ComponentReflector& reflector = m_component_reflector_registry_context->get_reflector(component->get_component_type_uuid());

    ImGui::PushID(reinterpret_cast<const void*>(component->get_component_type_uuid().value()));
    const ImVec2 content_region_available = ImGui::GetContentRegionAvail();

    const ImGuiTreeNodeFlags tree_node_flags =
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap;
    const bool is_tree_node_opened = ImGui::TreeNodeEx(reflector.name.characters(), tree_node_flags);

    ImVec2 button_size = ImGui::CalcTextSize("...");
    button_size.x += 2.0F * GImGui->Style.FramePadding.x;
    button_size.y += 2.0F * GImGui->Style.FramePadding.y;

    ImGui::SameLine(content_region_available.x - 0.5F * (button_size.x + GImGui->Style.FramePadding.x));
    if (ImGui::Button("...", button_size))
    {
        ImGui::OpenPopup("ComponentOptionsPopup");
    }

    if (ImGui::BeginPopup("ComponentOptionsPopup"))
    {
        ImGui::SeparatorText("Component Options");
        if (ImGui::MenuItem("Remove Component"))
        {
            // TODO: Actuall remove the component from the entity.
            // Currently, there is no API for removing components from an entity and that's
            // why this function is not implemented.
        }
        ImGui::EndPopup();
    }

    if (is_tree_node_opened)
    {
        for (const ComponentField& field : reflector.fields)
        {
            switch (field.type_stack.first())
            {
                case ComponentFieldType::Float32:
                {
                    float& field_value = field.get_value<float>(component);
                    float display_value = field_value;
                    if (field.metadata.has_flag(ComponentFieldFlag::DisplayInDegrees))
                        display_value *= Math::degrees(1.0F);

                    if (ImGui::DragFloat(field.name.characters(), &display_value))
                        display_value;

                    if (field.metadata.has_flag(ComponentFieldFlag::DisplayInDegrees))
                        display_value *= Math::radians(1.0F);
                    field_value = display_value;
                }
                break;

                case ComponentFieldType::Vector3:
                {
                    Vector3& field_value = field.get_value<Vector3>(component);
                    Vector3 display_value = field_value;
                    if (field.metadata.has_flag(ComponentFieldFlag::DisplayInDegrees))
                        display_value *= Math::degrees(1.0F);

                    if (ImGui::DragFloat3(field.name.characters(), display_value.value_ptr()))
                        display_value;

                    if (field.metadata.has_flag(ComponentFieldFlag::DisplayInDegrees))
                        display_value *= Math::radians(1.0F);
                    field_value = display_value;
                }
                break;

                case ComponentFieldType::Color4:
                {
                    Color4& field_value = field.get_value<Color4>(component);
                    ImGui::ColorEdit4(field.name.characters(), field_value.value_ptr());
                }
                break;
            }
        }

        ImGui::TreePop();
    }

    ImGui::Separator();
    ImGui::PopID();
}

} // namespace SE
