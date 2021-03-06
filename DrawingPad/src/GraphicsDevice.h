#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "GraphicsContext.h"

#include "CommandList.h"
#include "Framebuffer.h"
#include "Swapchain.h"
#include "Shader.h"

enum class API {
	None = 0, OpenGL = 1, Vulkan = 2, DirectX = 3
};

class GraphicsDevice 
{
public:
	void SwapBuffers();
	
	//virtual void Submit(const CommandList* cb) = 0;
	virtual void WaitForIdle() = 0;
	virtual void Present() = 0;

	//virtual CommandList* CreateCommandList() = 0;
	virtual Buffer* CreateBuffer(const BufferDesc& desc, void* data) = 0;
	virtual Texture* CreateTexture(const TextureDesc& desc, const TextureData* data) = 0;
	virtual RenderPass* CreateRenderPass(const RenderPassDesc& desc) = 0;
	virtual Framebuffer* CreateFramebuffer(const FramebufferDesc& desc) = 0;
	virtual Pipeline* CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
	virtual Pipeline* CreateComputePipeline(const ComputePipelineDesc& desc) = 0;
	virtual Swapchain* CreateSwapchain(const SwapchainDesc& desc, GLFWwindow* window) = 0;
	virtual Shader* CreateShader(const ShaderDesc& desc) = 0;

	virtual void OnWindowResize(int width, int height)
	{
		m_MainSwap->Resize(width, height);
	}

	GraphicsContext* CreateContext(const GraphicsContextDesc& desc);

	Framebuffer* GetSwapchainFramebuffer() { return m_SwapFB; }

	

	static GraphicsDevice* Create(GLFWwindow* window, API api);

	inline static API GetAPI() { return s_API; }
protected:
	virtual void SwapBuffers(Swapchain* swapchain) const = 0;
	Framebuffer* m_SwapFB;
	Swapchain* m_MainSwap;

	std::vector<GraphicsContext*> m_ImmediateContexts;

private:
	static API s_API;
};