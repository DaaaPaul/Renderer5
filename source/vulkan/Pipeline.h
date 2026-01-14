#pragma once

#include "Memory.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Sync;
	class Commands;
	class Engine;

	class Pipeline {
		friend class Sync;
		friend class Commands;
		friend class Engine;

	private:
		bool isSalvagedRemains;

		VkPipelineRenderingCreateInfo renderingInfo;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineTessellationStateCreateInfo tessellationInfo;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout layout;

		Memory memory;
		VkPipeline graphicsPipeline;

		void createGraphicsPipeline();

		VkPipelineRenderingCreateInfo initPipelineRenderingCreateInfo();
		std::vector<VkPipelineShaderStageCreateInfo> initPipelineShaderStageCreateInfo();
		VkPipelineVertexInputStateCreateInfo initPipelineVertexInputStateCreateInfo();
		VkPipelineInputAssemblyStateCreateInfo initPipelineInputAssemblyStateCreateInfo();
		VkPipelineTessellationStateCreateInfo initPipelineTessellationStateCreateInfo();
		VkPipelineViewportStateCreateInfo initPipelineViewportStateCreateInfo();
		VkPipelineRasterizationStateCreateInfo initPipelineRasterizationStateCreateInfo();
		VkPipelineMultisampleStateCreateInfo initPipelineMultisampleStateCreateInfo();
		VkPipelineDepthStencilStateCreateInfo initPipelineDepthStencilStateCreateInfo();
		VkPipelineColorBlendStateCreateInfo initPipelineColorBlendStateCreateInfo();
		VkPipelineDynamicStateCreateInfo initPipelineDynamicStateCreateInfo();
		VkPipelineLayout initPipelineLayout();

	public:
		Pipeline(Memory&& salvageMemory);
		Pipeline(Pipeline&& salvagePipeline);
		~Pipeline();

		Pipeline(Pipeline const&) = delete;
		Pipeline& operator=(Pipeline const&) = delete;
	};
}