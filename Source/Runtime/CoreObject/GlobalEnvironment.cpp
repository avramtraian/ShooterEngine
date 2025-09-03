// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Log.h>
#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/CoreObject/Object.h>
#include <Runtime/CoreObject/Pointers/WObjectPtr.h>
#include <Runtime/CoreObject/Reflection/Class.h>
#include <Runtime/CoreObject/Reflection/Enum.h>
#include <Runtime/CoreObject/Reflection/Struct.h>

namespace SE
{

struct ObjectSlot
{
    Object* ObjectInstance;
    uint32 SlotGeneration { INVALID_ENVIRONMENT_SLOT_GENERATION };
};

struct GlobalObjectEnvironmentData
{
    Vector<ObjectSlot> Slots;
    Vector<uint32> AvailableSlotIndices;

    HashMap<String, SObjectPtr<ObjectClass>> Classes;
    HashMap<String, SObjectPtr<ObjectEnum>> Enums;
    HashMap<String, SObjectPtr<ObjectStruct>> Structs;
};
GlobalObjectEnvironmentData* s_EnvironmentData;

bool GlobalObjectEnvironment::Initialize()
{
    if (s_EnvironmentData)
    {
        SE_LOG_ERROR("The global object environment was already initialized!");
        return false;
    }
    s_EnvironmentData = new GlobalObjectEnvironmentData();

    // NOTE(Traian): Initialize the class of the base 'Object' type. Since it has no reflected fields,
    // no default instance is required and no reflection macros shall be used.
    SObjectPtr<ObjectClass> objectClass = FindOrCreateObjectClassByName(VIEW("Object"));
    objectClass->SetName(VIEW("Object"));
    objectClass->SetStructureByteCount(sizeof(Object));

    return true;
}

void GlobalObjectEnvironment::Shutdown()
{
    if (!s_EnvironmentData)
    {
        SE_LOG_WARN("The global object environment was already shut down or was never initialized!");
        return;
    }

    s_EnvironmentData->Classes.ClearAndShrink();
    s_EnvironmentData->Enums.ClearAndShrink();
    s_EnvironmentData->Structs.ClearAndShrink();

    if (s_EnvironmentData->AvailableSlotIndices.Count() != s_EnvironmentData->Slots.Count())
    {
        // TODO(Traian): This error message could be a lot more helpful. Since all objects are registered
        // in the global object environment, and the great majority of fields are part of the reflection
        // system, we can determine exactly what objects are still alive and why they are still alive. We
        // can find reference loops (thus this functionality should be part of the public API as a standalone
        // function), and report the findings to the user instead of logging the following generic message.

        SE_LOG_WARN("Objects are still alive when the global object environment is set to shutdown!");
    }

    delete s_EnvironmentData;
    s_EnvironmentData = nullptr;
}

SObjectPtr<Object> GlobalObjectEnvironment::CreateObject(SObjectPtr<ObjectClass> objectClass)
{
    // Get the index of an empty object slot.
    const EnvironmentSlotIndex slotIndex = AcquireEmptySlotIndex();

    // Allocate memory and instantiate the object instance.
    void* objectMemoryBlock = ::operator new(objectClass->GetStructureByteCount());
    Object* objectInstance = objectClass->ConstructInPlace(objectMemoryBlock);

    // Construct the object slot.
    return ConstructSlot(slotIndex, objectInstance);
}

void GlobalObjectEnvironment::DestroyObject(Object* object)
{
    // Destroy the object instance and release its memory block.
    SE_ASSERT(object->GetReferenceCount() == 0);
    const EnvironmentSlotIndex slotIndex = object->GetEnvironmentSlotIndex();
    object->~Object();
    ::operator delete(object);

    // Invalidate the object slot.
    SE_ASSERT(s_EnvironmentData->Slots[slotIndex].ObjectInstance == object);
    s_EnvironmentData->Slots[slotIndex].ObjectInstance = nullptr;
    s_EnvironmentData->Slots[slotIndex].SlotGeneration++;
    s_EnvironmentData->AvailableSlotIndices.Add(slotIndex);
}

EnvironmentSlotGeneration GlobalObjectEnvironment::GetSlotGeneration(EnvironmentSlotIndex slotIndex)
{
    // Check that the slot index has a valid value.
    if (slotIndex == INVALID_ENVIRONMENT_SLOT_INDEX)
        return INVALID_ENVIRONMENT_SLOT_GENERATION;

    // Check that the slot index is not out of bounds.
    if (slotIndex >= s_EnvironmentData->Slots.Count())
        return INVALID_ENVIRONMENT_SLOT_GENERATION;

    return s_EnvironmentData->Slots[slotIndex].SlotGeneration;
}

bool GlobalObjectEnvironment::IsSlotValid(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration)
{
    // Get the current generation of the slot. If the provided slot index is not valid, this function
    // will return 'INVALID_ENVIRONMENT_SLOT_GENERATION', which is handled by the next if-statement.
    const EnvironmentSlotGeneration currentSlotGeneration = GetSlotGeneration(slotIndex);
    
    // Check that the slot generation values are valid.
    if (currentSlotGeneration == INVALID_ENVIRONMENT_SLOT_GENERATION || slotGeneration == INVALID_ENVIRONMENT_SLOT_GENERATION)
        return false;
    
    return (currentSlotGeneration == slotGeneration);
}

SObjectPtr<Object> GlobalObjectEnvironment::GetSlot(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration)
{
    if (!IsSlotValid(slotIndex, slotGeneration))
        return {};

    SObjectPtr<Object> objectPointer = SObjectPtr<Object>(s_EnvironmentData->Slots[slotIndex].ObjectInstance);
    return objectPointer;
}

SObjectPtr<ObjectClass> GlobalObjectEnvironment::FindOrCreateObjectClassByName(StringView className)
{
    // Check if the provided class name already has an associated object class.
    auto existingObjectClass = s_EnvironmentData->Classes.GetIfExists(className);
    if (existingObjectClass.HasValue())
        return existingObjectClass.Value();

    // Get the index of an empty object slot.
    const EnvironmentSlotIndex slotIndex = AcquireEmptySlotIndex();

    // Allocate memory and instantiate the object instance.
    ObjectClass* objectClassInstance = new ObjectClass();

    // Construct the object slot and register the object class.
    SObjectPtr<ObjectClass> objectClass = ConstructSlot(slotIndex, objectClassInstance).As<ObjectClass>();
    s_EnvironmentData->Classes.Add(className, objectClass);
    return objectClass;
}

SObjectPtr<ObjectEnum> GlobalObjectEnvironment::FindOrCreateObjectEnumByName(StringView enumName)
{
    // Check if the provided class enum already has an associated object enum.
    auto existingObjectEnum = s_EnvironmentData->Enums.GetIfExists(enumName);
    if (existingObjectEnum.HasValue())
        return existingObjectEnum.Value();

    // Get the index of an empty object slot.
    const EnvironmentSlotIndex slotIndex = AcquireEmptySlotIndex();

    // Allocate memory and instantiate the object instance.
    ObjectEnum* objectEnumInstance = new ObjectEnum();

    // Construct the object slot and register the object enum.
    SObjectPtr<ObjectEnum> objectEnum = ConstructSlot(slotIndex, objectEnumInstance).As<ObjectEnum>();
    s_EnvironmentData->Enums.Add(enumName, objectEnum);
    return objectEnum;
}

SObjectPtr<ObjectStruct> GlobalObjectEnvironment::FindOrCreateObjectStructByName(StringView structName)
{
    // Check if the provided struct name already has an associated object struct.
    auto existingObjectStruct = s_EnvironmentData->Structs.GetIfExists(structName);
    if (existingObjectStruct.HasValue())
        return existingObjectStruct.Value();

    // Get the index of an empty object slot.
    const EnvironmentSlotIndex slotIndex = AcquireEmptySlotIndex();

    // Allocate memory and instantiate the object instance.
    ObjectStruct* objectStructInstance = new ObjectStruct();

    // Construct the object slot.
    SObjectPtr<ObjectStruct> objectStruct = ConstructSlot(slotIndex, objectStructInstance).As<ObjectStruct>();
    s_EnvironmentData->Structs.Add(structName, objectStruct);
    return objectStruct;
}

void* GlobalObjectEnvironment::AllocateMemoryForObjectType(usize typeByteCount)
{
    // TODO(Traian): Implement a linear memory arena to fetch memory for the type system
    // instead of allocating directly from the heap, as it is very inefficient.
    return ::operator new(typeByteCount);
}

EnvironmentSlotIndex GlobalObjectEnvironment::AcquireEmptySlotIndex()
{
    if (s_EnvironmentData->AvailableSlotIndices.HasElements())
    {
        const EnvironmentSlotIndex slotIndex = s_EnvironmentData->AvailableSlotIndices.Last();
        s_EnvironmentData->AvailableSlotIndices.PopBack();
        return slotIndex;
    }
    else
    {
        const EnvironmentSlotIndex slotIndex = (EnvironmentSlotIndex)s_EnvironmentData->Slots.Count();
        s_EnvironmentData->Slots.Emplace();
        return slotIndex;
    }
}

SObjectPtr<Object> GlobalObjectEnvironment::ConstructSlot(EnvironmentSlotIndex slotIndex, Object* objectInstance)
{
    // Initialize the object slot.
    ObjectSlot& objectSlot = s_EnvironmentData->Slots[slotIndex];
    objectSlot.ObjectInstance = objectInstance;
    objectSlot.SlotGeneration++;
    objectInstance->SetEnvironmentSlotIndex({}, slotIndex);

    // Create the strong object pointer and return it.
    SObjectPtr<Object> objectPointer = SObjectPtr<Object>(objectInstance);
    return objectPointer;
}

}
