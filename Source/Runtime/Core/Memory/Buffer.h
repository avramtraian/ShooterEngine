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
    SHOOTER_API NODISCARD static Buffer Copy(const Buffer& sourceBuffer);
    SHOOTER_API NODISCARD static Buffer Copy(ReadonlyBufferView sourceBuffer);

public:
    SHOOTER_API Buffer();
    SHOOTER_API ~Buffer();

    SHOOTER_API Buffer(Buffer&& other) noexcept;
    SHOOTER_API Buffer& operator=(Buffer&& other) noexcept;

public:
    NODISCARD FORCEINLINE void* Data() { return m_Data; }
    NODISCARD FORCEINLINE const void* Data() const { return m_Data; }

    NODISCARD FORCEINLINE uint8* Bytes() { return (uint8*)m_Data; }
    NODISCARD FORCEINLINE const uint8* Bytes() const { return (const uint8*)m_Data; }

    NODISCARD FORCEINLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD FORCEINLINE bool IsEmpty() const { return (m_ByteCount == 0); }
    NODISCARD FORCEINLINE bool HasData() const { return (m_ByteCount > 0); }

    template<typename T>
    NODISCARD FORCEINLINE T* DataAs() { return (T*)m_Data; }
    template<typename T>
    NODISCARD FORCEINLINE const T* DataAs() const { return (const T*)m_Data; }

public:
    NODISCARD FORCEINLINE ReadonlyBufferView ToView() const { return ReadonlyBufferView(*this); }

    NODISCARD FORCEINLINE VectorView<uint8> ToVectorView() { return VectorView<uint8>(Bytes(), m_ByteCount); }
    NODISCARD FORCEINLINE VectorView<const uint8> ToVectorView() const { return VectorView<const uint8>(Bytes(), m_ByteCount); }

public:
    SHOOTER_API void SetByteCount(usize byteCount);
    SHOOTER_API void EnsureByteCount(usize byteCount);

    SHOOTER_API void SetByteCountWithoutCopy(usize byteCount);
    SHOOTER_API void EnsureByteCountWithoutCopy(usize byteCount);

    SHOOTER_API void Release();

private:
    void* m_Data;
    usize m_ByteCount;
};

//////////////////////////////////////////////////////////////////////////////
// The following constructors are declared in the 'BufferView.h', but their //
// implementation is in this file in order to avoid circular includes.      //
//////////////////////////////////////////////////////////////////////////////

FORCEINLINE ReadonlyBufferView::ReadonlyBufferView(const Buffer& buffer)
    : m_Data(buffer.Data())
    , m_ByteCount(buffer.ByteCount())
{}

FORCEINLINE ReadonlyBufferView& ReadonlyBufferView::operator=(const Buffer& buffer)
{
    m_Data = buffer.Data();
    m_ByteCount = buffer.ByteCount();
    return *this;
}

FORCEINLINE BufferView::BufferView(Buffer& buffer)
    : m_Data(buffer.Data())
    , m_ByteCount(buffer.ByteCount())
{}

FORCEINLINE BufferView& BufferView::operator=(Buffer& buffer)
{
    m_Data = buffer.Data();
    m_ByteCount = buffer.ByteCount();
    return *this;
}

}
