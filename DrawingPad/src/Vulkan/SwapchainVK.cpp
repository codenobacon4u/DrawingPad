#include "pwpch.h"
#include "SwapchainVK.h"

#include "GraphicsDeviceVK.h"
#include "TextureVK.h"

namespace VkAPI
{
	SwapchainVK::SwapchainVK(GraphicsDeviceVK* device, GraphicsContextVK* context, SwapchainDesc desc, GLFWwindow* window)
		: Swapchain(desc), m_Device(device), m_Swap(nullptr), m_Context(context)
	{
		auto res = glfwCreateWindowSurface(device->GetInstance(), window, nullptr, &m_Surface);
		if (res == VK_ERROR_INITIALIZATION_FAILED)
		{
			throw std::runtime_error("API not available");
		}
		else if (res == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			throw std::runtime_error("The extension was not available");
		}
		else if (res == VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
		{
			throw std::runtime_error("Native window is in use");
		}
		auto framebufferCallback = [&](GLFWwindow* window, int width, int height) {
			m_Resized = true;
		};
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int width, int height) {
			SwapchainVK* swap = (SwapchainVK*)glfwGetWindowUserPointer(win);
			swap->SetResized(width, height);
		});

		m_PresentIndex = m_Device->FindQueueFamilies(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT).graphicsFamily.value();

		if (!m_Device->QueryPresentSupport(m_PresentIndex, m_Surface))
			throw std::runtime_error("Physical Device does not support present capability");

		vkGetDeviceQueue(m_Device->Get(), m_PresentIndex, 0, &m_Present);

		RecreateSwap(desc.Width, desc.Height);

		AcquireNextImage();
	}

	void SwapchainVK::Resize(uint32_t width, uint32_t height)
	{
		RecreateSwap(width, height);
		if (AcquireNextImage()) {
		}
	}

	void SwapchainVK::Present(uint32_t sync)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swap;
		presentInfo.pImageIndices = &m_ImageIndex;
		auto result = vkQueuePresentKHR(m_Present, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Resized)
		{
			m_Resized = false;
			RecreateSwap(m_Desc.Width, m_Desc.Height);
			//m_Context->SetRenderTargets(0, nullptr, nullptr);
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to present swap image!");

		m_CurrFrame = (m_CurrFrame + 1) % FRAMES_IN_FLIGHT;

		if (AcquireNextImage()) {
		}
	}

	bool SwapchainVK::AcquireNextImage()
	{
		// Add waiting for the fences here so that we much wait for the current frame's fence to signal
		vkWaitForFences(m_Device->Get(), 1, &m_FlightFences[m_CurrFrame], VK_TRUE, UINT64_MAX);
		// We need the imageAcquired semaphores to be sent out for the command buffer submission
		VkResult result = vkAcquireNextImageKHR(m_Device->Get(), m_Swap, ULONG_MAX, m_ImageAcquiredSemaphores[m_CurrFrame], VK_NULL_HANDLE, &m_ImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			RecreateSwap(m_Desc.Width, m_Desc.Height);
			return false;
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("Could not  next image from the swapchain");

		if (m_ImagesInFlight[m_ImageIndex] != VK_NULL_HANDLE)
			vkWaitForFences(m_Device->Get(), 1, &m_ImagesInFlight[m_ImageIndex], VK_TRUE, UINT64_MAX);
		m_ImagesInFlight[m_ImageIndex] = m_FlightFences[m_CurrFrame];

		m_Context->AddWaitSemaphore(&m_ImageAcquiredSemaphores[m_CurrFrame]);
		m_Context->AddSubFence(&m_FlightFences[m_CurrFrame]);

		// Check if a previous frame is using the acquired image (if so... wait)
		// Mark the image as now in flight using the value of the current frame's 
		// inFlight fence and the index of the image.

		return true;
	}

	void SwapchainVK::RecreateSwap(uint32_t width, uint32_t height)
	{
		if (m_Swap != nullptr) {
			m_Device->WaitForIdle();
			for (uint32_t i = 0; i < m_FlightFences.size(); i++)
				if (m_FlightFences[i])
					if (vkGetFenceStatus(m_Device->Get(), m_FlightFences[i]) == VK_NOT_READY)
						vkWaitForFences(m_Device->Get(), 1, &m_FlightFences[i], VK_TRUE, UINT64_MAX);
		}

		m_BackBuffers.clear();
		if (m_DepthBuffer)
			vkDestroyImageView(m_Device->Get(), m_DepthBuffer->GetView(), nullptr);
		m_ImageAcquiredSemaphores.clear();
		m_DrawCompleteSemaphores.clear();
		m_FlightFences.clear();
		m_ImagesInFlight.clear();

		m_ImageIndex = 0;
		SwapSupportDetails support = QuerySwapSupport();
		auto surfaceFormat = ChooseSwapSurfaceFormat(support.formats);
		m_PresentMode = ChooseSwapPresentMode(support.presentModes);
		m_Extent = ChooseSwapExtent(support.capabilities, width, height);

		uint32_t imageCount = support.capabilities.minImageCount + 1;
		if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
			imageCount = support.capabilities.maxImageCount;
		}

		m_Desc.Width = m_Extent.width;
		m_Desc.Height = m_Extent.height;
		m_Desc.DepthFormat = ChooseDepthFormat();

		auto old = m_Swap;
		m_Swap = nullptr;

		VkSwapchainCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
		createInfo.presentMode = m_PresentMode;
		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.clipped = true;
		createInfo.oldSwapchain = old;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		m_Desc.ColorFormat = UtilsVK::Convert(surfaceFormat.format);

		//QueueFamilyIndices indices = m_Device->FindQueueFamilies();
		//uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		//
		//if (indices.graphicsFamily != indices.presentFamily) {
		//	createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		//	createInfo.queueFamilyIndexCount = 2;
		//	createInfo.pQueueFamilyIndices = queueFamilyIndices;
		//}
		//else {
		//}

		if (vkCreateSwapchainKHR(m_Device->Get(), &createInfo, nullptr, &m_Swap) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create swapchain");
		}

		DebugMarker::SetName(m_Device->Get(), (uint64_t)m_Swap, VK_OBJECT_TYPE_SWAPCHAIN_KHR, "Main Swapchain");

		if (old != nullptr)
		{
			vkDestroySwapchainKHR(m_Device->Get(), old, NULL);
			old = nullptr;
		}


		uint32_t swapImageCount;
		vkGetSwapchainImagesKHR(m_Device->Get(), m_Swap, &swapImageCount, nullptr);
		if (swapImageCount != m_Desc.BufferCount)
			m_Desc.BufferCount = swapImageCount;
		m_ImageAcquiredSemaphores.resize(FRAMES_IN_FLIGHT);
		m_DrawCompleteSemaphores.resize(FRAMES_IN_FLIGHT);
		m_FlightFences.resize(FRAMES_IN_FLIGHT);
		m_ImagesInFlight.resize(swapImageCount, VK_NULL_HANDLE);
		for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{ 
			VkSemaphoreCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			vkCreateSemaphore(m_Device->Get(), &createInfo, nullptr, &m_ImageAcquiredSemaphores[i]);
			vkCreateSemaphore(m_Device->Get(), &createInfo, nullptr, &m_DrawCompleteSemaphores[i]);

			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			vkCreateFence(m_Device->Get(), &fenceInfo, nullptr, &m_FlightFences[i]);
		}

		CreatePresentResources(surfaceFormat);
	}

	SwapSupportDetails SwapchainVK::QuerySwapSupport()
	{
		SwapSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device->GetPhysical(), m_Surface, &details.capabilities);

		uint32_t count;
		if ((vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysical(), m_Surface, &count, nullptr) == VK_SUCCESS) && count) {
			details.presentModes.resize(count);
			if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device->GetPhysical(), m_Surface, &count, reinterpret_cast<VkPresentModeKHR*>(details.presentModes.data())) != VK_SUCCESS)
				throw std::runtime_error("Failed to get present modes for surface");
		}

		count = 0;
		if ((vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysical(), m_Surface, &count, nullptr) == VK_SUCCESS) && count) {
			details.formats.resize(count);
			if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device->GetPhysical(), m_Surface, &count, reinterpret_cast<VkSurfaceFormatKHR*>(details.formats.data())) != VK_SUCCESS)
				throw std::runtime_error("Failed to get present modes for surface");
		}

		return details;
	}

	VkSurfaceFormatKHR SwapchainVK::ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapchainVK::ChooseSwapPresentMode(std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapchainVK::ChooseSwapExtent(VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtent = { width, height };

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	TextureFormat SwapchainVK::ChooseDepthFormat()
	{
		
		std::vector<VkFormat> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
		for (VkFormat format : formats)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_Device->GetPhysical(), format, &props);

			if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
				return UtilsVK::Convert(format);
		}
		throw std::runtime_error("Fialed to find supported depth buffer format!");
	}

	void SwapchainVK::CreatePresentResources(VkSurfaceFormatKHR surfaceFormat)
	{
		uint32_t imageCount;
		vkGetSwapchainImagesKHR(m_Device->Get(), m_Swap, &imageCount, nullptr);
		m_BackBuffers.resize(imageCount);
		m_SwapImagesInitialized.resize(m_BackBuffers.size(), false);
		m_ImageAcquiredFenceSubmitted.resize(m_BackBuffers.size(), false);
		std::vector<VkImage> swapImages(imageCount);
		auto err = vkGetSwapchainImagesKHR(m_Device->Get(), m_Swap, &imageCount, swapImages.data());
		if (err != VK_SUCCESS)
			throw new std::runtime_error("Failed to get swapchain images");
		for (uint32_t i = 0; i < imageCount; i++)
		{	
			TextureDesc backDesc;
			backDesc.Type = TextureType::DimTex2D;
			backDesc.Width = m_Desc.Width;
			backDesc.Height = m_Desc.Height;
			backDesc.Format = m_Desc.ColorFormat;
			backDesc.BindFlags = BindFlags::RenderTarget;
			backDesc.MipLevels = 1;

			TextureVK* backTex = static_cast<TextureVK*>(m_Device->CreateTextureFromImage(backDesc, swapImages[i]));
			
			std::string name = "Backbuffer Image View ";
			name += std::to_string(i);
			DebugMarker::SetName(m_Device->Get(), (uint64_t)backTex->GetImage(), VK_OBJECT_TYPE_IMAGE, name);
			
			TextureViewDesc tvDesc;
			tvDesc.ViewType = ViewType::RenderTarget;
			tvDesc.Format = backDesc.Format;
			TextureViewVK* rtv = static_cast<TextureViewVK*>(backTex->CreateView(tvDesc));
			m_BackBuffers[i] = rtv;

			name = "Backbuffer Image View ";
			name += std::to_string(i);
			DebugMarker::SetName(m_Device->Get(), (uint64_t)rtv->GetView(), VK_OBJECT_TYPE_IMAGE_VIEW, name);
		}

		if (m_Desc.DepthFormat != TextureFormat::Unknown)
		{
			TextureDesc depthDesc = {};
			depthDesc.Type = TextureType::DimTex2D;
			depthDesc.Width = m_Desc.Width;
			depthDesc.Height = m_Desc.Height;
			depthDesc.Format = m_Desc.DepthFormat;
			depthDesc.SampleCount = 1;
			depthDesc.MipLevels = 1;
			depthDesc.Usage = Usage::Default;
			depthDesc.BindFlags = BindFlags::DepthStencil;
			auto* tex = m_Device->CreateTexture(depthDesc, nullptr);
			//m_DepthBuffer = (TextureViewVK*)tex->;
		}
	}
}
