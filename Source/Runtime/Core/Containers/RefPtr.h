/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Assertions.h>
#include <Core/CoreTypes.h>

namespace SE
{

class RefCounted
{
    SE_MAKE_NONCOPYABLE(RefCounted);
    SE_MAKE_NONMOVABLE(RefCounted);

    template<typename T>
    friend class RefPtr;

public:
    ALWAYS_INLINE RefCounted()
        : m_reference_count(0)
    {}

    virtual ~RefCounted() = default;

private:
    NODISCARD ALWAYS_INLINE u32 get_reference_count() const
    {
        SE_ASSERT(m_reference_count > 0);
        return m_reference_count;
    }

    ALWAYS_INLINE void increment_reference_count()
    {
        // TODO: Ensure that addition would not overflow.
        ++m_reference_count;
    }

    // Returns true when the instance is destroyed.
    ALWAYS_INLINE bool decrement_reference_count()
    {
        SE_DEBUG_ASSERT(m_reference_count > 0);

        if (--m_reference_count == 0)
        {
            delete this;
            return true;
        }

        return false;
    }

private:
    u32 m_reference_count;
};

template<typename T>
class RefPtr
{
    template<typename Q>
    friend RefPtr<Q> adopt_ref(Q*);
    template<typename Q>
    friend class RefPtr;

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
        release();
        m_instance = other.m_instance;
        if (m_instance)
            increment_reference_count();
        return *this;
    }

    ALWAYS_INLINE RefPtr& operator=(RefPtr&& other) noexcept
    {
        release();
        m_instance = other.m_instance;
        other.m_instance = nullptr;
        return *this;
    }

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

    ALWAYS_INLINE void release()
    {
        if (m_instance)
        {
            decrement_reference_count();
            m_instance = nullptr;
        }
    }

    template<typename Q>
    NODISCARD ALWAYS_INLINE RefPtr<Q> as() const
    {
        Q* casted_raw_pointer = (Q*)(m_instance);
        return RefPtr<Q>(casted_raw_pointer);
    }

    NODISCARD ALWAYS_INLINE bool operator==(const RefPtr& other) const
    {
        if (!is_valid() || !other.is_valid())
            return false;
        return (m_instance == other.m_instance);
    }

    NODISCARD ALWAYS_INLINE bool operator!=(const RefPtr& other) const { return !(*this == other); }

private:
    ALWAYS_INLINE explicit RefPtr(T* pointer_to_adopt)
        : m_instance(pointer_to_adopt)
    {
        if (m_instance)
            increment_reference_count();
    }

    void increment_reference_count() { ((RefCounted*)(m_instance))->increment_reference_count(); }
    void decrement_reference_count() { ((RefCounted*)(m_instance))->decrement_reference_count(); }

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
