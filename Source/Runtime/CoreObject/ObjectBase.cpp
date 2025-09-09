// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/CoreObject/ObjectBase.h>

namespace SE
{

ObjectBase::ObjectBase(const ObjectInitializer& objectInitializer)
{
    m_ReferenceCount = objectInitializer.InitialReferenceCount;
    SE_ASSERT(objectInitializer.EnvSlotIndex != INVALID_ENVIRONMENT_SLOT_INDEX);
    m_EnvironmentSlotIndex = objectInitializer.EnvSlotIndex;
}

ObjectBase::~ObjectBase()
{}

}
