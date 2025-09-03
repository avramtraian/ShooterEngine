// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/GlobalEnvironment.h>
#include <Runtime/CoreObject/Pointers/SObjectPtr.h>

namespace SE
{

SHOOTER_API void InvokeGlobalObjectEnvironment_DestroyObject(ObjectBase* baseObject)
{
    Object* object = static_cast<Object*>(baseObject);
    GlobalObjectEnvironment::DestroyObject(object);
}

}
