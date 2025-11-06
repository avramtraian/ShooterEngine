// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreTypes.h>
#include <Runtime/Core/UUID.h>
#include <Runtime/Scene/Entity.h>
#include <Runtime/Scene/Reflection/ReflectionScript.h>

namespace SE
{

enum class ScriptLifecycleStage : uint8
{
    PendingBeginPlay,
    Updating,
    PendingDestroy,
};

class Script
{
    SE_MAKE_NONCOPYABLE(Script);
    SE_MAKE_NONMOVABLE(Script);

public:
    RUNTIME_API Script(const ScriptInitializationInfo& initializationInfo);
    RUNTIME_API virtual ~Script();

public:
    NODISCARD ALWAYS_INLINE UUID GetReflectionScriptUUID() const { return m_ReflectionScriptUUID; }
    NODISCARD RUNTIME_API bool IsDerivedFrom(UUID baseReflectionScriptUUID) const;

    template<typename BaseScriptType>
    NODISCARD ALWAYS_INLINE bool IsDerivedFrom() const
    {
        static_assert(ReflectionScriptMetadata<BaseScriptType>::IsSpecialized);
        const UUID baseReflectionScriptUUID = ReflectionScriptMetadata<BaseScriptType>::ScriptUUID;
        return IsDerivedFrom(baseReflectionScriptUUID);
    }

    NODISCARD ALWAYS_INLINE Entity GetParentEntity() const { return m_ParentEntity; }
    NODISCARD ALWAYS_INLINE EntityID GetParentEntityID() const { return m_ParentEntity.ID(); }
    NODISCARD ALWAYS_INLINE Scene& GetSceneContext() const { return *m_ParentEntity.SceneContext().GetNonConst(); }

    NODISCARD ALWAYS_INLINE ScriptLifecycleStage GetLifecycleStage() const { return m_LifecycleStage; }
    NODISCARD ALWAYS_INLINE bool IsPendingDestroy() const { return (m_LifecycleStage == ScriptLifecycleStage::PendingDestroy); }

public:
    RUNTIME_API virtual void OnBeginPlay();
    RUNTIME_API virtual void OnEndPlay();
    RUNTIME_API virtual void OnUpdate(float deltaTime);

private:
    UUID                 m_ReflectionScriptUUID;
    Entity               m_ParentEntity;
    ScriptLifecycleStage m_LifecycleStage;
};

} // namespace SE
