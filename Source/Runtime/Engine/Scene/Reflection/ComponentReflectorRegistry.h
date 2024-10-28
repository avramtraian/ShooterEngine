/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/HashMap.h>
#include <Core/Misc/IterationDecision.h>
#include <Engine/Scene/Reflection/ComponentReflector.h>

namespace SE
{

class ComponentReflectorRegistry
{
public:
    SHOOTER_API bool initialize();
    SHOOTER_API void shutdown();

    SHOOTER_API ComponentReflector& allocate_reflector(UUID component_type_uuid);
    SHOOTER_API const ComponentReflector& get_reflector(UUID component_type_uuid) const;

    template<typename PredicateFunction>
    ALWAYS_INLINE void for_each_reflector(PredicateFunction predicate_function) const
    {
        for (auto it : m_registry)
        {
            const IterationDecision iteration_decision = predicate_function(it.key, it.value);
            if (iteration_decision == IterationDecision::Break)
                break;
        }
    }

private:
    HashMap<UUID, ComponentReflector> m_registry;
};

} // namespace SE
