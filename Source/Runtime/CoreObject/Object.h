// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Badge.h>
#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/CoreObject/EnvironmentSlotIndex.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/ObjectBase.h>
#include <Runtime/CoreObject/Pointers/SObjectPtr.h>

namespace SE
{

class Object : public ObjectBase
{
    SE_MAKE_NONCOPYABLE(Object);
    SE_MAKE_NONMOVABLE(Object);

public:
    SHOOTER_API Object(const ObjectInitializer& objectInitializer);
    SHOOTER_API virtual ~Object() override;
    SHOOTER_API virtual void OnDestructor();

public:
    NODISCARD static SObjectPtr<ObjectClass> GetStaticClass();
    NODISCARD virtual SObjectPtr<ObjectClass> GetClass() const = 0;
};

}
