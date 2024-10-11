/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Engine/Scene/EntityComponent.h>

namespace SE
{

EntityComponent::EntityComponent(const EntityComponentInitializer& initializer)
    : m_parent_entity(*initializer.parent_entity)
{}

} // namespace SE
