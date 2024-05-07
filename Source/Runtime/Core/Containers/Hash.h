/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Assertions.h"
#include "Core/CoreTypes.h"

namespace SE {

template<typename T>
struct Hasher {
    NODISCARD ALWAYS_INLINE static u64 get_hash(const T&)
    {
        SE_ASSERT(false);
        return 0;
    }
};

template<>
struct Hasher<void*> {
    NODISCARD ALWAYS_INLINE static u64 get_hash(const void* const& value)
    {
        // TODO: Provide a better hashing function for raw pointers.
        return reinterpret_cast<u64>(value);
    }
};

} // namespace SE