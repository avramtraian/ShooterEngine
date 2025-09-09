// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Badge.h>
#include <Runtime/Core/NumericLimits.h>
#include <Runtime/CoreObject/EnvironmentSlotIndex.h>
#include <Runtime/CoreObject/Forward.h>

namespace SE
{

struct ObjectInitializer
{
    uint32 InitialReferenceCount { 0 };
    EnvironmentSlotIndex EnvSlotIndex { INVALID_ENVIRONMENT_SLOT_INDEX };
};

//
// The base class for the 'Object' class.
// This type exists mostly for header-include related reasons. No class should directly inherit
// from this, and instead the inheritance chain should start with the 'Object' class.
//
class ObjectBase
{
    SE_MAKE_NONCOPYABLE(ObjectBase);
    SE_MAKE_NONMOVABLE(ObjectBase);

public:
    SHOOTER_API ObjectBase(const ObjectInitializer& objectInitializer);
    SHOOTER_API virtual ~ObjectBase();

public:
    NODISCARD FORCEINLINE uint32 GetReferenceCount() const
    {
        return m_ReferenceCount;
    }

    NODISCARD FORCEINLINE void IncrementReferenceCount()
    {
        SE_ASSERT(m_ReferenceCount < NumericLimits<uint32>::Max());
        ++m_ReferenceCount;
    }

    NODISCARD FORCEINLINE void DecrementReferenceCount()
    {
        SE_ASSERT(m_ReferenceCount > NumericLimits<uint32>::Min());
        --m_ReferenceCount;
    }

public:
    NODISCARD FORCEINLINE EnvironmentSlotIndex GetEnvironmentSlotIndex() const
    {
        SE_ASSERT(m_EnvironmentSlotIndex != INVALID_ENVIRONMENT_SLOT_INDEX);
        return m_EnvironmentSlotIndex;
    }
    
private:
    //
    // The number of active strong object pointer that point towards this object instance.
    // As long as this reference count is greater than zero, the object can't be destroyed. As soon
    // as the last strong object pointer that references this instance is released, the reference count
    // hits zero and the global object environment destroys it.
    //
    uint32 m_ReferenceCount { 0 };

    //
    // The slot index where this object instance is stored in the global object environment. This index
    // is heavily used by the weak object pointers (in combination with slot generations) to reference
    // objects without keeping them alive and be able to safely check if the object is alive or not.
    //
    EnvironmentSlotIndex m_EnvironmentSlotIndex { INVALID_ENVIRONMENT_SLOT_INDEX };
};

}
