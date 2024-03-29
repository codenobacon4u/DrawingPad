#include "dppch.h"
#include "FramebufferPoolVK.h"

#include <chrono>

#include "GraphicsDeviceVK.h"

namespace DrawingPad
{
	namespace Vulkan
	{
		using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;
		bool FBKey::operator==(const FBKey& rhs) const
		{
			if (GetHash() != rhs.GetHash() ||
				Pass != rhs.Pass ||
				Attachments.size() != rhs.Attachments.size())
				return false;
			for (uint32_t i = 0; i < Attachments.size(); i++)
				if (Attachments[i] != rhs.Attachments[i])
					return false;
			return true;
		}

		size_t FBKey::GetHash() const
		{
			if (Hash == 0)
			{
				hash_combine(Hash, Pass);
				for (uint32_t i = 0; i < Attachments.size(); i++)
					hash_combine(Hash, Attachments[i]);
			}
			return Hash;
		}

		FramebufferPoolVK::~FramebufferPoolVK()
		{
			for (auto& [key, fb] : m_Map)
				vkDestroyFramebuffer(m_Device->Get(), fb, nullptr);
		}

		void FramebufferPoolVK::DeleteViewEntry(VkImageView view)
		{
			auto range = m_VTKMap.equal_range(view);
			for (auto& it = range.first; it != range.second; it++)
			{
				auto fb = m_Map.find(it->second);
				if (fb != m_Map.end())
				{
					vkDestroyFramebuffer(m_Device->Get(), fb->second, nullptr);
					m_Map.erase(fb);
				}
			}
			m_VTKMap.erase(range.first, range.second);
		}

		VkFramebuffer FramebufferPoolVK::GetFramebuffer(const FBKey& key, uint32_t width, uint32_t height, uint32_t layers)
		{
			auto it = m_Map.find(key);
			if (it != m_Map.end())
				return it->second;
			else
			{
				VkFramebufferCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				createInfo.flags = 0;
				createInfo.renderPass = key.Pass;
				createInfo.attachmentCount = static_cast<uint32_t>(key.Attachments.size());
				createInfo.pAttachments = key.Attachments.data();
				createInfo.width = width;
				createInfo.height = height;
				createInfo.layers = layers;
				VkFramebuffer fb;
				vkCreateFramebuffer(m_Device->Get(), &createInfo, nullptr, &fb);
				m_Map.insert(std::make_pair(key, std::move(fb)));

				for (size_t i = 0; i < key.Attachments.size(); i++)
					m_VTKMap.emplace(key.Attachments[i], key);

				return fb;
			}
		}
	}
}
