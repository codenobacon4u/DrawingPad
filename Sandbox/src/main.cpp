#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <DrawingPad.h>
#include <fstream>
#include <chrono>
#include <iostream>
#include <glm/glm/gtx/string_cast.hpp>

API Curr_API = API::Vulkan;

static std::string vertSrc = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;
 
layout(location = 0) out vec3 fragColor;


void main() {
	gl_Position = vec4(inPos, 0.0, 1.0);
	fragColor = inColor;
}
)";

static std::string fragSrc = R"(
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(fragColor, 1.0);
}
)";

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

void UpdateUniformBuffer(uint32_t index, uint32_t width, uint32_t height)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), width / (float) height, 0.1f, 10.0f);
	ubo.proj[1][1] += -1;
}

void printMat(glm::mat4 mat)
{
	int i, j;
	for (j = 0; j < 4; j++) {
		for (i = 0; i < 4; i++) {
			printf("%f ", mat[i][j]);
		}
		printf("\n");
	}
}

int main() {
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	GraphicsDevice* gd;
	GraphicsContext* ctx;
	Swapchain* swap;
	Buffer* vb;
	Buffer* ib;
	Buffer* ub;
	Pipeline* pipeline;
	Shader* vertShader;
	Shader* fragShader;
	if (!glfwInit())
		printf("Failed");
	if (Curr_API != API::OpenGL)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "DrawingPad Test", NULL, NULL);
	if (window == NULL) {
		printf("Window is NULL!\n");
		std::abort();
	}

	gd = GraphicsDevice::Create(window, Curr_API);
	GraphicsContextDesc desc = {};
	desc.ContextID = 0;
	desc.Name = "Immediate";
	ctx = gd->CreateContext(desc);
	SwapchainDesc swapSpec;
	swap = gd->CreateSwapchain(swapSpec, ctx, window);

	const std::vector<Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	BufferDesc bufDesc = {};
	bufDesc.Usage = BufferUsageFlags::Default;
	// ======== Create Vertex Buffer ========
	bufDesc.BindFlags = BufferBindFlags::Vertex;
	bufDesc.Size = sizeof(vertices[0]) * vertices.size();
	vb = gd->CreateBuffer(bufDesc, (void*)vertices.data());
	{
		BufferDesc sDesc = {}; 
		sDesc.Usage = BufferUsageFlags::Staging;
		sDesc.BindFlags = BufferBindFlags::Staging;
		sDesc.Size = bufDesc.Size;
		Buffer *sb = gd->CreateBuffer(sDesc, (void*)vertices.data());
		sb->BeginStaging();
		ctx->CopyBuffer(sb, vb, sDesc.Size);
		sb->EndStaging();
		delete sb;
	}
	// ======== Create Index Buffer ========
	bufDesc.BindFlags = BufferBindFlags::Index;
	bufDesc.Size = sizeof(indices[0]) * indices.size();
	ib = gd->CreateBuffer(bufDesc, (void*)indices.data());
	{
		BufferDesc sDesc = {};
		sDesc.Usage = BufferUsageFlags::Staging;
		sDesc.BindFlags = BufferBindFlags::Staging;
		sDesc.Size = bufDesc.Size;
		Buffer* sb = gd->CreateBuffer(sDesc, (void*)indices.data());
		sb->BeginStaging();
		ctx->CopyBuffer(sb, ib, sDesc.Size);
		sb->EndStaging();
		delete sb;
	}
	// ======== Create Uniform Buffer ========
	bufDesc.BindFlags = BufferBindFlags::Uniform;
	bufDesc.Size = sizeof(UniformBufferObject) * 3;
	bufDesc.Buffered = true;
	ub = gd->CreateBuffer(bufDesc, nullptr);
	// ======== Create Shaders ========
	ShaderDesc sDesc = {};
	sDesc.EntryPoint = "main";
	sDesc.Name = "Basic Vert";
	//sDesc.Src = vertSrc;
	sDesc.Path = "shaders/ubo.vert";
	sDesc.Type = ShaderType::Vertex;
	vertShader = gd->CreateShader(sDesc);
	sDesc.Name = "Basic Frag";
	//sDesc.Src = fragSrc;
	sDesc.Path = "shaders/ubo.frag";
	sDesc.Type = ShaderType::Fragment;
	fragShader = gd->CreateShader(sDesc);

	LayoutElement vertInputs[]{
		{
			0, // InputIndex Location
			0, // BufferSlot Binding
			2, // Num Components
			offsetof(Vertex, pos), // Offset
			sizeof(Vertex) // Stride
		},
		{
			1, // InputIndex Location
			0, // BufferSlot Binding
			3, // Num Components
			offsetof(Vertex, color),  // Offset
			sizeof(Vertex) // Stride
		}
	};

	GraphicsPipelineDesc pDesc = {};
	pDesc.NumViewports = 1;
	pDesc.NumColors = 1;
	pDesc.ColorFormats[0] = swap->GetDesc().ColorFormat;
	pDesc.DepthFormat = TextureFormat::Unknown;
	pDesc.InputLayout.NumElements = 2;
	pDesc.InputLayout.Elements = vertInputs;
	pDesc.ShaderCount = 2;
	pDesc.Shaders[0] = vertShader;
	pDesc.Shaders[1] = fragShader;
	pipeline = gd->CreateGraphicsPipeline(pDesc);

	//Texture* texture = gd->CreateTexture();
	
	auto glfwVersion = glfwGetVersionString();
	if (Curr_API == API::OpenGL) {
		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		auto glVersion = glGetString(GL_VERSION);
		auto glRenderer = glGetString(GL_RENDERER);
		auto glVendor = glGetString(GL_VENDOR);
		printf("GLFW Version: %s\nGL Version: %s\nGL Renderer: %s\nGL Vendor: %s\n", glfwVersion, glVersion, glRenderer, glVendor);
	}

	DrawAttribs drawAttribs = {};
	drawAttribs.VrtIdxCount = indices.size();
	drawAttribs.InstanceCount = 1;
	drawAttribs.FirstVrtIdx = 0;
	drawAttribs.FirstInstance = 0;

	while (!glfwWindowShouldClose(window))
	{
		//====UPDATE====
		glfwPollEvents();

		//====RENDER====
		uint32_t i = swap->GetNextBackbuffer().first;
		TextureView* rtv = swap->GetNextBackbuffer().second;
		TextureView* dsv = swap->GetDepthBufferView();
		float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		ctx->Begin(i);
		ctx->SetRenderTargets(1, &rtv, nullptr);
		ctx->ClearColor(rtv, nullptr);
		//ctx->ClearDepth(dsv, ClearDepthStencil::Depth, 1, 0);
		ctx->SetPipeline(pipeline);
		Buffer* vertexBuffs[] = { vb };
		uint32_t offsets[] = { 0 };
		ctx->SetVertexBuffers(0, 1, vertexBuffs, offsets);
		ctx->SetIndexBuffer(ib, 0);
		
		{
			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			UniformBufferObject ubo = {};
			ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 10.0f);
			ubo.proj[1][1] *= -1;
			ctx->UploadBuffer(ub, i * sizeof(ubo), sizeof(ubo), &ubo);
		}
		ctx->SetShaderResource(ResourceBindingType::UniformBuffer, 0, 0, ub);
		ctx->DrawIndexed(drawAttribs);
		ctx->Flush();

		//====PRESENT====
		swap->Present(0);
	}
	gd->WaitForIdle();
	delete pipeline;
	delete swap;
	delete ctx;
	delete ib;
	delete vb;
	delete ub;
	delete fragShader;
	delete vertShader;
	delete gd;

	glfwDestroyWindow(window);
	glfwTerminate();
}
