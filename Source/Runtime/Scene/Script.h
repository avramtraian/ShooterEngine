// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/UUID.h>
#include <Runtime/Scene/Reflection/ReflectionScript.h>

namespace SE
{

class Script
{
    SE_MAKE_NONCOPYABLE(Script);
    SE_MAKE_NONMOVABLE(Script);

public:
    RUNTIME_API Script(UUID reflectionScriptUUID);
    RUNTIME_API virtual ~Script();

    NODISCARD ALWAYS_INLINE UUID GetReflectionScriptUUID() const { return m_ReflectionScriptUUID; }
    NODISCARD RUNTIME_API bool IsDerivedFrom(UUID baseReflectionScriptUUID) const;

    template<typename BaseScriptType>
    NODISCARD ALWAYS_INLINE bool IsDerivedFrom() const
    {
        static_assert(ReflectionScriptMetadata<BaseScriptType>::IsSpecialized);
        const UUID baseReflectionScriptUUID = ReflectionScriptMetadata<BaseScriptType>::ScriptUUID;
        return IsDerivedFrom(baseReflectionScriptUUID);
    }

public:
    RUNTIME_API virtual void OnBeginPlay();
    RUNTIME_API virtual void OnEndPlay();
    RUNTIME_API virtual void OnUpdate(float deltaTime);

private:
    UUID m_ReflectionScriptUUID;
};

} // namespace SE
