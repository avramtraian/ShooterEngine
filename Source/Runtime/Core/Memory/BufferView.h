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
    ALWAYS_INLINE constexpr ReadonlyBufferView()
        : m_Data(nullptr)
        , m_ByteCount(0)
    {}

    ALWAYS_INLINE constexpr ReadonlyBufferView(const ReadonlyBufferView& other)
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {}

    ALWAYS_INLINE constexpr ReadonlyBufferView(ReadonlyBufferView&& other) noexcept
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {
        other.m_Data = nullptr;
        other.m_ByteCount = 0;
    }

    ALWAYS_INLINE constexpr ReadonlyBufferView& operator=(const ReadonlyBufferView& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_Data = other.m_Data;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    ALWAYS_INLINE constexpr ReadonlyBufferView& operator=(ReadonlyBufferView&& other) noexcept
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
    ALWAYS_INLINE ReadonlyBufferView(const Buffer& buffer);
    ALWAYS_INLINE ReadonlyBufferView& operator=(const Buffer& buffer);

public:
    NODISCARD ALWAYS_INLINE const void* Data() const { return m_Data; }
    NODISCARD ALWAYS_INLINE const uint8* Bytes() const { return (const uint8*)m_Data; }

    NODISCARD ALWAYS_INLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD ALWAYS_INLINE bool IsEmpty() const { return (m_ByteCount == 0); }
    NODISCARD ALWAYS_INLINE bool HasData() const { return (m_ByteCount > 0); }

    template<typename T>
    NODISCARD ALWAYS_INLINE const T* DataAs() const { return (const T*)m_Data; }

    NODISCARD ALWAYS_INLINE VectorView<const uint8> ToVectorView() const { return VectorView<const uint8>(Bytes(), m_ByteCount); }

private:
    const void* m_Data;
    usize m_ByteCount;
};

class BufferView
{
public:
    ALWAYS_INLINE constexpr BufferView()
        : m_Data(nullptr)
        , m_ByteCount(0)
    {}

    ALWAYS_INLINE constexpr BufferView(const BufferView& other)
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {}

    ALWAYS_INLINE constexpr BufferView(BufferView&& other) noexcept
        : m_Data(other.m_Data)
        , m_ByteCount(other.m_ByteCount)
    {
        other.m_Data = nullptr;
        other.m_ByteCount = 0;
    }

    ALWAYS_INLINE constexpr BufferView& operator=(const BufferView& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_Data = other.m_Data;
        m_ByteCount = other.m_ByteCount;
        return *this;
    }

    ALWAYS_INLINE constexpr BufferView& operator=(BufferView&& other) noexcept
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
    ALWAYS_INLINE BufferView(Buffer& buffer);
    ALWAYS_INLINE BufferView& operator=(Buffer& buffer);

public:
    NODISCARD ALWAYS_INLINE void* Data() { return m_Data; }
    NODISCARD ALWAYS_INLINE const void* Data() const { return m_Data; }

    NODISCARD ALWAYS_INLINE uint8* Bytes() { return (uint8*)m_Data; }
    NODISCARD ALWAYS_INLINE const uint8* Bytes() const { return (const uint8*)m_Data; }

    NODISCARD ALWAYS_INLINE usize ByteCount() const { return m_ByteCount; }
    NODISCARD ALWAYS_INLINE bool IsEmpty() const { return (m_ByteCount == 0); }
    NODISCARD ALWAYS_INLINE bool HasData() const { return (m_ByteCount > 0); }

    template<typename T>
    NODISCARD ALWAYS_INLINE T* DataAs() { return (T*)m_Data; }
    template<typename T>
    NODISCARD ALWAYS_INLINE const T* DataAs() const { return (const T*)m_Data; }

    NODISCARD ALWAYS_INLINE VectorView<uint8> ToVectorView() { return VectorView<uint8>(Bytes(), m_ByteCount); }
    NODISCARD ALWAYS_INLINE VectorView<const uint8> ToVectorView() const { return VectorView<const uint8>(Bytes(), m_ByteCount); }

private:
    void* m_Data;
    usize m_ByteCount;
};

}
