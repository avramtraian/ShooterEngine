/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Math/Vector3.h>
#include <Engine/Scene/EntityComponent.h>

namespace SE
{

class TransformComponent : public EntityComponent
{
    SE_ENGINE_ENTITY_COMPONENT(TransformComponent, EntityComponent);

public:
    SHOOTER_API TransformComponent(const EntityComponentInitializer&, Vector3 in_translation, Vector3 in_rotation, Vector3 in_scale);

public:
    NODISCARD ALWAYS_INLINE Vector3 translation() const { return m_translation; }
    NODISCARD ALWAYS_INLINE Vector3 rotation() const { return m_rotation; }
    NODISCARD ALWAYS_INLINE Vector3 scale() const { return m_scale; }

    ALWAYS_INLINE void set_translation(Vector3 new_translation) { m_translation = new_translation; }
    ALWAYS_INLINE void set_rotation(Vector3 in_rotation) { m_rotation = in_rotation; }
    ALWAYS_INLINE void set_scale(Vector3 in_scale) { m_scale = in_scale; }

private:
    Vector3 m_translation { 0, 0, 0 };
    Vector3 m_rotation { 0, 0, 0 };
    Vector3 m_scale { 1, 1, 1 };
};

} // namespace SE
