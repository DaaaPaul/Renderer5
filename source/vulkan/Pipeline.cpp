#include "Pipeline.h"

namespace Vulkan {
	Pipeline::Pipeline(Memory&& salvageMemory) {
		initPipelineRenderingCreateInfo();
		initPipelineShaderStageCreateInfo();
		initPipelineVertexInputStateCreateInfo();
		initPipelineInputAssemblyStateCreateInfo();
		initPipelineTessellationStateCreateInfo();
		initPipelineViewportStateCreateInfo();
		initPipelineRasterizationStateCreateInfo();
		initPipelineMultisampleStateCreateInfo();
		initPipelineDepthStencilStateCreateInfo();
		initPipelineColorBlendStateCreateInfo();
		initPipelineDynamicStateCreateInfo();
		initPipelineLayout();

		createGraphicsPipeline();
	}

	Pipeline::Pipeline(Pipeline&& salvagePipeline) {
		salvagePipeline.isSalvagedRemains = true;
	}

	Pipeline::~Pipeline() {
		if(!isSalvagedRemains) {
		
		}
	}

	void Pipeline::createGraphicsPipeline() {
		
	}

	VkPipelineRenderingCreateInfo Pipeline::initPipelineRenderingCreateInfo() {

	}

	std::vector<VkPipelineShaderStageCreateInfo> Pipeline::initPipelineShaderStageCreateInfo() {

	}

	VkPipelineVertexInputStateCreateInfo Pipeline::initPipelineVertexInputStateCreateInfo() {

	}

	VkPipelineInputAssemblyStateCreateInfo Pipeline::initPipelineInputAssemblyStateCreateInfo() {

	}

	VkPipelineTessellationStateCreateInfo Pipeline::initPipelineTessellationStateCreateInfo() {

	}

	VkPipelineViewportStateCreateInfo Pipeline::initPipelineViewportStateCreateInfo() {

	}

	VkPipelineRasterizationStateCreateInfo Pipeline::initPipelineRasterizationStateCreateInfo() {

	}

	VkPipelineMultisampleStateCreateInfo Pipeline::initPipelineMultisampleStateCreateInfo() {

	}

	VkPipelineDepthStencilStateCreateInfo Pipeline::initPipelineDepthStencilStateCreateInfo() {

	}

	VkPipelineColorBlendStateCreateInfo Pipeline::initPipelineColorBlendStateCreateInfo() {

	}

	VkPipelineDynamicStateCreateInfo Pipeline::initPipelineDynamicStateCreateInfo() {

	}

	VkPipelineLayout Pipeline::initPipelineLayout() {

	}
}