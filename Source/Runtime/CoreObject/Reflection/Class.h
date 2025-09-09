// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Delegate.h>
#include <Runtime/CoreObject/Forward.h>
#include <Runtime/CoreObject/Reflection/Struct.h>

namespace SE
{

SE_DECLARE_DELEGATE_WITH_RETURN_TWO_PARAMS(ConstructObjectInPlaceDelegate, Object*, void*, const ObjectInitializer&);

class ObjectClass : public ObjectStruct
{
public:
    ObjectClass(const ObjectInitializer& objectInitializer);
    virtual ~ObjectClass() override = default;
    NODISCARD virtual SObjectPtr<ObjectClass> GetClass() const { return {}; }

public:
    NODISCARD FORCEINLINE Object* ConstructInPlace(void* memoryBlock, const ObjectInitializer& objectInitializer) const
    {
        Object* objectInstance = m_ConstructInPlaceDelegate.Execute(memoryBlock, objectInitializer);
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
