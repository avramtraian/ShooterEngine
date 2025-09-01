// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/Renderer/RHI/Vulkan/VulkanPipeline.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanRenderingDriver.h>
#include <Runtime/Renderer/RHI/Vulkan/VulkanShader.h>

namespace SE
{

static uint32 GetVertexInputAttributeSize(GraphicsVertexInputAttributeType type)
{
    switch (type)
    {
        case GraphicsVertexInputAttributeType::Float:  return 4 * 1;
        case GraphicsVertexInputAttributeType::Float2: return 4 * 2;
        case GraphicsVertexInputAttributeType::Float3: return 4 * 3;
        case GraphicsVertexInputAttributeType::Float4: return 4 * 4;

        case GraphicsVertexInputAttributeType::Int:  return 4 * 1;
        case GraphicsVertexInputAttributeType::Int2: return 4 * 2;
        case GraphicsVertexInputAttributeType::Int3: return 4 * 3;
        case GraphicsVertexInputAttributeType::Int4: return 4 * 4;

        case GraphicsVertexInputAttributeType::UInt:  return 4 * 1;
        case GraphicsVertexInputAttributeType::UInt2: return 4 * 2;
        case GraphicsVertexInputAttributeType::UInt3: return 4 * 3;
        case GraphicsVertexInputAttributeType::UInt4: return 4 * 4;
    }

    SE_ASSERT_NOT_REACHED;
    return 0;
}

static VkFormat GetVertexInputAttributeFormat(GraphicsVertexInputAttributeType type)
{
    switch (type)
    {
        case GraphicsVertexInputAttributeType::Float:  return VK_FORMAT_R32_SFLOAT;
        case GraphicsVertexInputAttributeType::Float2: return VK_FORMAT_R32G32_SFLOAT;
        case GraphicsVertexInputAttributeType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
        case GraphicsVertexInputAttributeType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;

        case GraphicsVertexInputAttributeType::Int:  return VK_FORMAT_R32_SINT;
        case GraphicsVertexInputAttributeType::Int2: return VK_FORMAT_R32G32_SINT;
        case GraphicsVertexInputAttributeType::Int3: return VK_FORMAT_R32G32B32_SINT;
        case GraphicsVertexInputAttributeType::Int4: return VK_FORMAT_R32G32B32A32_SINT;

        case GraphicsVertexInputAttributeType::UInt:  return VK_FORMAT_R32_UINT;
        case GraphicsVertexInputAttributeType::UInt2: return VK_FORMAT_R32G32_UINT;
        case GraphicsVertexInputAttributeType::UInt3: return VK_FORMAT_R32G32B32_UINT;
        case GraphicsVertexInputAttributeType::UInt4: return VK_FORMAT_R32G32B32A32_UINT;
    }

    SE_ASSERT_NOT_REACHED;
    return VK_FORMAT_UNDEFINED;
}

VulkanPipeline::VulkanPipeline(const WeakRefPtr<VulkanRenderPass>& parentRenderPass)
    : m_Handle(VK_NULL_HANDLE)
    , m_ParentRenderPass(parentRenderPass)
{}

VulkanPipeline::~VulkanPipeline()
{
    Destroy();
}

VkPipeline VulkanPipeline::GetHandle() const
{
    if (!IsLocked())
    {
        SE_LOG_ERROR("Trying to get the handle of a Vulkan pipeline that is not locked!");
        SE_ASSERT_NOT_REACHED;
        return VK_NULL_HANDLE;
    }

    return m_Handle;
}

const GraphicsState& VulkanPipeline::GetGraphicsState() const
{
    if (!IsLocked())
    {
        SE_LOG_ERROR("Trying to get the graphics state of a Vulkan pipeline that is not locked!");
        SE_ASSERT_NOT_REACHED;
    }

    return m_GraphicsState;
}

RefPtr<VulkanShader> VulkanPipeline::GetShader() const
{
    if (!IsLocked())
    {
        SE_LOG_ERROR("Trying to get the shader of a Vulkan pipeline that is not locked!");
        SE_ASSERT_NOT_REACHED;
        return {};
    }

    return m_LockedShader;
}

void VulkanPipeline::Invalidate(const GraphicsState& graphicsState, const RefPtr<Shader>& shader, VkRenderPass renderPassHandle, uint32 colorAttachmentCount)
{
    if (IsLocked())
    {
        SE_LOG_ERROR("Trying to invalidate a Vulkan pipeline that is locked!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

    // Destroy the old pipeline.
    Destroy();

    // Set the new graphics state.
    m_GraphicsState = graphicsState;

    // Set the new shader.
    SE_ASSERT(shader.IsValid());
    m_Shader = shader.As<VulkanShader>();
    m_ShaderPreDestroyCallback = m_Shader->AddCallback(RHIObjectCallbackType::PreDestroy,
        [this](RHIObject& object)
        {
            SE_ASSERT(IsUnlocked());
            Destroy();
        }
    );

    Vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.EnsureCapacity(m_Shader->GetModuleCount());

    for (const VulkanShader::Module& module : m_Shader->GetModules())
    {
        VkPipelineShaderStageCreateInfo& stageCreateInfo = shaderStages.Emplace();
        stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageCreateInfo.stage = module.Stage;
        stageCreateInfo.module = module.Handle;
        stageCreateInfo.pName = module.EntryPoint.Characters();
        stageCreateInfo.pSpecializationInfo = nullptr;
    }

    Vector<VkVertexInputAttributeDescription> vertexAttributes;
    vertexAttributes.EnsureCapacity(m_GraphicsState.VertexInputLayout.Attributes.Count());

    uint32 vertexInputStride = 0;
    uint32 attributeLocation = 0;

    for (const auto& attribute : m_GraphicsState.VertexInputLayout.Attributes)
    {
        VkVertexInputAttributeDescription& attributeDescription = vertexAttributes.Emplace();
        attributeDescription.location = attributeLocation;
        attributeDescription.binding = 0;
        attributeDescription.format = GetVertexInputAttributeFormat(attribute.Type);
        attributeDescription.offset = vertexInputStride;

        const uint32 attributeSize = GetVertexInputAttributeSize(attribute.Type);
        vertexInputStride += attributeSize;
        attributeLocation++;
    }

    Vector<VkVertexInputBindingDescription> vertexBindings;
    vertexBindings.SetCountDefaulted(1);

    vertexBindings[0].binding = 0;
    vertexBindings[0].stride = vertexInputStride;
    vertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = (uint32)vertexBindings.Count();
    vertexInputState.pVertexBindingDescriptions = vertexBindings.Elements();
    vertexInputState.vertexAttributeDescriptionCount = (uint32)vertexAttributes.Count();
    vertexInputState.pVertexAttributeDescriptions = vertexAttributes.Elements();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    switch (m_GraphicsState.Topology)
    {
        case GraphicsTopology::PointList:                inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; break;
        case GraphicsTopology::LineList:                 inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST; break;
        case GraphicsTopology::LineStrip:                inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; break;
        case GraphicsTopology::TriangleList:             inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; break;
        case GraphicsTopology::TriangleStrip:            inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP; break;
        case GraphicsTopology::TriangleFan:              inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN; break;
        case GraphicsTopology::LineListWithAdjaceny:     inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY; break;
        case GraphicsTopology::LineStripWithAdjaceny:    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY; break;
        case GraphicsTopology::TriangleListWithdjaceny:  inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY; break;
        case GraphicsTopology::TriangleStripWithdjaceny: inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    VkPipelineTessellationStateCreateInfo tessellationState = {};
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    VkViewport defaultViewport = {};
    defaultViewport.x = 0;
    defaultViewport.y = 0;
    defaultViewport.width = 0;
    defaultViewport.height = 0;

    VkRect2D defaultScissor = {};
    defaultScissor.offset.x = 0;
    defaultScissor.offset.y = 0;
    defaultScissor.extent.width = 0;
    defaultScissor.extent.height = 0;

    // NOTE(Traian): The viewport state is created as dynamic and thus it is correctly set when the render pass begins,
    // as the target framebuffer is known and its dimensions are available.
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &defaultViewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &defaultScissor;

    VkPipelineRasterizationStateCreateInfo rasterizationState = {};
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;

    switch (m_GraphicsState.CullMode)
    {
        case GraphicsCullMode::Disabled: rasterizationState.cullMode = VK_CULL_MODE_NONE; break;
        case GraphicsCullMode::Back:     rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT; break;
        case GraphicsCullMode::Front:    rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    switch (m_GraphicsState.FrontFace)
    {
        case GraphicsFrontFace::Clockwise:        rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE; break;
        case GraphicsFrontFace::CounterClockwise: rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; break;
        default: SE_ASSERT_NOT_REACHED;
    }

    VkPipelineMultisampleStateCreateInfo multisampleState = {};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = m_GraphicsState.HasDepthAttachment ? VK_TRUE : VK_FALSE;
    depthStencilState.depthWriteEnable = m_GraphicsState.HasDepthAttachment ? VK_TRUE : VK_FALSE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;

    Vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.EnsureCapacity(colorAttachmentCount);

    for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; ++colorAttachmentIndex)
    {
        VkPipelineColorBlendAttachmentState& colorBlendAttachment = colorBlendAttachments.Emplace();
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    }

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.attachmentCount = (uint32)colorBlendAttachments.Count();
    colorBlendState.pAttachments = colorBlendAttachments.Elements();

    Vector<VkDynamicState> dynamicStates;
    dynamicStates.Add(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.Add(VK_DYNAMIC_STATE_SCISSOR);
    dynamicStates.Add(VK_DYNAMIC_STATE_LINE_WIDTH);

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = (uint32)dynamicStates.Count();
    dynamicState.pDynamicStates = dynamicStates.Elements();

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = (uint32)shaderStages.Count();
    graphicsPipelineCreateInfo.pStages = shaderStages.Elements();
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputState;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    graphicsPipelineCreateInfo.pTessellationState = &tessellationState;
    graphicsPipelineCreateInfo.pViewportState = &viewportState;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilState;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicState;
    graphicsPipelineCreateInfo.layout = m_Shader->GetPipelineLayout();
    graphicsPipelineCreateInfo.renderPass = renderPassHandle;
    graphicsPipelineCreateInfo.subpass = 0;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    // Create the graphics pipeline object.
    SE_VULKAN_CHECK(vkCreateGraphicsPipelines(g_VulkanDriver->GetDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &m_Handle));
}

void VulkanPipeline::Destroy()
{
    if (IsLocked())
    {
        SE_LOG_ERROR("Trying to destroy a Vulkan pipeline that is locked!");
        SE_ASSERT_NOT_REACHED;
        return;
    }

    // Release the held weak-reference of the shader.
    m_Shader.Release();
    m_ShaderPreDestroyCallback.Release();

    // Destroy the graphics pipeline object.
    vkDestroyPipeline(g_VulkanDriver->GetDevice(), m_Handle, nullptr);
    m_Handle = VK_NULL_HANDLE;

    // Invalidate the graphics state.
    m_GraphicsState = {};
}

bool VulkanPipeline::IsCompatibleWithGraphicsStateAndShader(const GraphicsState& graphicsState, const RefPtr<Shader>& shader) const
{
    // Check if the shaders are the same.
    if (m_Shader != shader)
        return false;
    
    // Check if the vertex input layouts are the same.
    {
        const auto& thisVertexAttributes = m_GraphicsState.VertexInputLayout.Attributes;
        const auto& otherVertexAttributes = graphicsState.VertexInputLayout.Attributes;
        if (thisVertexAttributes.Count() != otherVertexAttributes.Count())
            return false;
        for (uint32 attributeIndex = 0; attributeIndex < thisVertexAttributes.Count(); ++attributeIndex)
        {
            const GraphicsVertexInputAttribute& thisAttribute = thisVertexAttributes[attributeIndex];
            const GraphicsVertexInputAttribute& otherAttribute = otherVertexAttributes[attributeIndex];
    
            if (thisAttribute.Name != otherAttribute.Name) { return false; }
            if (thisAttribute.Type != otherAttribute.Type) { return false; }
        }
    }

    // Check other pipeline creation flags.
    if (m_GraphicsState.Topology             != graphicsState.Topology)             { return false; }
    if (m_GraphicsState.CullMode             != graphicsState.CullMode)             { return false; }
    if (m_GraphicsState.FrontFace            != graphicsState.FrontFace)            { return false; }
    if (m_GraphicsState.HasDepthAttachment   != graphicsState.HasDepthAttachment)   { return false; }
    if (m_GraphicsState.HasStencilAttachment != graphicsState.HasStencilAttachment) { return false; }
    if (m_GraphicsState.EnableBlending       != graphicsState.EnableBlending)       { return false; }

    return true;
}

void VulkanPipeline::OnLock()
{
    // Acquire strong reference for the parent render pass.
    SE_ASSERT(m_ParentRenderPass.IsValid());
    m_LockedParentRenderPass = m_ParentRenderPass;

    // Acquire strong reference for the used shader.
    SE_ASSERT(m_Shader.IsValid());
    m_LockedShader = m_Shader;
}

void VulkanPipeline::OnUnlock()
{
    // Release the strong reference for the used shader.
    m_LockedShader.Release();

    // Release the strong reference for the parent render pass.
    m_LockedParentRenderPass.Release();
}

}
