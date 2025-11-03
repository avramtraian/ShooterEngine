// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/VectorView.h>
#include <Runtime/Core/Memory/BufferView.h>

namespace SE
{

class Buffer
{
    SE_MAKE_NONCOPYABLE(Buffer);

public:
    NODISCARD RUNTIME_API static Buffer Copy(const Buffer& sourceBuffer);
    NODISCARD RUNTIME_API static Buffer Copy(ReadonlyBufferView sourceBuffer);

public:
    RUNTIME_API Buffer();
    RUNTIME_API ~Buffer();

    RUNTIME_API         Buffer(Buffer&& other) noexcept;
    RUNTIME_API Buffer& operator=(Buffer&& other) noexcept;

public:
    NODISCARD ALWAYS_INLINE void*       Data() { return m_Data; }
    NODISCARD ALWAYS_INLINE const void* Data() const { return m_Data; }

    NODISCARD ALWAYS_INLINE uint8*       Bytes() { return (uint8*)m_Data; }
    NODISCARD ALWAYS_INLINE const uint8* Bytes() const { return (const uint8*)m_Data; }

    NODISCARD ALWAYS_INLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD ALWAYS_INLINE bool  IsEmpty() const { return (m_ByteCount == 0); }
    NODISCARD ALWAYS_INLINE bool  HasData() const { return (m_ByteCount > 0); }

    template<typename T>
    NODISCARD ALWAYS_INLINE T* DataAs()
    {
        return (T*)m_Data;
    }
    template<typename T>
    NODISCARD ALWAYS_INLINE const T* DataAs() const
    {
        return (const T*)m_Data;
    }

public:
    NODISCARD ALWAYS_INLINE ReadonlyBufferView ToView() const { return ReadonlyBufferView(*this); }

    NODISCARD ALWAYS_INLINE VectorView<uint8> ToVectorView() { return VectorView<uint8>(Bytes(), m_ByteCount); }
    NODISCARD ALWAYS_INLINE VectorView<const uint8> ToVectorView() const { return VectorView<const uint8>(Bytes(), m_ByteCount); }

public:
    RUNTIME_API void SetByteCount(usize byteCount);
    RUNTIME_API void EnsureByteCount(usize byteCount);

    RUNTIME_API void SetByteCountWithoutCopy(usize byteCount);
    RUNTIME_API void EnsureByteCountWithoutCopy(usize byteCount);

    RUNTIME_API void Release();

private:
    void* m_Data;
    usize m_ByteCount;
};

//////////////////////////////////////////////////////////////////////////////
// The following constructors are declared in the 'BufferView.h', but their //
// implementation is in this file in order to avoid circular includes.      //
//////////////////////////////////////////////////////////////////////////////

ALWAYS_INLINE ReadonlyBufferView::ReadonlyBufferView(const Buffer& buffer)
    : m_Data(buffer.Data())
    , m_ByteCount(buffer.ByteCount())
{}

ALWAYS_INLINE ReadonlyBufferView& ReadonlyBufferView::operator=(const Buffer& buffer)
{
    m_Data      = buffer.Data();
    m_ByteCount = buffer.ByteCount();
    return *this;
}

ALWAYS_INLINE BufferView::BufferView(Buffer& buffer)
    : m_Data(buffer.Data())
    , m_ByteCount(buffer.ByteCount())
{}

ALWAYS_INLINE BufferView& BufferView::operator=(Buffer& buffer)
{
    m_Data      = buffer.Data();
    m_ByteCount = buffer.ByteCount();
    return *this;
}

} // namespace SE
