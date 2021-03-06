#pragma once
#include "Pipeline.h"

#include "RenderPass.h"

#include <vulkan/vulkan.h>

namespace VkAPI
{
    class GraphicsDeviceVK;
    class PipelineVK : public Pipeline
    {
    public:
        PipelineVK(GraphicsDeviceVK* device, const GraphicsPipelineDesc& createInfo, RenderPass* renderPass = nullptr);
        PipelineVK(GraphicsDeviceVK* device, const ComputePipelineDesc& createInfo);
        PipelineVK(GraphicsDeviceVK* device, const RaytracingPipelineDesc& createInfo);
        ~PipelineVK();

        VkPipeline Get() { return m_Pipeline; }

    private:
        VkPipeline m_Pipeline;
        GraphicsDeviceVK* m_Device;
    };
}
