// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/Object.h>
#include <Runtime/CoreObject/GlobalEnvironment.h>

namespace SE
{

SObjectPtr<ObjectClass> Object::GetStaticClass()
{
    // TODO(Traian): This call should be beter optimized!
    return GlobalObjectEnvironment::FindOrCreateObjectClassByName(VIEW("Object"));
}

Object::Object(const ObjectInitializer& objectInitializer)
    : ObjectBase(objectInitializer)
{}

Object::~Object()
{}

void Object::OnDestructor()
{}

}
