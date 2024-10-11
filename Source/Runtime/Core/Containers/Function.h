/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Span.h>

namespace SE
{

template<typename>
class Function;

template<typename ReturnType, typename... ParameterTypes>
class Function<ReturnType(ParameterTypes...)>
{
public:
    static constexpr usize inline_capacity = 2 * sizeof(uintptr);
    static constexpr usize inline_buffer_alignment = sizeof(uintptr);

    class FunctorBase
    {
    public:
        virtual ~FunctorBase() = default;

        virtual ReturnType invoke(ParameterTypes&&... parameters) = 0;
        virtual void copy_to(WriteonlyBytes destination_buffer) = 0;
        virtual void move_to(WriteonlyBytes destination_buffer) = 0;
    };

    template<typename FunctorType>
    class Functor : public FunctorBase
    {
    public:
        Functor(const FunctorType& functor)
            : m_functor(functor)
        {}

        Functor(FunctorType&& functor)
            : m_functor(move(functor))
        {}

        virtual ~Functor() override = default;

    public:
        virtual ReturnType invoke(ParameterTypes&&... parameters) override { return m_functor(forward<ParameterTypes>(parameters)...); }
        virtual void copy_to(WriteonlyBytes destination_buffer) override { new (destination_buffer) Functor(m_functor); }
        virtual void move_to(WriteonlyBytes destination_buffer) override { new (destination_buffer) Functor(move(m_functor)); }

    private:
        FunctorType m_functor;
    };

public:
    ALWAYS_INLINE Function()
        : m_inline_buffer {}
        , m_functor_byte_count(0)
    {}

    ALWAYS_INLINE Function(const Function& other)
    {
        if (!other.has_value())
        {
            m_functor_byte_count = 0;
            return;
        }

        ReadWriteBytes this_functor = m_inline_buffer;
        ReadonlyBytes other_functor = other.m_inline_buffer;

        if (other.is_stored_on_heap())
        {
            m_heap_buffer = allocate_memory(other.m_functor_byte_count);
            this_functor = m_heap_buffer.elements();
            other_functor = other.m_heap_buffer;
        }

        m_functor_byte_count = other.m_functor_byte_count;
        reinterpret_cast<FunctorBase*>(other_functor)->copy_to(this_functor);
    }

    ALWAYS_INLINE Function(Function&& other) noexcept
    {
        if (other.is_stored_inline())
        {
            FunctorBase* other_functor = reinterpret_cast<FunctorBase*>(other.m_inline_buffer);
            other_functor->move_to(m_inline_buffer);
            other_functor->~FunctorBase();
        }
        else
        {
            m_heap_buffer = other.m_heap_buffer;
        }

        m_functor_byte_count = other.m_functor_byte_count;
        other.m_functor_byte_count = 0;
    }

    template<typename FunctorType>
    ALWAYS_INLINE Function(FunctorType typed_functor)
    {
        using TypedFunctor = Functor<FunctorType>;
        m_functor_byte_count = sizeof(TypedFunctor);

        if constexpr (sizeof(TypedFunctor) <= inline_capacity)
        {
            new (m_inline_buffer) TypedFunctor(move(typed_functor));
        }
        else
        {
            m_heap_buffer = allocate_memory(sizeof(TypedFunctor));
            new (*m_heap_buffer) TypedFunctor(move(typed_functor));
        }
    }

    ALWAYS_INLINE ~Function()
    {
        if (m_functor_byte_count <= inline_capacity)
        {
            if (m_functor_byte_count > 0)
                reinterpret_cast<FunctorBase*>(m_inline_buffer)->~FunctorBase();
        }
        else
        {
            reinterpret_cast<FunctorBase*>(*m_heap_buffer)->~FunctorBase();
            release_memory(m_heap_buffer);
        }
    }

    ALWAYS_INLINE Function& operator=(const Function& other)
    {
        functor().~FunctorBase();

        if (other.m_functor_byte_count > buffer_capacity())
        {
            if (is_stored_on_heap())
                release_memory(m_heap_buffer);

            m_heap_buffer = allocate_memory(other.m_functor_byte_count);
        }
        else if (other.is_stored_inline())
        {
            if (is_stored_on_heap())
                release_memory(m_heap_buffer);
        }

        m_functor_byte_count = other.m_functor_byte_count;
        if (other.has_value())
            other.functor().copy_to(functor());

        return *this;
    }

    ALWAYS_INLINE Function& operator=(Function&& other) noexcept
    {
        clear();

        if (other.is_stored_inline())
        {
            FunctorBase* other_functor = reinterpret_cast<FunctorBase*>(other.m_inline_buffer);
            other_functor->move_to(m_inline_buffer);
            other_functor->~FunctorBase();
        }
        else
        {
            m_heap_buffer = other.m_heap_buffer;
        }

        m_functor_byte_count = other.m_functor_byte_count;
        other.m_functor_byte_count = 0;

        return *this;
    }

    template<typename FunctorType>
    ALWAYS_INLINE Function& operator=(FunctorType typed_functor)
    {
        using TypedFunctor = Functor<FunctorType>;

        functor().~FunctorBase();

        if constexpr (sizeof(TypedFunctor) <= inline_capacity)
        {
            if (is_stored_on_heap())
                release_memory(m_heap_buffer);
        }
        else
        {
            if (sizeof(TypedFunctor) > buffer_capacity())
            {
                if (is_stored_on_heap())
                    release_memory(m_heap_buffer);

                m_heap_buffer = allocate_memory(sizeof(TypedFunctor));
            }
        }

        m_functor_byte_count = sizeof(TypedFunctor);
        new (&functor()) TypedFunctor(move(typed_functor));

        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE bool has_value() const { return (m_functor_byte_count > 0); }
    NODISCARD ALWAYS_INLINE operator bool() const { return has_value(); }

    ALWAYS_INLINE ReturnType operator()(ParameterTypes&&... parameters) { return functor().invoke(forward<ParameterTypes>(parameters)...); }

    ALWAYS_INLINE void clear()
    {
        if (is_stored_inline())
        {
            reinterpret_cast<FunctorBase*>(m_inline_buffer)->~FunctorBase();
        }
        else
        {
            reinterpret_cast<FunctorBase*>(*m_heap_buffer)->~FunctorBase();
            release_memory(m_heap_buffer);
            m_heap_buffer = {};
        }

        m_functor_byte_count = 0;
    }

private:
    NODISCARD ALWAYS_INLINE static ReadWriteByteSpan allocate_memory(usize byte_count)
    {
        void* memory_block = ::operator new(byte_count);
        SE_ASSERT(memory_block != nullptr);
        return ReadWriteByteSpan(static_cast<ReadWriteBytes>(memory_block), byte_count);
    }

    ALWAYS_INLINE static void release_memory(ReadWriteByteSpan heap_buffer) { ::operator delete(heap_buffer.elements()); }

private:
    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return (m_functor_byte_count <= inline_capacity); }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return (m_functor_byte_count > inline_capacity); }

    NODISCARD ALWAYS_INLINE usize buffer_capacity() const { return is_stored_inline() ? inline_capacity : m_heap_buffer.count(); }

    NODISCARD ALWAYS_INLINE FunctorBase& functor()
    {
        ReadWriteBytes buffer = is_stored_inline() ? m_inline_buffer : *m_heap_buffer;
        return *reinterpret_cast<FunctorBase*>(buffer);
    }

    NODISCARD ALWAYS_INLINE const FunctorBase& functor() const
    {
        ReadonlyBytes buffer = is_stored_inline() ? m_inline_buffer : *m_heap_buffer;
        return *reinterpret_cast<const FunctorBase*>(buffer);
    }

private:
    union
    {
        ReadWriteByteSpan m_heap_buffer;
        alignas(inline_buffer_alignment) u8 m_inline_buffer[inline_capacity];
    };

    usize m_functor_byte_count;
};

} // namespace SE
