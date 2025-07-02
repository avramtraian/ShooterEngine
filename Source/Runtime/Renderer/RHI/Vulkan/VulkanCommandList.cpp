// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanCommandList.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanTexture.h>

namespace SE
{

VulkanCommandList::VulkanCommandList(const CommandListInfo& info)
    : m_ActiveRenderPass(VK_NULL_HANDLE)
    , m_ActiveFramebuffer(VK_NULL_HANDLE)
{}

VulkanCommandList::~VulkanCommandList()
{}

static inline VkAttachmentLoadOp AttachmentLoadOpToVulkan(AttachmentLoadOp loadOp)
{
    switch (loadOp)
    {
        case AttachmentLoadOp::Load:     return VK_ATTACHMENT_LOAD_OP_LOAD;
        case AttachmentLoadOp::Clear:    return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case AttachmentLoadOp::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    SE_ASSERT(!"Invalid AttachmentLoadOp!");
    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

static inline VkAttachmentStoreOp AttachmentStoreOpToVulkan(AttachmentStoreOp storeOp)
{
    switch (storeOp)
    {
        case AttachmentStoreOp::Store:    return VK_ATTACHMENT_STORE_OP_STORE;
        case AttachmentStoreOp::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }

    SE_ASSERT(!"Invalid AttachmentLoadOp!");
    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

void VulkanCommandList::BeginRenderPass(const RenderPassInfo& renderPassInfo)
{
}

void VulkanCommandList::EndRenderPass()
{
}

}
