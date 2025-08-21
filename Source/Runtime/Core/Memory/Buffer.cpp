// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Core/Memory/Buffer.h>
#include <Runtime/Core/Memory/MemoryOperations.h>

namespace SE
{

Buffer Buffer::Copy(const Buffer& sourceBuffer)
{
    // Forward the implementation to the other 'Copy' function.
    return Buffer::Copy(sourceBuffer.ToView());
}

Buffer Buffer::Copy(ReadonlyBufferView sourceBuffer)
{
    Buffer destinationBuffer;
    destinationBuffer.SetByteCount(sourceBuffer.ByteCount());
    MemoryCopy(destinationBuffer.Data(), sourceBuffer.Data(), sourceBuffer.ByteCount());
    return destinationBuffer;
}

Buffer::Buffer()
    : m_Data(nullptr)
    , m_ByteCount(0)
{}

Buffer::~Buffer()
{
    Release();
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_Data(other.m_Data)
    , m_ByteCount(other.m_ByteCount)
{
    other.m_Data = nullptr;
    other.m_ByteCount = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    // Handle the self-assignment case.
    if (this == &other)
        return *this;

    m_Data = other.m_Data;
    m_ByteCount = other.m_ByteCount;
    other.m_Data = nullptr;
    other.m_ByteCount = 0;
    return *this;
}

void Buffer::SetByteCount(usize byteCount)
{
    if (byteCount == m_ByteCount)
        return;

    void* newData = ::operator new(byteCount);
    usize copyByteCount = m_ByteCount;
    if (byteCount < copyByteCount)
        copyByteCount = byteCount;
    MemoryCopy(newData, m_Data, copyByteCount);

    Release();
    m_Data = newData;
    m_ByteCount = byteCount;
}

void Buffer::EnsureByteCount(usize byteCount)
{
    if (byteCount <= m_ByteCount)
        return;
    SetByteCount(byteCount);
}

void Buffer::SetByteCountWithoutCopy(usize byteCount)
{
    if (byteCount == m_ByteCount)
        return;

    Release();
    SetByteCount(byteCount);
}

void Buffer::EnsureByteCountWithoutCopy(usize byteCount)
{
    if (byteCount <= m_ByteCount)
        return;
    SetByteCountWithoutCopy(byteCount);
}

void Buffer::Release()
{
    ::operator delete(m_Data);
    m_Data = nullptr;
    m_ByteCount = 0;
}

}
