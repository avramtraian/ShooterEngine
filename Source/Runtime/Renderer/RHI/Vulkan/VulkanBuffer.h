// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/Buffer.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanCore.h>

namespace SE
{

class VulkanVertexBuffer : public VertexBuffer
{
public:
    VulkanVertexBuffer(const VertexBufferInfo& info);
    virtual ~VulkanVertexBuffer() override;

public:
    NODISCARD FORCEINLINE VkBuffer GetHandle() const { return m_Handle; }

private:
    VkBuffer m_Handle;
    VkDeviceMemory m_MemoryHandle;
    usize m_BufferSize;
};

class VulkanIndexBuffer : public IndexBuffer
{
public:
    VulkanIndexBuffer(const IndexBufferInfo& info);
    virtual ~VulkanIndexBuffer() override;

    NODISCARD FORCEINLINE virtual IndexBufferDataType GetDataType() const override { return m_DataType; }
    NODISCARD FORCEINLINE virtual uint32 GetIndexCount() const override { return m_IndexCount; }

public:
    NODISCARD FORCEINLINE VkBuffer GetHandle() const { return m_Handle; }

private:
    VkBuffer m_Handle;
    VkDeviceMemory m_MemoryHandle;
    IndexBufferDataType m_DataType;
    uint32 m_IndexCount;
};

}
