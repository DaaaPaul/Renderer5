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
		VkFormat colorAttachmentFormat;
		std::vector<VkPipelineShaderStageCreateInfo> manualShadersInfo;
		std::vector<char> shadersData;
		VkShaderModule shaderModule;
		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo;
		VkVertexInputBindingDescription vertexBinding;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineColorBlendAttachmentState colorAttachmentState;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		std::vector<VkDynamicState> dynamicStates;
		VkPipelineLayout layout;

		Memory memory;
		VkPipeline graphicsPipeline;

		void createGraphicsPipeline();

		void initPipelineRenderingCreateInfo();
		void initPipelineShaderStageCreateInfo();
		void initPipelineVertexInputStateCreateInfo();
		void initPipelineInputAssemblyStateCreateInfo();
		void initPipelineViewportStateCreateInfo();
		void initPipelineRasterizationStateCreateInfo();
		void initPipelineMultisampleStateCreateInfo();
		void initPipelineDepthStencilStateCreateInfo();
		void initPipelineColorBlendStateCreateInfo();
		void initPipelineDynamicStateCreateInfo();
		void initPipelineLayout();

		std::vector<char> loadSpvBytes(const char* path);
	public:
		Pipeline(Memory&& salvageMemory);
		Pipeline(Pipeline&& salvagePipeline);
		~Pipeline();

		Pipeline(Pipeline const&) = delete;
		Pipeline& operator=(Pipeline const&) = delete;
	};
}