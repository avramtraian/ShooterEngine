// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{


//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// GENERIC BUFFER DECLARATION. //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanBuffer
{
    SE_MAKE_NONCOPYABLE(VulkanBuffer);
    SE_MAKE_NONMOVABLE(VulkanBuffer);

public:
    VulkanBuffer() = default;
    ~VulkanBuffer() { Release(); }

    void Invalidate(usize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags);
    void Release();

public:
    NODISCARD ALWAYS_INLINE VkBuffer GetHandle() const { return m_BufferHandle; }
    NODISCARD ALWAYS_INLINE VkDeviceMemory GetMemory() const { return m_BufferMemory; }
    NODISCARD ALWAYS_INLINE usize GetSize() const { return m_BufferSize; }

public:
    void* Map(usize offset, usize size);
    void Unmap();

private:
    VkBuffer m_BufferHandle { VK_NULL_HANDLE };
    VkDeviceMemory m_BufferMemory { VK_NULL_HANDLE };
    usize m_BufferSize { 0 };
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// VERTEX BUFFER DECLARATION. ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanVertexBuffer : public VertexBuffer
{
public:
    VulkanVertexBuffer(const VertexBufferInfo& info);
    virtual ~VulkanVertexBuffer() override = default;

    virtual void UploadDataImmediately(const void* verticesData, usize verticesDataOffset, usize verticesDataSize) override;

public:
    NODISCARD ALWAYS_INLINE VkBuffer GetHandle() const { return m_Buffer.GetHandle(); }

private:
    VulkanBuffer m_Buffer;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// INDEX BUFFER DECLARATION. ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanIndexBuffer : public IndexBuffer
{
public:
    VulkanIndexBuffer(const IndexBufferInfo& info);
    virtual ~VulkanIndexBuffer() override = default;

    NODISCARD ALWAYS_INLINE virtual IndexBufferDataType GetDataType() const override { return m_DataType; }
    NODISCARD ALWAYS_INLINE virtual uint32 GetIndexCount() const override { return m_IndexCount; }

    virtual void UploadDataImmediately(const void* indices, uint32 indexOffset, uint32 indexCount) override;

public:
    NODISCARD ALWAYS_INLINE VkBuffer GetHandle() const { return m_Buffer.GetHandle(); }

private:
    VulkanBuffer m_Buffer;
    IndexBufferDataType m_DataType;
    uint32 m_IndexCount;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// UNIFORM BUFFER DECLARATION. //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VulkanUniformBuffer : public UniformBuffer
{
public:
    VulkanUniformBuffer(const UniformBufferInfo& info);
    virtual ~VulkanUniformBuffer() override;

    virtual void UploadDataImmediately(const void* bufferData, usize bufferDataOffset, usize bufferDataSize) override;

public:
    NODISCARD ALWAYS_INLINE VkBuffer GetHandle() const { return m_Buffer.GetHandle(); }
    NODISCARD ALWAYS_INLINE usize GetBufferSize() const { return m_Buffer.GetSize(); }

public:
    VulkanBuffer m_Buffer;
};

}
