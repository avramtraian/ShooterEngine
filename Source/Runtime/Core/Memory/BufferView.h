// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/VectorView.h>

namespace SE
{

// Forward declaration.
class Buffer;

class ReadonlyBufferView
{
public:
    FORCEINLINE constexpr ReadonlyBufferView()
        : m_Data(nullptr)
        , m_ByteCount(0)
    {}

    FORCEINLINE constexpr ReadonlyBufferView(const ReadonlyBufferView& other)
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {}

    FORCEINLINE constexpr ReadonlyBufferView(ReadonlyBufferView&& other) noexcept
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {
        other.m_Data = nullptr;
        other.m_ByteCount = 0;
    }

    FORCEINLINE constexpr ReadonlyBufferView& operator=(const ReadonlyBufferView& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_Data = other.m_Data;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    FORCEINLINE constexpr ReadonlyBufferView& operator=(ReadonlyBufferView&& other) noexcept
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

public:
    // These functions are implemented in the 'Buffer.h' file, in order to avoid circular includes.
    FORCEINLINE ReadonlyBufferView(const Buffer& buffer);
    FORCEINLINE ReadonlyBufferView& operator=(const Buffer& buffer);

public:
    NODISCARD FORCEINLINE const void* Data() const { return m_Data; }
    NODISCARD FORCEINLINE const uint8* Bytes() const { return (const uint8*)m_Data; }

    NODISCARD FORCEINLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD FORCEINLINE bool IsEmpty() const { return (m_ByteCount == 0); }
    NODISCARD FORCEINLINE bool HasData() const { return (m_ByteCount > 0); }

    template<typename T>
    NODISCARD FORCEINLINE const T* DataAs() const { return (const T*)m_Data; }

    NODISCARD FORCEINLINE VectorView<const uint8> ToVectorView() const { return VectorView<const uint8>(Bytes(), m_ByteCount); }

private:
    const void* m_Data;
    usize m_ByteCount;
};

class BufferView
{
public:
    FORCEINLINE constexpr BufferView()
        : m_Data(nullptr)
        , m_ByteCount(0)
    {}

    FORCEINLINE constexpr BufferView(const BufferView& other)
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {}

    FORCEINLINE constexpr BufferView(BufferView&& other) noexcept
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {
        other.m_Data = nullptr;
        other.m_ByteCount = 0;
    }

    FORCEINLINE constexpr BufferView& operator=(const BufferView& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_Data = other.m_Data;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    FORCEINLINE constexpr BufferView& operator=(BufferView&& other) noexcept
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

public:
    // These functions are implemented in the 'Buffer.h' file, in order to avoid circular includes.
    FORCEINLINE BufferView(Buffer& buffer);
    FORCEINLINE BufferView& operator=(Buffer& buffer);

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

    NODISCARD FORCEINLINE VectorView<uint8> ToVectorView() { return VectorView<uint8>(Bytes(), m_ByteCount); }
    NODISCARD FORCEINLINE VectorView<const uint8> ToVectorView() const { return VectorView<const uint8>(Bytes(), m_ByteCount); }

private:
    void* m_Data;
    usize m_ByteCount;
};

}
