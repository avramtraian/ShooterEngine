// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Delegate.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Reflection/Struct.h>

namespace SE
{

SE_DECLARE_DELEGATE_WITH_RETURN_ONE_PARAM(ConstructObjectInPlaceDelegate, Object*, void*);

class ObjectClass : public ObjectStruct
{
public:
    ObjectClass() = default;
    virtual ~ObjectClass() override = default;
    NODISCARD virtual SObjectPtr<ObjectClass> GetClass() const { return {}; }

public:
    NODISCARD FORCEINLINE Object* ConstructInPlace(void* memoryBlock) const
    {
        Object* objectInstance = m_ConstructInPlaceDelegate.Execute(memoryBlock);
        return objectInstance;
    }

    NODISCARD FORCEINLINE ConstructObjectInPlaceDelegate& GetConstructInPlaceDelegate()
    {
        SE_ASSERT(!m_ConstructInPlaceDelegate.IsBound());
        return m_ConstructInPlaceDelegate;
    }

private:
    ConstructObjectInPlaceDelegate m_ConstructInPlaceDelegate;
};

}
