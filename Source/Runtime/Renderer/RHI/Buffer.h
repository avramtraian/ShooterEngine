// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/ShaderResource.h>

namespace SE
{

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// VERTEX BUFFER. /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

struct VertexBufferInfo
{
public:
    usize             BufferSize      { 0 };
    const void*       InitialData     { nullptr };
    usize             InitialDataSize { 0 };

public:
    inline VertexBufferInfo& SetBufferSize  (usize bufferSize)                               { BufferSize = bufferSize;                                      return *this; }
    inline VertexBufferInfo& SetInitialData (const void* initialData, usize initialDataSize) { InitialData = initialData; InitialDataSize = initialDataSize; return *this; }
};

class VertexBuffer : public RefCounted
{
    SE_MAKE_RENDERER_RHI_INTERFACE(VertexBuffer);

public:
    virtual void UploadDataImmediately(const void* verticesData, usize verticesDataOffset, usize verticesDataSize) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// INDEX BUFFER. /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

enum IndexBufferDataType : uint8
{
    Unknown = 0,
    UInt16,
    UInt32,
};

struct IndexBufferInfo
{
public:
    IndexBufferDataType DataType          { IndexBufferDataType::Unknown };
    uint32              IndexCount        { 0 };
    const void*         InitialIndices    { nullptr };
    uint32              InitialIndexCount { 0 };

public:
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

    virtual void UploadDataImmediately(const void* indices, uint32 indexOffset, uint32 indexCount) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// UNIFORM BUFFER. ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

struct UniformBufferInfo
{
public:
    usize              BufferSize      { 0 };
    const void*        InitialData     { nullptr };
    usize              InitialDataSize { 0 };

public:
    inline UniformBufferInfo& SetBufferSize  (usize bufferSize)                               { BufferSize = bufferSize;                                      return *this; }
    inline UniformBufferInfo& SetInitialData (const void* initialData, usize initialDataSize) { InitialData = initialData; InitialDataSize = initialDataSize; return *this; }
};

struct UniformBuffer : public ShaderResource
{
    SE_MAKE_RENDERER_RHI_INTERFACE(UniformBuffer);

public:
    virtual void UploadDataImmediately(const void* bufferData, usize bufferDataOffset, usize bufferDataSize) = 0;
};

}
