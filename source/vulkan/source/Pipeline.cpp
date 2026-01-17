#include "../headers/Pipeline.h"
#include "../../geometry/headers/Vertex.h"
#include <fstream>
#include <iostream>

namespace Vulkan {
	Pipeline::Pipeline(Memory&& salvageMemory) :
		isSalvagedRemains{ false },

		renderingInfo{},
		colorAttachmentFormat{ VK_FORMAT_R8G8B8A8_SRGB },
		manualShadersInfo{},
		shadersData{},
		shaderModule{ VK_NULL_HANDLE },
		vertexInputStateInfo{},
		vertexBinding{},
		attributeDescriptions{},
		inputAssemblyInfo{},
		viewportInfo{},
		rasterizationInfo{},
		multisampleInfo{},
		depthStencilInfo{},
		colorBlendInfo{},
		colorAttachmentState{},
		dynamicStateInfo{},
		dynamicStates{},
		layout{ VK_NULL_HANDLE },

		memory(std::move(salvageMemory)),
		graphicsPipeline{ VK_NULL_HANDLE } {

		std::cout << "---CREATING GRAPHICS PIPELINE...---\n";

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

	Pipeline::Pipeline(Pipeline&& salvagePipeline) :
		isSalvagedRemains{ false },

		renderingInfo{ salvagePipeline.renderingInfo },
		colorAttachmentFormat{ salvagePipeline.colorAttachmentFormat },
		manualShadersInfo{ salvagePipeline.manualShadersInfo },
		shadersData{ salvagePipeline.shadersData },
		shaderModule{ salvagePipeline.shaderModule },
		vertexInputStateInfo{ salvagePipeline.vertexInputStateInfo },
		vertexBinding{ salvagePipeline.vertexBinding },
		attributeDescriptions{ salvagePipeline.attributeDescriptions },
		inputAssemblyInfo{ salvagePipeline.inputAssemblyInfo },
		viewportInfo{ salvagePipeline.viewportInfo },
		rasterizationInfo{ salvagePipeline.rasterizationInfo },
		multisampleInfo{ salvagePipeline.multisampleInfo },
		depthStencilInfo{ salvagePipeline.depthStencilInfo },
		colorBlendInfo{ salvagePipeline.colorBlendInfo },
		colorAttachmentState{ salvagePipeline.colorAttachmentState },
		dynamicStateInfo{ salvagePipeline.dynamicStateInfo },
		dynamicStates{ salvagePipeline.dynamicStates },
		layout{ salvagePipeline.layout },

		memory(std::move(salvagePipeline.memory)),
		graphicsPipeline{ salvagePipeline.graphicsPipeline } {
		salvagePipeline.isSalvagedRemains = true;

		salvagePipeline.renderingInfo = {};
		salvagePipeline.colorAttachmentFormat = {};
		salvagePipeline.manualShadersInfo = {};
		salvagePipeline.shadersData = {};
		salvagePipeline.shaderModule = VK_NULL_HANDLE;
		salvagePipeline.vertexInputStateInfo = {};
		salvagePipeline.vertexBinding = {};
		salvagePipeline.attributeDescriptions = {};
		salvagePipeline.inputAssemblyInfo = {};
		salvagePipeline.viewportInfo = {};
		salvagePipeline.rasterizationInfo = {};
		salvagePipeline.multisampleInfo = {};
		salvagePipeline.depthStencilInfo = {};
		salvagePipeline.colorBlendInfo = {};
		salvagePipeline.colorAttachmentState = {};
		salvagePipeline.dynamicStateInfo = {};
		salvagePipeline.dynamicStates = {};
		salvagePipeline.layout = VK_NULL_HANDLE;

		salvagePipeline.graphicsPipeline = VK_NULL_HANDLE;

		std::cout << "---MOVED GRAPHICS PIPELINE---\n";
	}

	Pipeline::~Pipeline() {
		if(!isSalvagedRemains) {
			std::cout << "---CLEANING GRAPHICS PIPELINE...---\n";

			vkDestroyShaderModule(memory.swapchain.queues.backend.device, shaderModule, nullptr);
			vkDestroyPipelineLayout(memory.swapchain.queues.backend.device, layout, nullptr);
			vkDestroyPipeline(memory.swapchain.queues.backend.device, graphicsPipeline, nullptr);
		}
	}

	void Pipeline::createGraphicsPipeline() {
		VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &renderingInfo,
			.flags = 0,
			.stageCount = static_cast<uint32_t>(manualShadersInfo.size()),
			.pStages = manualShadersInfo.data(),
			.pVertexInputState = &vertexInputStateInfo,
			.pInputAssemblyState = &inputAssemblyInfo,
			.pTessellationState = nullptr,
			.pViewportState = &viewportInfo,
			.pRasterizationState = &rasterizationInfo,
			.pMultisampleState = &multisampleInfo,
			.pDepthStencilState = &depthStencilInfo,
			.pColorBlendState = &colorBlendInfo,
			.pDynamicState = &dynamicStateInfo,
			.layout = layout,
			.renderPass = VK_NULL_HANDLE,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = 0
		};

		if(vkCreateGraphicsPipelines(memory.swapchain.queues.backend.device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &graphicsPipeline) == VK_SUCCESS) {
			std::cout << "Vulkan graphics pipeline created\n";
		} else {
			throw std::runtime_error("Vulkan graphics pipeline creation failure");
		}
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
		VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasConstantFactor = 1.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 1.0f,
			.lineWidth = 1.0f
		};

		this->rasterizationInfo = rasterizationInfo;
	}

	void Pipeline::initPipelineMultisampleStateCreateInfo() {
		VkPipelineMultisampleStateCreateInfo multisampleInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 0.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		this->multisampleInfo = multisampleInfo;
	}

	void Pipeline::initPipelineDepthStencilStateCreateInfo() {
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = VK_FALSE,
			.depthWriteEnable = VK_FALSE,
			.depthCompareOp = VK_COMPARE_OP_NEVER,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
			.front = {},
			.back = {},
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
		};

		this->depthStencilInfo = depthStencilInfo;
	}

	void Pipeline::initPipelineColorBlendStateCreateInfo() {
		VkPipelineColorBlendAttachmentState colorAttachmentState = {
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		this->colorAttachmentState = colorAttachmentState;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_CLEAR,
			.attachmentCount = 1,
			.pAttachments = &(this->colorAttachmentState),
			.blendConstants = { 1.0f, 1.0f, 1.0f, 1.0f }
		};
		this->colorBlendInfo = colorBlendInfo;
	}

	void Pipeline::initPipelineDynamicStateCreateInfo() {
		dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

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
		VkPipelineLayoutCreateInfo layoutInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &memory.descriptorSetLayout,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		if(vkCreatePipelineLayout(memory.swapchain.queues.backend.device, &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
			throw std::runtime_error("Pipeline layout creation failure");
		}
	}
}