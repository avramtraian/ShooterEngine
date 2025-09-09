// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/CoreObject/EnvironmentSlotIndex.h>
#include <Runtime/CoreObject/Pointers/SObjectPtr.h>
#include <Runtime/CoreObject/Object.h>

namespace SE
{

// Internal use only.
NODISCARD SHOOTER_API bool InvokeGlobalObjectEnvironment_IsSlotValid(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration);
NODISCARD SHOOTER_API SObjectPtr<Object> InvokeGlobalObjectEnvironment_GetSlot(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration);
NODISCARD SHOOTER_API EnvironmentSlotGeneration InvokeGlobalObjectEnvironment_GetSlotGeneration(EnvironmentSlotIndex slotIndex);

template<typename T>
class WObjectPtr
{
public:
    template<typename Q>
    friend class SObjectPtr;

    template<typename Q>
    friend class WObjectPtr;

    friend class GlobalObjectEnvironment;

public:
    NODISCARD FORCEINLINE static uint64 GetHash(const WObjectPtr& value)
    {
        if (!value.IsValid())
            return 0;
        return (((uint64)value.m_SlotIndex << 32) | ((uint64)value.m_SlotGeneration << 0));
    }

public:
    FORCEINLINE WObjectPtr()
        : m_SlotIndex(INVALID_ENVIRONMENT_SLOT_INDEX)
        , m_SlotGeneration(INVALID_ENVIRONMENT_SLOT_GENERATION)
    {}

    WObjectPtr(const WObjectPtr&) = default;
    WObjectPtr& operator=(const WObjectPtr&) = default;

    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE WObjectPtr(const WObjectPtr<Q>& other)
        : m_SlotIndex(other.m_SlotIndex)
        , m_SlotGeneration(other.m_SlotGeneration)
    {}

    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE WObjectPtr& operator=(const WObjectPtr<Q>& other)
    {
        m_SlotIndex = other.m_SlotIndex;
        m_SlotGeneration = other.m_SlotGeneration;
        return *this;
    }

    FORCEINLINE WObjectPtr(WObjectPtr&& other) noexcept
        : m_SlotIndex(other.m_SlotIndex)
        , m_SlotGeneration(other.m_SlotGeneration)
    {
        other.m_SlotIndex = INVALID_ENVIRONMENT_SLOT_INDEX;
        other.m_SlotGeneration = INVALID_ENVIRONMENT_SLOT_GENERATION;
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE WObjectPtr(WObjectPtr<Q>&& other) noexcept
        : m_SlotIndex(other.m_SlotIndex)
        , m_SlotGeneration(other.m_SlotGeneration)
    {
        other.m_SlotIndex = INVALID_ENVIRONMENT_SLOT_INDEX;
        other.m_SlotGeneration = INVALID_ENVIRONMENT_SLOT_GENERATION;
    }

    FORCEINLINE WObjectPtr& operator=(WObjectPtr&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_SlotIndex = other.m_SlotIndex;
        m_SlotGeneration = other.m_SlotGeneration;

        other.m_SlotIndex = INVALID_ENVIRONMENT_SLOT_INDEX;
        other.m_SlotGeneration = INVALID_ENVIRONMENT_SLOT_GENERATION;

        return *this;
    }
    
    template<typename Q>
    requires (std::is_base_of_v<T, Q> && !std::is_same_v<T, Q>)
    FORCEINLINE WObjectPtr& operator=(WObjectPtr<Q>&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_SlotIndex = other.m_SlotIndex;
        m_SlotGeneration = other.m_SlotGeneration;

        other.m_SlotIndex = INVALID_ENVIRONMENT_SLOT_INDEX;
        other.m_SlotGeneration = INVALID_ENVIRONMENT_SLOT_GENERATION;

        return *this;
    }

public:
    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    FORCEINLINE WObjectPtr(const SObjectPtr<Q>& strongObjectPointer)
        : m_SlotIndex(INVALID_ENVIRONMENT_SLOT_INDEX)
        , m_SlotGeneration(INVALID_ENVIRONMENT_SLOT_GENERATION)
    {
        if (strongObjectPointer.IsValid())
        {
            const ObjectBase* objectBase = (const ObjectBase*)strongObjectPointer.Get();
            m_SlotIndex = objectBase->GetEnvironmentSlotIndex();
            m_SlotGeneration = InvokeGlobalObjectEnvironment_GetSlotGeneration(m_SlotIndex);
        }
    }

    template<typename Q>
    requires (std::is_base_of_v<T, Q>)
    FORCEINLINE WObjectPtr& operator=(const SObjectPtr<Q>& strongObjectPointer)
    {
        Release();

        if (strongObjectPointer.IsValid())
        {
            const ObjectBase* objectBase = (const ObjectBase*)strongObjectPointer.Get();
            m_SlotIndex = objectBase->GetEnvironmentSlotIndex();
            m_SlotGeneration = InvokeGlobalObjectEnvironment_GetSlotGeneration(m_SlotIndex);
        }

        return *this;
    }

public:
    NODISCARD FORCEINLINE SObjectPtr<T> Get() const
    {
        SObjectPtr<Object> strongObjectPointer = InvokeGlobalObjectEnvironment_GetSlot(m_SlotIndex, m_SlotGeneration);
        SE_ASSERT(strongObjectPointer.IsValid());
        return strongObjectPointer.As<T>();
    }

public:
    NODISCARD FORCEINLINE bool IsValid() const
    {
        // NOTE(Traian): The implementation of this function checks that the slot index and slot generation are valid values.
        const bool isSlotValid = InvokeGlobalObjectEnvironment_IsSlotValid(m_SlotIndex, m_SlotGeneration);
        return isSlotValid;
    }

    FORCEINLINE void Release()
    {
        m_SlotIndex = INVALID_ENVIRONMENT_SLOT_INDEX;
        m_SlotGeneration = INVALID_ENVIRONMENT_SLOT_GENERATION;
    }

    template<typename Q>
    NODISCARD FORCEINLINE WObjectPtr<Q> As() const
    {
        WObjectPtr<Q> casted;
        casted.m_SlotIndex = m_SlotIndex;
        casted.m_SlotGeneration = m_SlotGeneration;
        return casted;
    }

private:
    EnvironmentSlotIndex m_SlotIndex;
    EnvironmentSlotGeneration m_SlotGeneration;
};

}
