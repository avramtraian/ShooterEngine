/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Assertions.h"
#include "Core/CoreTypes.h"

namespace SE {

template<typename T>
class OwnPtr {
    SE_MAKE_NONCOPYABLE(OwnPtr);

    template<typename Q>
    friend OwnPtr<Q> adopt_own(Q*);
    template<typename Q>
    friend class OwnPtr;
    template<typename Q>
    friend class NonnullOwnPtr;

public:
    ALWAYS_INLINE OwnPtr()
        : m_instance(nullptr)
    {}

    ALWAYS_INLINE OwnPtr(OwnPtr&& other)
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    ALWAYS_INLINE OwnPtr& operator=(OwnPtr&& other)
    {
        release();
        m_instance = other.m_instance;
        other.m_instance = nullptr;
        return *this;
    }

    ALWAYS_INLINE ~OwnPtr() { release(); }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const { return (m_instance != nullptr); }
    NODISCARD ALWAYS_INLINE operator bool() const { return is_valid(); }

    NODISCARD ALWAYS_INLINE T* get()
    {
        SE_ASSERT(is_valid());
        return m_instance;
    }

    NODISCARD ALWAYS_INLINE const T* get() const
    {
        SE_ASSERT(is_valid());
        return m_instance;
    }

    NODISCARD ALWAYS_INLINE T* operator->() { return get(); }
    NODISCARD ALWAYS_INLINE const T* operator->() const { return get(); }

public:
    ALWAYS_INLINE void release()
    {
        if (m_instance) {
            delete m_instance;
            m_instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD ALWAYS_INLINE OwnPtr<Q> as()
    {
        Q* casted_raw_pointer = (Q*)(m_instance);
        m_instance = nullptr;
        return OwnPtr<Q>(casted_raw_pointer);
    }

private:
    ALWAYS_INLINE explicit OwnPtr(T* pointer_to_adopt)
        : m_instance(pointer_to_adopt)
    {}

private:
    T* m_instance;
};

template<typename T>
NODISCARD ALWAYS_INLINE OwnPtr<T> adopt_own(T* pointer_to_adopt)
{
    return OwnPtr<T>(pointer_to_adopt);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE OwnPtr<T> make_own(Args&&... args)
{
    T* raw_pointer = new T(forward<Args>(args)...);
    return adopt_own(raw_pointer);
}

} // namespace SE