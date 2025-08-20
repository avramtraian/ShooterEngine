// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

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
    NODISCARD FORCEINLINE VkBuffer GetHandle() const { return m_BufferHandle; }
    NODISCARD FORCEINLINE VkDeviceMemory GetMemory() const { return m_BufferMemory; }
    NODISCARD FORCEINLINE usize GetSize() const { return m_BufferSize; }

public:
    void* Map(usize offset, usize size);
    void Unmap();

private:
    VkBuffer m_BufferHandle { VK_NULL_HANDLE };
    VkDeviceMemory m_BufferMemory { VK_NULL_HANDLE };
    usize m_BufferSize { 0 };
};

class VulkanVertexBuffer : public VertexBuffer
{
public:
    VulkanVertexBuffer(const VertexBufferInfo& info);
    virtual ~VulkanVertexBuffer() override = default;

public:
    NODISCARD FORCEINLINE VkBuffer GetHandle() const { return m_Buffer.GetHandle(); }

private:
    VulkanBuffer m_Buffer;
};

class VulkanIndexBuffer : public IndexBuffer
{
public:
    VulkanIndexBuffer(const IndexBufferInfo& info);
    virtual ~VulkanIndexBuffer() override = default;

    NODISCARD FORCEINLINE virtual IndexBufferDataType GetDataType() const override { return m_DataType; }
    NODISCARD FORCEINLINE virtual uint32 GetIndexCount() const override { return m_IndexCount; }

public:
    NODISCARD FORCEINLINE VkBuffer GetHandle() const { return m_Buffer.GetHandle(); }

private:
    VulkanBuffer m_Buffer;
    IndexBufferDataType m_DataType;
    uint32 m_IndexCount;
};

class VulkanUniformBuffer : public UniformBuffer
{
public:
    VulkanUniformBuffer(const UniformBufferInfo& info);
    virtual ~VulkanUniformBuffer() override = default;

public:
    NODISCARD FORCEINLINE VkBuffer GetHandle() const { return m_Buffer.GetHandle(); }
    NODISCARD FORCEINLINE usize GetBufferSize() const { return m_Buffer.GetSize(); }

public:
    VulkanBuffer m_Buffer;
};

}
