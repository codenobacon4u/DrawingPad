#include "pwpch.h"
#include "GraphicsDevice.h"

#include "OpenGL/GraphicsDeviceGL.h"
#include "Vulkan/GraphicsDeviceVK.h"
#include "Vulkan/GraphicsContextVK.h"

API GraphicsDevice::s_API = API::OpenGL;

void GraphicsDevice::SwapBuffers()
{
	
}

GraphicsContext* GraphicsDevice::CreateContext(const GraphicsContextDesc& desc)
{
    switch (s_API)
    {
    case API::None:
        return nullptr;
        break;
    case API::OpenGL:
        return nullptr;
        break;
    case API::Vulkan:
        return DBG_NEW VkAPI::GraphicsContextVK((VkAPI::GraphicsDeviceVK*)this, desc); 
        break;
    case API::DirectX:
        return nullptr;
        break;
    default:
        return nullptr;
        break;
    }
}

GraphicsDevice* GraphicsDevice::Create(GLFWwindow* window, API api)
{
    s_API = api;
	switch (api)
	{
    case API::None:
        return nullptr;
        break;
    case API::OpenGL:
        return DBG_NEW GlAPI::GraphicsDeviceGL(window);
        break;
    case API::Vulkan:
        return DBG_NEW VkAPI::GraphicsDeviceVK();
        break;
    case API::DirectX:
        return nullptr;
        break;
    default:
        return nullptr;
        break;
	}
}