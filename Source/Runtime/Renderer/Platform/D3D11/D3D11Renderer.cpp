/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Log.h>
#include <Renderer/Platform/D3D11/D3D11Framebuffer.h>
#include <Renderer/Platform/D3D11/D3D11IndexBuffer.h>
#include <Renderer/Platform/D3D11/D3D11Pipeline.h>
#include <Renderer/Platform/D3D11/D3D11RenderPass.h>
#include <Renderer/Platform/D3D11/D3D11Renderer.h>
#include <Renderer/Platform/D3D11/D3D11Shader.h>
#include <Renderer/Platform/D3D11/D3D11VertexBuffer.h>
#include <Renderer/Renderer.h>
#include <comdef.h>
#include <system_error>

namespace SE
{

struct D3D11RendererData
{
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* device_context = nullptr;
    IDXGIFactory2* dxgi_factory = nullptr;
    D3D_FEATURE_LEVEL device_feature_level;

    RefPtr<D3D11RenderPass> active_render_pass;
};

static OwnPtr<D3D11RendererData> s_d3d11_renderer;

bool D3D11Renderer::initialize()
{
    if (s_d3d11_renderer.is_valid())
        return false;
    s_d3d11_renderer = create_own<D3D11RendererData>();

    UINT device_flags = 0;
#if SE_CONFIGURATION_DEBUG
    device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // SE_CONFIGURATION_DEBUG

    const D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    const HRESULT device_result = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        device_flags,
        feature_levels,
        SE_ARRAY_COUNT(feature_levels),
        D3D11_SDK_VERSION,
        &s_d3d11_renderer->device,
        &s_d3d11_renderer->device_feature_level,
        &s_d3d11_renderer->device_context
    );

    if (device_result != S_OK)
    {
        SE_LOG_TAG_ERROR("D3D11", "Failed to create the device!");
        return false;
    }

    const HRESULT dxgi_factory_result = CreateDXGIFactory1(IID_PPV_ARGS(&s_d3d11_renderer->dxgi_factory));
    if (dxgi_factory_result != S_OK)
    {
        SE_LOG_TAG_ERROR("D3D11", "Failed to create the DXGI factory!");
        return false;
    }

    return true;
}

void D3D11Renderer::shutdown()
{
    if (!s_d3d11_renderer.is_valid())
        return;

    s_d3d11_renderer->dxgi_factory->Release();
    s_d3d11_renderer->dxgi_factory = nullptr;

    s_d3d11_renderer->device_context->Release();
    s_d3d11_renderer->device_context = nullptr;

#if SE_CONFIGURATION_DEBUG
    ID3D11Debug* debug_interface;
    SE_D3D11_CHECK(s_d3d11_renderer->device->QueryInterface(IID_PPV_ARGS(&debug_interface)));
    debug_interface->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
    debug_interface->Release();
    debug_interface = nullptr;
#endif // SE_CONFIGURATION_DEBUG

    s_d3d11_renderer->device->Release();
    s_d3d11_renderer->device = nullptr;

    s_d3d11_renderer.release();
}

String D3D11Renderer::get_error_message(HRESULT result)
{
    std::string error_message = std::system_category().message(result);
    return StringView::create_from_utf8(error_message.c_str(), error_message.size() - 1);
}

ID3D11Device* D3D11Renderer::get_device()
{
    SE_DEBUG_ASSERT(s_d3d11_renderer->device != nullptr);
    return s_d3d11_renderer->device;
}

ID3D11DeviceContext* D3D11Renderer::get_device_context()
{
    SE_DEBUG_ASSERT(s_d3d11_renderer->device_context != nullptr);
    return s_d3d11_renderer->device_context;
}

IDXGIFactory2* D3D11Renderer::get_dxgi_factory()
{
    SE_DEBUG_ASSERT(s_d3d11_renderer->dxgi_factory != nullptr);
    return s_d3d11_renderer->dxgi_factory;
}

D3D11RenderingContext* D3D11Renderer::get_active_context()
{
    RenderingContext* active_context = Renderer::get_active_context();
    SE_DEBUG_ASSERT(active_context);
    return static_cast<D3D11RenderingContext*>(active_context);
}

void D3D11Renderer::on_resize(u32 new_width, u32 new_height)
{
    s_d3d11_renderer->device_context->Flush();
    s_d3d11_renderer->device_context->ClearState();
}

void D3D11Renderer::present(RenderingContext* rendering_context)
{
    D3D11RenderingContext* context = static_cast<D3D11RenderingContext*>(rendering_context);
    context->get_swapchain()->Present(1, 0);
}

void D3D11Renderer::begin_render_pass(RefPtr<RenderPass> render_pass)
{
    // Another render pass is already active.
    SE_ASSERT(!s_d3d11_renderer->active_render_pass.is_valid());
    s_d3d11_renderer->active_render_pass = render_pass.as<D3D11RenderPass>();

    RefPtr<D3D11RenderPass> d3d11_render_pass = render_pass.as<D3D11RenderPass>();
    RefPtr<D3D11Pipeline> pipeline = d3d11_render_pass->get_pipeline();
    RefPtr<D3D11Shader> shader = pipeline->get_shader().as<D3D11Shader>();
    RefPtr<D3D11Framebuffer> framebuffer = d3d11_render_pass->get_target_framebuffer();

    //
    // Set pipeline input layout.
    //

    D3D11_PRIMITIVE_TOPOLOGY primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    switch (pipeline->get_primitive_topology())
    {
        case PipelinePrimitiveTopology::TriangleList: primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
    }

    s_d3d11_renderer->device_context->IASetInputLayout(pipeline->get_input_layout());
    s_d3d11_renderer->device_context->IASetPrimitiveTopology(primitive_topology);

    //
    // Set shaders.
    //

    const Vector<D3D11Shader::ShaderModule>& shader_modules = shader->get_shader_modules();
    for (const D3D11Shader::ShaderModule& shader_module : shader_modules)
    {
        switch (shader_module.stage)
        {
            case ShaderStage::Vertex:
            {
                ID3D11VertexShader* vertex_shader = static_cast<ID3D11VertexShader*>(shader_module.handle);
                get_device_context()->VSSetShader(vertex_shader, nullptr, 0);
            }
            break;

            case ShaderStage::Fragment:
            {
                ID3D11PixelShader* fragment_shader = static_cast<ID3D11PixelShader*>(shader_module.handle);
                get_device_context()->PSSetShader(fragment_shader, nullptr, 0);
            }
            break;
        }
    }

    //
    // Set the viewport and the rasterizer state.
    //

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (FLOAT)(framebuffer->get_width());
    viewport.Height = (FLOAT)(framebuffer->get_height());
    viewport.MinDepth = 0.0F;
    viewport.MaxDepth = 1.0F;

    s_d3d11_renderer->device_context->RSSetViewports(1, &viewport);
    s_d3d11_renderer->device_context->RSSetState(pipeline->get_rasterizer_state());

    //
    // Set render targets.
    //

    Vector<ID3D11RenderTargetView*> render_target_views;
    render_target_views.set_fixed_capacity(framebuffer->get_attachment_count());
    for (u32 attachment_index = 0; attachment_index < framebuffer->get_attachment_count(); ++attachment_index)
    {
        void* attachment_view_handle = framebuffer->get_attachment_image_view(attachment_index);
        render_target_views.add(static_cast<ID3D11RenderTargetView*>(attachment_view_handle));
    }

    s_d3d11_renderer->device_context->OMSetRenderTargets((UINT)(render_target_views.count()), render_target_views.elements(), nullptr);

    //
    // Clear the render targets (if required).
    //

    for (u32 attachment_index = 0; attachment_index < framebuffer->get_attachment_count(); ++attachment_index)
    {
        const RenderPassAttachmentDescription& attachment_description = d3d11_render_pass->get_attachment_description(attachment_index);
        if (attachment_description.load_operation == RenderPassAttachmentLoadOperation::Clear)
        {
            void* attachment_view_handle = framebuffer->get_attachment_image_view(attachment_index);
            ID3D11RenderTargetView* rtv = static_cast<ID3D11RenderTargetView*>(attachment_view_handle);

            const Color4 color = attachment_description.clear_color;
            const FLOAT clear_color[4] = { color.r, color.g, color.b, color.a };
            s_d3d11_renderer->device_context->ClearRenderTargetView(rtv, clear_color);
        }
    }
}

void D3D11Renderer::end_render_pass()
{
    SE_ASSERT(s_d3d11_renderer->active_render_pass.is_valid());
    // NOTE: Ending the render pass does nothing when using the D3D11 renderer API.
    s_d3d11_renderer->active_render_pass.release();
}

void D3D11Renderer::draw_indexed(RefPtr<VertexBuffer> vertex_buffer, RefPtr<IndexBuffer> index_buffer, u32 index_count)
{
    // A render pass must be active.
    SE_ASSERT(s_d3d11_renderer->active_render_pass.is_valid());

    RefPtr<D3D11VertexBuffer> d3d11_vertex_buffer = vertex_buffer.as<D3D11VertexBuffer>();
    RefPtr<D3D11IndexBuffer> d3d11_index_buffer = index_buffer.as<D3D11IndexBuffer>();

    // Bind the vertex buffer.
    const u32 vertex_stride = s_d3d11_renderer->active_render_pass->get_pipeline()->get_vertex_stride();
    ID3D11Buffer* vertex_buffers[1] = { d3d11_vertex_buffer->get_handle() };
    UINT strides[1] = { vertex_stride };
    UINT offsets[1] = { 0 };

    s_d3d11_renderer->device_context->IASetVertexBuffers(0, SE_ARRAY_COUNT(vertex_buffers), vertex_buffers, strides, offsets);

    DXGI_FORMAT index_format = DXGI_FORMAT_UNKNOWN;
    switch (d3d11_index_buffer->get_index_type())
    {
        case IndexType::UInt16: index_format = DXGI_FORMAT_R16_UINT; break;
        case IndexType::UInt32: index_format = DXGI_FORMAT_R32_UINT; break;
    }

    // Bind the index buffer.
    s_d3d11_renderer->device_context->IASetIndexBuffer(d3d11_index_buffer->get_handle(), index_format, 0);

    // Draw.
    s_d3d11_renderer->device_context->DrawIndexed(index_count, 0, 0);
}

} // namespace SE
