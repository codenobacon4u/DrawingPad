#pragma once
#include "Texture.h"

namespace DrawingPad
{
	typedef struct SwapchainDesc {
		uint32_t Width = 0;
		uint32_t Height = 0;
		std::vector<TextureFormat> SurfaceFormats = { TextureFormat::BGRA8UnormSRGB };
		TextureFormat ColorFormat = TextureFormat::RGBA8UnormSRGB;
		TextureFormat DepthFormat = TextureFormat::D32Float;
		uint32_t BufferCount = 2;
	} SwapchainDesc;

	class Swapchain
	{
	public:

		virtual ~Swapchain() {}

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetImageIndex() = 0;
		virtual TextureView* GetBackbuffer() = 0;
		virtual TextureView* GetDepthBufferView() = 0;

		void SetResized(uint32_t width, uint32_t height) { m_Resized = true; m_Desc.Width = width; m_Desc.Height = height; }

		void SetVSync(bool enabled) { m_VSync = enabled; }
		bool IsVSync() { return m_VSync; }

		SwapchainDesc GetDesc() { return m_Desc; }

	protected:
		Swapchain(const SwapchainDesc& desc)
			: m_Desc(desc) {}

	protected:
		bool m_VSync = false;
		bool m_Resized = false;
		SwapchainDesc m_Desc;
	};
}
