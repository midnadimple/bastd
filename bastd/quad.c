// quad batch renderer
// currently requires texture atlas bcs gpuweb sucks and won't add texture arrays
// TODO Textures
#ifndef BASTD_QUAD_C
#define BASTD_QUAD_C

#include "../bastd.c"
#include "math.c"
#include "webgpu.h"

#define quad_WEBGPU_STR(str) (WGPUStringView) { .data = str, .length = sizeof(str) - 1 }

FUNCTION WGPUStringView
quad__S8_toWGPUStringView(S8 s)
{
	return (WGPUStringView) { .data = s.raw, .length = s.len };
}

FUNCTION S8
quad__S8_fromWGPUStringView(WGPUStringView s)
{
	return (S8) { .len = s.length, .raw = s.data };
}

CALLBACK_EXPORT void 
quad__onDeviceError(WGPUDevice const * device, WGPUErrorType type, WGPUStringView msg, void* userdata1, void* userdata2)
{
	os_errorBoxGui(quad__S8_fromWGPUStringView(msg));
}

FUNCTION void
quad__onAdapterRequestEnded(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView msg, void *userdata1, void *userdata2)
{
    if (status != WGPURequestAdapterStatus_Success)
    {
        // cannot find adapter?
		os_errorBoxGui(quad__S8_fromWGPUStringView(msg));
    }
    else
    {
		// use first adapter provided
		WGPUAdapter* result = userdata1;
		if (*result == NIL)
		{
			*result = adapter;
		}
    }
}

FUNCTION void
quad__onDeviceRequestEnded(WGPURequestDeviceStatus status, WGPUDevice adapter, WGPUStringView msg, void *userdata1, void *userdata2)
{
    if (status != WGPURequestDeviceStatus_Success)
    {
        // cannot find adapter?
		os_errorBoxGui(quad__S8_fromWGPUStringView(msg));
    }
    else
    {
		// use first adapter provided
		WGPUDevice* result = userdata1;
		if (*result == NIL)
		{
			*result = adapter;
		}
    }
}

#define quad_Renderer_TEXFORMAAT WGPUTextureFormat_BGRA8Unorm

typedef F32 quad_Color[4];

typedef struct quad_Vertex quad_Vertex;
struct quad_Vertex {
	math_Vec2 xy;
	quad_Color col;
	math_Vec2 uv;
};
FUNCTION quad_Vertex
quad_V(math_Vec2 xy, math_Vec2 uv, quad_Color color)
{
	quad_Vertex v = {0};
	v.xy = xy;
	v.col[0] = color[0];
	v.col[1] = color[1];
	v.col[2] = color[2];
	v.col[3] = color[3];
	v.uv = uv;
	return v;
}

#define quad_MAX_QUADS KILO(8)
#define quad_MAX_VERTS quad_MAX_QUADS * 4
#define quad_MAX_INDICES quad_MAX_QUADS * 6

typedef struct quad_Renderer quad_Renderer;
struct quad_Renderer {
	os_Window *window;
	math_Mat4 mvp;
	quad_Vertex vertices[quad_MAX_VERTS];
	U32 vertex_count;
	
	U32 indices[quad_MAX_INDICES];
	U32 index_count;
	
	WGPUInstance instance;
	WGPUSurface surface;
	U32 surface_width, surface_height;
	B32 surface_configured;
	WGPUAdapter adapter;
	WGPUDevice device;
	WGPUQueue queue;
	WGPUBuffer vbuffer;
	WGPUBuffer ibuffer;
	WGPUBuffer ubuffer;
	WGPUTexture tex;
	WGPUTextureView tex_view;
	WGPUSampler tex_sampler;
    WGPURenderPipeline pipeline;
	WGPUBindGroup bind_group;
};

FUNCTION void
quad_Renderer_setTexture(quad_Renderer *renderer, void *pixels, WGPUExtent2D size)
{
	WGPUTextureDescriptor tex_desc = {0};
	tex_desc.dimension = WGPUTextureDimension_2D;
	tex_desc.size = (WGPUExtent3D){size.width, size.height, 1};
	tex_desc.format = quad_Renderer_TEXFORMAAT;
	tex_desc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
	tex_desc.sampleCount = 1;
	tex_desc.mipLevelCount = 1;
	renderer->tex = wgpuDeviceCreateTexture(renderer->device, &tex_desc);

	WGPUTexelCopyTextureInfo dest = {0};
	dest.texture = renderer->tex;
	dest.mipLevel = 0;
	dest.origin = (WGPUOrigin3D){0};
	dest.aspect = WGPUTextureAspect_All;
	
	U64 data_size = size.width * size.height * 4 * sizeof(U8); // 4 channels

	WGPUTexelCopyBufferLayout data_layout = {0};
	data_layout.offset = 0;
	data_layout.bytesPerRow  = size.width * 4 * sizeof(U8);
	data_layout.rowsPerImage = size.height;

	WGPUExtent3D extents = {0};
	extents.width = size.width;
	extents.height = size.height;
	extents.depthOrArrayLayers = 1;
	
	wgpuQueueWriteTexture(renderer->queue, &dest, pixels, data_size, &data_layout, &extents);
	
	// Texture View
	WGPUTextureViewDescriptor tex_view_desc = {0};
	tex_view_desc.label = quad_WEBGPU_STR("src_view");
	tex_view_desc.aspect = WGPUTextureAspect_All;
	tex_view_desc.baseMipLevel = 0;
	tex_view_desc.mipLevelCount = 1;
	tex_view_desc.dimension = WGPUTextureViewDimension_2D;
	tex_view_desc.baseArrayLayer  = 0;
	tex_view_desc.arrayLayerCount = 1;
	renderer->tex_view = wgpuTextureCreateView(renderer->tex, &tex_view_desc);
}

FUNCTION quad_Renderer*
quad_Renderer_create(os_Window *window, m_Allocator *perm)
{
	quad_Renderer *renderer = m_MAKE(quad_Renderer, 1, perm);
	renderer->window = window;

	// Create instance
	{
		WGPUInstanceDescriptor desc = {0};
		renderer->instance = wgpuCreateInstance(&desc);
		ASSERT(renderer->instance, "Failed to create WebGPU instance");
	}

	// Create surface
	{
		WGPUSurfaceDescriptor surface_desc = os_Window_getWGPUSurfaceDesc(*window);
		renderer->surface = wgpuInstanceCreateSurface(renderer->instance, &surface_desc);
		ASSERT(renderer->surface, "Failed to create WebGPU instance");
	}

	// Create Adapter
	{
		WGPURequestAdapterOptions options = {0};
		options.compatibleSurface = renderer->surface;
		wgpuInstanceRequestAdapter(renderer->instance, &options, (WGPURequestAdapterCallbackInfo){
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = &quad__onAdapterRequestEnded,
			.userdata1 = &renderer->adapter,
		});
		ASSERT(renderer->adapter, "Failed to get WebGPU adapter");
	}

	// Create Device
    {
        WGPUDeviceDescriptor desc = {0};
        desc.uncapturedErrorCallbackInfo.callback = &quad__onDeviceError;

        wgpuAdapterRequestDevice(renderer->adapter, &desc, (WGPURequestDeviceCallbackInfo){
			.mode = WGPUCallbackMode_AllowSpontaneous,
			.callback = quad__onDeviceRequestEnded,
			.userdata1 = &renderer->device
		});
        ASSERT(renderer->device, "Failed to create WebGPU device");

		renderer->queue = wgpuDeviceGetQueue(renderer->device);
    }

	// Create Shaders
	WGPUShaderModule shaders;
	{
		#include "quad.wgsl.inl"

		WGPUShaderSourceWGSL src = {0};
		src.chain.sType = WGPUSType_ShaderSourceWGSL;
		src.code = quad_WEBGPU_STR(wgsl);

		WGPUShaderModuleDescriptor desc = {0};
        desc.nextInChain = &src.chain;

		shaders = wgpuDeviceCreateShaderModule(renderer->device, &desc);
	}

	// Vertices
    {
        WGPUBufferDescriptor desc = {0};
        desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
        desc.size = sizeof(renderer->vertices);
        renderer->vbuffer = wgpuDeviceCreateBuffer(renderer->device, &desc);
    }

	// Indicies
	{
		WGPUBufferDescriptor desc = {0};
		desc.usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst;
		desc.size = sizeof(renderer->indices);
		renderer->ibuffer = wgpuDeviceCreateBuffer(renderer->device, &desc);
	}

	// Uniforms
    {
		WGPUBufferDescriptor desc = {0};
        desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        desc.size = 4 * 4 * sizeof(float); // 4x4 matrix
        renderer->ubuffer = wgpuDeviceCreateBuffer(renderer->device, &desc);
	}

	// Sampler
	{
		WGPUSamplerDescriptor desc = {0};
		desc.addressModeU = WGPUAddressMode_ClampToEdge;
		desc.addressModeV = WGPUAddressMode_ClampToEdge;
		desc.addressModeW = WGPUAddressMode_ClampToEdge;
		desc.magFilter = WGPUFilterMode_Nearest;
		desc.minFilter = WGPUFilterMode_Nearest;
		desc.mipmapFilter = WGPUMipmapFilterMode_Nearest;
		desc.lodMinClamp = 0.0f;
		desc.lodMaxClamp = 1.0f;
		desc.compare = WGPUCompareFunction_Undefined;
		desc.maxAnisotropy = 1;
		renderer->tex_sampler = wgpuDeviceCreateSampler(renderer->device, &desc);
	}

	// Texture
	{
		U32 pixels[] = { 0xffffffff }; // single white pixel
		WGPUExtent2D size = {0};
		size.width = 1;
		size.height = 1;
		quad_Renderer_setTexture(renderer, &pixels[0], size);
	}

	// Pipeline
	{
		WGPUBindGroupLayoutEntry bind_group_layout_entries[3] = {0};

		// MVP for Vertex
		bind_group_layout_entries[0].binding = 0;
		bind_group_layout_entries[0].visibility = WGPUShaderStage_Vertex;
		bind_group_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
		bind_group_layout_entries[0].buffer.minBindingSize = 4 * 4 * sizeof(float);

		// Texture for Fragment
		bind_group_layout_entries[1].binding = 1;
		bind_group_layout_entries[1].visibility = WGPUShaderStage_Fragment;
		bind_group_layout_entries[1].texture.sampleType = WGPUTextureSampleType_Float;
		bind_group_layout_entries[1].texture.viewDimension = WGPUTextureViewDimension_2D;
		bind_group_layout_entries[1].texture.multisampled = FALSE;

		// Sampler for Fragment
		bind_group_layout_entries[2].binding = 2;
		bind_group_layout_entries[2].visibility = WGPUShaderStage_Fragment;
		bind_group_layout_entries[2].sampler.type = WGPUSamplerBindingType_Filtering;

		WGPUBindGroupLayoutDescriptor bind_group_layout_desc = {0};
        bind_group_layout_desc.entryCount = 3,
        bind_group_layout_desc.entries = &bind_group_layout_entries[0];
		WGPUBindGroupLayout bind_group_layout = wgpuDeviceCreateBindGroupLayout(renderer->device, &bind_group_layout_desc);

		WGPUPipelineLayoutDescriptor pipeline_layout_desc = {0};
		pipeline_layout_desc.bindGroupLayoutCount = 1,
        pipeline_layout_desc.bindGroupLayouts = &bind_group_layout;
        WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(renderer->device, &pipeline_layout_desc);
    
		WGPURenderPipelineDescriptor pipeline_desc = {0};
        pipeline_desc.layout = pipeline_layout;
        // draw triangle list
        pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
		pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
		pipeline_desc.primitive.cullMode = WGPUCullMode_None;

        // vertex shader
		WGPUVertexAttribute vertex_attribs[3] = {0};
		vertex_attribs[0].format = WGPUVertexFormat_Float32x2;
		vertex_attribs[0].offset = offsetof(quad_Vertex, xy);
		vertex_attribs[0].shaderLocation = 0;
		vertex_attribs[1].format = WGPUVertexFormat_Float32x4;
		vertex_attribs[1].offset = offsetof(quad_Vertex, col);
		vertex_attribs[1].shaderLocation = 1;
		vertex_attribs[2].format = WGPUVertexFormat_Float32x2;
		vertex_attribs[2].offset = offsetof(quad_Vertex, uv);
		vertex_attribs[2].shaderLocation = 2;

		WGPUVertexBufferLayout vertex_buffer_layout = {0};
		vertex_buffer_layout.arrayStride = sizeof(quad_Vertex);
		vertex_buffer_layout.stepMode = WGPUVertexStepMode_Vertex;
		vertex_buffer_layout.attributeCount = 3;
		vertex_buffer_layout.attributes = &vertex_attribs[0];

		pipeline_desc.vertex.module = shaders;
		pipeline_desc.vertex.entryPoint = quad_WEBGPU_STR("vs");
		pipeline_desc.vertex.bufferCount = 1;
		pipeline_desc.vertex.buffers = &vertex_buffer_layout;

        // fragment shader
		WGPUBlendState blend_state = {0};
		blend_state.color.operation = WGPUBlendOperation_Add;
		blend_state.color.srcFactor = WGPUBlendFactor_SrcAlpha;
		blend_state.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
		blend_state.alpha.operation = WGPUBlendOperation_Add;
		blend_state.alpha.srcFactor = WGPUBlendFactor_SrcAlpha;
		blend_state.alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;

		WGPUColorTargetState color_target_state = {0};
		color_target_state.format = quad_Renderer_TEXFORMAAT;
		color_target_state.blend = &blend_state;
		color_target_state.writeMask = WGPUColorWriteMask_All;

		WGPUFragmentState frag_state = {0};
		frag_state.module = shaders;
		frag_state.entryPoint = quad_WEBGPU_STR("fs");
		frag_state.targetCount = 1;
		frag_state.targets = &color_target_state;

		pipeline_desc.fragment = &frag_state;

		pipeline_desc.multisample.count = 1;
		pipeline_desc.multisample.mask = 0xffffffff;
		pipeline_desc.multisample.alphaToCoverageEnabled = FALSE;

		renderer->pipeline = wgpuDeviceCreateRenderPipeline(renderer->device, &pipeline_desc);
		// wgpuPipelineLayoutRelease(pipeline_layout);

		WGPUBindGroupEntry bind_group_entries[3] = {0};

		// MVP for Vertex
		bind_group_entries[0].binding = 0;
		bind_group_entries[0].buffer = renderer->ubuffer;
		bind_group_entries[0].offset = 0;
		bind_group_entries[0].size = 4 * 4 * sizeof(float);

		// Texture View for Fragment
		bind_group_entries[1].binding = 1;
		bind_group_entries[1].textureView = renderer->tex_view;

		// Sampler for Fragment
		bind_group_entries[2].binding = 2;
		bind_group_entries[2].sampler = renderer->tex_sampler;

		WGPUBindGroupDescriptor bind_group_desc = {0};
        bind_group_desc.layout = bind_group_layout;
		bind_group_desc.entryCount = 3,
        bind_group_desc.entries = &bind_group_entries[0];
        renderer->bind_group = wgpuDeviceCreateBindGroup(renderer->device, &bind_group_desc);
	}

	wgpuAdapterRelease(renderer->adapter);

	return renderer;
}

FUNCTION void
quad_Renderer_begin(quad_Renderer *renderer)
{
	renderer->vertex_count = 0;
	renderer->index_count = 0;
	
	U32 pixels[] = { 0xffffffff }; // single white pixel
	WGPUExtent2D size = {0};
	size.width = 1;
	size.height = 1;
	quad_Renderer_setTexture(renderer, &pixels[0], size);
}

FUNCTION void
quad_Renderer_end(quad_Renderer *renderer)
{
	wgpuInstanceProcessEvents(renderer->instance);

	// Configure Surface
	if (!renderer->surface_configured || renderer->surface_width != renderer->window->width || renderer->surface_height != renderer->window->height)
	{
		if (renderer->surface_configured)
		{
			// release old swap chain
			wgpuSurfaceUnconfigure(renderer->surface);
			renderer->surface_configured = FALSE;
		}

		// resize to new size for non-zero window size
		if (renderer->window->width != 0 && renderer->window->height != 0)
		{
			WGPUSurfaceConfiguration config =
			{
				.nextInChain = NIL,
				.device = renderer->device,
				.format = WGPUTextureFormat_BGRA8Unorm,
				.width = renderer->window->width,
				.height = renderer->window->height,
				.usage = WGPUTextureUsage_RenderAttachment,
				.presentMode = WGPUPresentMode_Fifo, // WGPUPresentMode_Mailbox // WGPUPresentMode_Immediate
				.alphaMode = WGPUCompositeAlphaMode_Auto,
			};
			wgpuSurfaceConfigure(renderer->surface, &config);
			renderer->surface_configured = TRUE;
		}

		renderer->surface_width = renderer->window->width;
		renderer->surface_height = renderer->window->height;
	}
	
	// Rendering
	if (renderer->surface_configured) {
		// Sending the data
        wgpuQueueWriteBuffer(renderer->queue, renderer->vbuffer, 0, renderer->vertices, sizeof(renderer->vertices));
		wgpuQueueWriteBuffer(renderer->queue, renderer->ibuffer, 0, renderer->indices, sizeof(renderer->indices));
				
		// Camera
		renderer->mvp = math_M4D(1);
		renderer->mvp = math_MulM4(renderer->mvp, math_Orthographic_LH_ZO(0.0f, (F32)renderer->window->width, (F32)renderer->window->height, 0.0f, 0.1f, 100.0f));
		renderer->mvp = math_MulM4(renderer->mvp, math_Translate(math_V3(0, 0, 0)));
		renderer->mvp = math_MulM4(renderer->mvp, math_QToM4(math_QFromAxisAngle_LH(math_V3(0, 0, 1), 0)));
		renderer->mvp = math_MulM4(renderer->mvp, math_Scale(math_V3(1, 1, 1)));
		wgpuQueueWriteBuffer(renderer->queue, renderer->ubuffer, 0, &renderer->mvp.Elements, 4 * 4 * sizeof(float));

		WGPUSurfaceTexture surface_tex;
		wgpuSurfaceGetCurrentTexture(renderer->surface, &surface_tex);
		if (surface_tex.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
		{
			os_errorBoxGui(S8("Test"));
			return;
		}

		WGPUTextureViewDescriptor view_desc = {
			.nextInChain = NIL,
			.label = "Surface texture view",
			.format = quad_Renderer_TEXFORMAAT,
			.dimension = WGPUTextureViewDimension_2D,
			.baseMipLevel = 0,
			.mipLevelCount = 1,
			.baseArrayLayer = 0,
			.arrayLayerCount = 1,
			.aspect = WGPUTextureAspect_All,
		};
		WGPUTextureView target_view = wgpuTextureCreateView(surface_tex.texture, &view_desc);
		ASSERT(target_view, "Failed to get WGPU Target View");

		WGPURenderPassDescriptor desc =
		{
			.colorAttachmentCount = 1,
			.colorAttachments = (WGPURenderPassColorAttachment[])
			{
				// one output to write to, initially cleared with background color
				{
					.view = target_view,
					.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
					.loadOp = WGPULoadOp_Clear,
					.storeOp = WGPUStoreOp_Store,
					.clearValue = { 0.0, 0.0, 0.0, 1.0 }, // r,g,b,a
				},
			},
		};

		WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(renderer->device, NULL);
		WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &desc);
		{
			wgpuRenderPassEncoderSetViewport(pass, 0.f, 0.f, (float)renderer->surface_width, (float)renderer->surface_height, 0.f, 1.f);

			// draw the triangle using 3 vertices in vertex buffer
			wgpuRenderPassEncoderSetPipeline(pass, renderer->pipeline);
			wgpuRenderPassEncoderSetVertexBuffer(pass, 0, renderer->vbuffer, 0, WGPU_WHOLE_SIZE);
			wgpuRenderPassEncoderSetIndexBuffer(pass, renderer->ibuffer, WGPUIndexFormat_Uint32, 0, WGPU_WHOLE_SIZE);
			wgpuRenderPassEncoderSetBindGroup(pass, 0, renderer->bind_group, 0, NIL);
			wgpuRenderPassEncoderDrawIndexed(pass, renderer->index_count, 1, 0, 0, 0);
		}
		wgpuRenderPassEncoderEnd(pass);
		WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, NULL);
		wgpuQueueSubmit(renderer->queue, 1, &command);

		wgpuCommandBufferRelease(command);
		wgpuCommandEncoderRelease(encoder);
		
		wgpuTextureViewRelease(target_view);

		// clean up textures
		wgpuTextureViewRelease(renderer->tex_view);
		wgpuTextureDestroy(renderer->tex);
		wgpuTextureRelease(renderer->tex);
	}

	wgpuSurfacePresent(renderer->surface);

	// // Surface Configuration
	// if (!renderer->surface_configured || renderer->surface_width != renderer->window->width || renderer->surface_height != renderer->window->height) {
	// 	if (renderer->surface_configured) {
	// 		wgpuSurfaceUnconfigure(renderer->surface);
	// 		renderer->surface_configured = FALSE;
	// 	}

	// 	if (renderer->surface_width != 0 && renderer->surface_height != 0) {
	// 		WGPUSurfaceConfiguration config =
	// 		{
	// 			.device = renderer->device,
	// 			.format = quad_Renderer_TEXFORMAAT,
	// 			.usage = WGPUTextureUsage_RenderAttachment,
	// 			.width = renderer->surface_width,
	// 			.height = renderer->surface_height,
	// 			.presentMode = WGPUPresentMode_Fifo, // WGPUPresentMode_Mailbox // WGPUPresentMode_Immediate
	// 		};
	// 		wgpuSurfaceConfigure(renderer->surface, &config);

	// 		renderer->surface_configured = TRUE;
	// 	}
	// }
}

FUNCTION void
quad_Renderer_pushQuad(quad_Renderer *renderer, math_Vec2 min_xy, math_Vec2 max_xy, math_Vec2 min_uv, math_Vec2 max_uv, quad_Color tint)
{
	if (renderer->vertex_count == quad_MAX_VERTS || renderer->index_count == quad_MAX_INDICES) {
		quad_Renderer_end(renderer);
		quad_Renderer_begin(renderer);
	}

	renderer->vertices[renderer->vertex_count] = quad_V(min_xy, min_uv, tint);
	renderer->vertices[renderer->vertex_count + 1] = quad_V(math_V2(max_xy.X, min_xy.Y), math_V2(max_uv.U, min_uv.V), tint);
	renderer->vertices[renderer->vertex_count + 2] = quad_V(math_V2(min_xy.X, max_xy.Y), math_V2(min_uv.U, max_uv.V), tint);
	renderer->vertices[renderer->vertex_count + 3] = quad_V(max_xy, max_uv, tint);
	
	renderer->indices[renderer->index_count] = 0 + renderer->vertex_count;
	renderer->indices[renderer->index_count + 1] = 1 + renderer->vertex_count;
	renderer->indices[renderer->index_count + 2] = 2 + renderer->vertex_count;
	renderer->indices[renderer->index_count + 3] = 1 + renderer->vertex_count;
	renderer->indices[renderer->index_count + 4] = 2 + renderer->vertex_count;
	renderer->indices[renderer->index_count + 5] = 3 + renderer->vertex_count;

	renderer->vertex_count += 4;
	renderer->index_count += 6;
}
#endif//BASTD_QUAD_C