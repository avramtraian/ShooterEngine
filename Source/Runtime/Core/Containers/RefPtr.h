/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Assertions.h"
#include "Core/CoreTypes.h"

namespace SE {

template<typename T>
class RefCounted {
public:
    RefCounted()
        : m_reference_count(0)
    {}
    virtual ~RefCounted() = default;

    void increment_reference_count()
    {
        SE_ASSERT_DEBUG(m_reference_count > 0);

        // TODO: Ensure that addition would not overflow.
        ++m_reference_count;
    }

    bool decrement_reference_count()
    {
        SE_ASSERT_DEBUG(m_reference_count > 0);

        if (--m_reference_count == 0) {
            auto* self = static_cast<T*>(this);
            delete self;
            return true;
        }

        return false;
    }

    u32 reference_count() const
    {
        SE_ASSERT(m_reference_count > 0);
        return m_reference_count;
    }

private:
    u32 m_reference_count;
};

template<typename T>
class RefPtr {
    template<typename Q>
    friend RefPtr<Q> adopt_ref(Q*);

public:
    ALWAYS_INLINE RefPtr()
        : m_instance(nullptr)
    {}

    ALWAYS_INLINE RefPtr(const RefPtr& other)
        : m_instance(other.m_instance)
    {
        if (m_instance)
            increment_reference_count();
    }

    ALWAYS_INLINE RefPtr(RefPtr&& other) noexcept
        : m_instance(other.m_instance)
    {
        other.m_instance = nullptr;
    }

    ALWAYS_INLINE ~RefPtr()
    {
        if (m_instance)
            decrement_reference_count();
    }

    ALWAYS_INLINE RefPtr& operator=(const RefPtr& other)
    {
        clear();
        m_instance = other.m_instance;
        if (m_instance)
            increment_reference_count();
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(RefPtr&& other) noexcept
    {
        clear();
        m_instance = other.m_instance;
        other.m_instance = nullptr;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const { return (m_instance != nullptr); }
    NODISCARD ALWAYS_INLINE operator bool() const { return is_valid(); }

    NODISCARD ALWAYS_INLINE T& get() const
    {
        SE_ASSERT(is_valid());
        return *m_instance;
    }

    NODISCARD ALWAYS_INLINE T* operator->() const { return &get(); }
    NODISCARD ALWAYS_INLINE T& operator*() const { return get(); }

    NODISCARD ALWAYS_INLINE T* raw() const { return m_instance; }

    ALWAYS_INLINE void clear()
    {
        if (m_instance) {
            decrement_reference_count();
            m_instance = nullptr;
        }
    }

private:
    ALWAYS_INLINE explicit RefPtr(T* pointer_to_adopt)
        : m_instance(pointer_to_adopt)
    {
        if (m_instance)
            increment_reference_count();
    }

    void increment_reference_count() { static_cast<RefCounted<T>*>(m_instance)->increment_reference_count(); }
    void decrement_reference_count() { static_cast<RefCounted<T>*>(m_instance)->decrement_reference_count(); }

private:
    T* m_instance;
};

template<typename T>
NODISCARD ALWAYS_INLINE RefPtr<T> adopt_ref(T* pointer_to_adopt)
{
    return RefPtr<T>(pointer_to_adopt);
}

template<typename T>
NODISCARD ALWAYS_INLINE RefPtr<T> adopt_ref(T& pointer_to_adopt)
{
    return adopt_ref(&pointer_to_adopt);
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE RefPtr<T> make_ref(Args&&... args)
{
    auto* raw_pointer = new T(forward<Args>(args)...);
    return adopt_ref(raw_pointer);
}

} // namespace SE