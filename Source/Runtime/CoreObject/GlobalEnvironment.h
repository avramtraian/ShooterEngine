// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/CoreObject/EnvironmentSlotIndex.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Object.h>
#include <Runtime/CoreObject/Pointers/SObjectPtr.h>
#include <Runtime/CoreObject/Reflection/Type.h>

namespace SE
{

class GlobalObjectEnvironment
{
    SE_MAKE_NAMESPACE_CLASS(GlobalObjectEnvironment);

public:
    static bool Initialize();
    static void Shutdown();

public:
    NODISCARD SHOOTER_API static SObjectPtr<Object> CreateObject(SObjectPtr<ObjectClass> objectClass);

    SHOOTER_API static void DestroyObject(Object* object);

    template<typename T>
    NODISCARD FORCEINLINE static SObjectPtr<T> CreateObject()
    {
        SObjectPtr<ObjectClass> objectClass = T::GetStaticClass();
        return CreateObject(Move(objectClass)).As<T>();
    }

public:
    NODISCARD SHOOTER_API static EnvironmentSlotGeneration GetSlotGeneration(EnvironmentSlotIndex slotIndex);

    NODISCARD SHOOTER_API static bool IsSlotValid(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration);

    NODISCARD SHOOTER_API static SObjectPtr<Object> GetSlot(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration);

public:
    NODISCARD SHOOTER_API static SObjectPtr<ObjectClass> FindOrCreateObjectClassByName(StringView className);
    NODISCARD SHOOTER_API static SObjectPtr<ObjectEnum> FindOrCreateObjectEnumByName(StringView enumName);
    NODISCARD SHOOTER_API static SObjectPtr<ObjectStruct> FindOrCreateObjectStructByName(StringView structName);

public:
    NODISCARD SHOOTER_API static void* AllocateMemoryForObjectType(usize typeByteCount);

    template<typename T, typename... Args>
    NODISCARD FORCEINLINE static T* AllocateObjectType(Args&&... args)
    {
        void* memoryBlock = AllocateMemoryForObjectType(sizeof(T));
        new (memoryBlock) T(Forward<Args>(args)...);
        return static_cast<T*>(memoryBlock);
    }

private:
    static EnvironmentSlotIndex AllocateSlot(void* objectInstanceMemoryBlock);
};

}
