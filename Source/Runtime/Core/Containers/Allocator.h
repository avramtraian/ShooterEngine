// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

class DefaultAllocator
{
    SE_MAKE_NAMESPACE_CLASS(DefaultAllocator);

public:
    NODISCARD FORCEINLINE static void* Allocate(usize byteCount)
    {
        if (byteCount == 0)
            return nullptr;
        void* allocationBlock = ::operator new(byteCount);
        return allocationBlock;
    }

    NODISCARD FORCEINLINE static void Release(void* allocationBlock)
    {
        if (allocationBlock == nullptr)
            return;
        ::operator delete(allocationBlock);
    }

    NODISCARD FORCEINLINE static void* GrowAllocation(void* allocationBlock, usize newByteCount)
    {
        // TODO(Traian): Actually try to grow the current allocation block instead of just fetching a new one.
        return DefaultAllocator::Allocate(newByteCount);
    }
};

}
