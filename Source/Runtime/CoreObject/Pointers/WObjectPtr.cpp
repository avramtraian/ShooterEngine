// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/CoreObject/Pointers/WObjectPtr.h>

namespace SE
{

SHOOTER_API bool InvokeGlobalObjectEnvironment_IsSlotValid(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration)
{
    const bool isSlotValid = GlobalObjectEnvironment::IsSlotValid(slotIndex, slotGeneration);
    return isSlotValid;
}

SHOOTER_API SObjectPtr<Object> InvokeGlobalObjectEnvironment_GetSlot(EnvironmentSlotIndex slotIndex, EnvironmentSlotGeneration slotGeneration)
{
    return GlobalObjectEnvironment::GetSlot(slotIndex, slotGeneration);
}

SHOOTER_API EnvironmentSlotGeneration InvokeGlobalObjectEnvironment_GetSlotGeneration(EnvironmentSlotIndex slotIndex)
{
    const EnvironmentSlotGeneration slotGeneration = GlobalObjectEnvironment::GetSlotGeneration(slotIndex);
    return slotGeneration;
}

}
