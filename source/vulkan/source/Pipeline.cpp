#include "../headers/Pipeline.h"
#include "../../geometry/headers/Vertex.h"
#include <fstream>

namespace Vulkan {
	Pipeline::Pipeline(Memory&& salvageMemory) {
		initPipelineRenderingCreateInfo();
		initPipelineShaderStageCreateInfo();
		initPipelineVertexInputStateCreateInfo();
		initPipelineInputAssemblyStateCreateInfo();
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

	void Pipeline::initPipelineRenderingCreateInfo() {
		VkPipelineRenderingCreateInfo renderingInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.pNext = nullptr,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &colorAttachmentFormat,
			.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
			.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
		};

		this->renderingInfo = renderingInfo;
	}

	void Pipeline::initPipelineShaderStageCreateInfo() {
		shadersData = loadSpvBytes("source/shaders/shaders.spv");
		VkShaderModuleCreateInfo shadersInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = static_cast<uint32_t>(shadersData.size()),
			.pCode = reinterpret_cast<uint32_t*>(shadersData.data())
		};
		if(vkCreateShaderModule(memory.swapchain.queues.backend.device, &shadersInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failure creating shader module");
		}

		VkPipelineShaderStageCreateInfo vertexShaderInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = shaderModule,
			.pName = "vertexShader",
			.pSpecializationInfo = nullptr
		};
		VkPipelineShaderStageCreateInfo fragmentShaderInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = shaderModule,
			.pName = "fragmentShader",
			.pSpecializationInfo = nullptr
		};

		manualShadersInfo.push_back(vertexShaderInfo);
		manualShadersInfo.push_back(fragmentShaderInfo);
	}

	std::vector<char> Pipeline::loadSpvBytes(const char* path) {
		std::ifstream fileIn(path, std::ios::binary | std::ios::ate);
		if(!fileIn.good()) {
			throw std::runtime_error("Failure reading file at " + std::string(path));
		}

		uint32_t fileSize = fileIn.tellg();
		std::vector<char> bytes(fileSize);

		fileIn.seekg(0);
		fileIn.read(bytes.data(), fileSize);

		return bytes;
	}

	void Pipeline::initPipelineVertexInputStateCreateInfo() {
		vertexBinding = Geometry::Vertex::getInputBindingDescription();
		attributeDescriptions = Geometry::Vertex::getInputAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &vertexBinding,
			.vertexAttributeDescriptionCount = 2,
			.pVertexAttributeDescriptions = attributeDescriptions.data()
		};

		this->vertexInputStateInfo = vertexInputStateInfo;
	}

	void Pipeline::initPipelineInputAssemblyStateCreateInfo() {
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		this->inputAssemblyInfo = inputAssemblyInfo;
	}

	void Pipeline::initPipelineViewportStateCreateInfo() {
		VkPipelineViewportStateCreateInfo viewportInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = nullptr, // will dynamically set
			.scissorCount = 1,
			.pScissors = nullptr // will dynamically set
		};

		this->viewportInfo = viewportInfo;
	}

	void Pipeline::initPipelineRasterizationStateCreateInfo() {

	}

	void Pipeline::initPipelineMultisampleStateCreateInfo() {

	}

	void Pipeline::initPipelineDepthStencilStateCreateInfo() {

	}

	void Pipeline::initPipelineColorBlendStateCreateInfo() {

	}

	void Pipeline::initPipelineDynamicStateCreateInfo() {
		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = 2,
			.pDynamicStates = dynamicStates.data()
		};

		this->dynamicStateInfo = dynamicStateInfo;
	}

	void Pipeline::initPipelineLayout() {

	}
}