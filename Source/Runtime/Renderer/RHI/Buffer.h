// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>

namespace SE
{

enum class VertexBufferUsage : uint8
{
    Normal,
    Static,
    Dynamic,
};

struct VertexBufferInfo
{
public:
    VertexBufferUsage Usage { VertexBufferUsage::Normal };
    usize BufferSize        { 0 };
    const void* InitialData { nullptr };
    usize InitialDataSize   { 0 };

public:
    inline VertexBufferInfo& SetUsage       (VertexBufferUsage usage)                        { Usage = usage;                                                return *this; }
    inline VertexBufferInfo& SetBufferSize  (usize bufferSize)                               { BufferSize = bufferSize;                                      return *this; }
    inline VertexBufferInfo& SetInitialData (const void* initialData, usize initialDataSize) { InitialData = initialData; InitialDataSize = initialDataSize; return *this; }
};

class VertexBuffer : public RefCounted
{
    SE_MAKE_RENDERER_RHI_INTERFACE(VertexBuffer);
};

enum IndexBufferUsage : uint8
{
    Normal,
    Static,
    Dynamic,
};

enum IndexBufferDataType : uint8
{
    Unknown = 0,
    UInt16,
    UInt32,
};

struct IndexBufferInfo
{
public:
    IndexBufferUsage Usage       { IndexBufferUsage::Normal };
    IndexBufferDataType DataType { IndexBufferDataType::Unknown };
    uint32 IndexCount            { 0 };
    const void* InitialIndices   { nullptr };
    uint32 InitialIndexCount     { 0 };

public:
    inline IndexBufferInfo& SetUsage          (IndexBufferUsage usage)                 { Usage = usage;                                            return *this; }
    inline IndexBufferInfo& SetDataType       (IndexBufferDataType dataType)           { DataType = dataType;                                      return *this; }
    inline IndexBufferInfo& SetIndexCount     (uint32 indexCount)                      { IndexCount = indexCount;                                  return *this; }
    inline IndexBufferInfo& SetInitialIndices (const void* indices, uint32 indexCount) { InitialIndices = indices; InitialIndexCount = indexCount; return *this; }
};

class IndexBuffer : public RefCounted
{
    SE_MAKE_RENDERER_RHI_INTERFACE(IndexBuffer);

public:
    NODISCARD virtual IndexBufferDataType GetDataType() const = 0;
    NODISCARD virtual uint32 GetIndexCount() const = 0;
};

}
