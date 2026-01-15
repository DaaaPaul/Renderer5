#include "../headers/Engine.h"
#include <iostream>
#include <climits>

#define BACKEND commands.sync.pipeline.memory.swapchain.queues.backend

namespace Vulkan {
	Engine::Engine(Commands&& salvageCommands) :
		FRAMES_IN_FLIGHT{ salvageCommands.FRAMES_IN_FLIGHT },
		frameInFlightIndex{ 0 },
		clearColor{ 0.55f, 0.27f, 0.074f, 1.0f },
		commands(std::move(salvageCommands)) {
		std::cout << "---CREATED ENGINE---\n";
	}

	void Engine::run() {
		while (!glfwWindowShouldClose(BACKEND.window)) {
			drawToQueues();
			glfwPollEvents();

			if(glfwGetKey(BACKEND.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(BACKEND.window, true);
			}
		}

		vkQueueWaitIdle(commands.sync.pipeline.memory.swapchain.queues.graphicsQueues[0]);
		vkQueueWaitIdle(commands.sync.pipeline.memory.swapchain.queues.graphicsQueues[1]);
	}

	void Engine::drawToQueues() {
		renderAndPresent(commands.sync.pipeline.memory.swapchain.queues.graphicsQueues[0]);
		renderAndPresent(commands.sync.pipeline.memory.swapchain.queues.graphicsQueues[1]);
	}

	void Engine::renderAndPresent(VkQueue& queue) {
		vkWaitForFences(BACKEND.device, 1, &commands.sync.commandBufferFinished[frameInFlightIndex], true, UINT64_MAX);

		uint32_t imageIndex = 0xFFFFFFFF;
		vkAcquireNextImageKHR(BACKEND.device, commands.sync.pipeline.memory.swapchain.swapchain, UINT64_MAX, commands.sync.imageReady[frameInFlightIndex], VK_NULL_HANDLE, &imageIndex);

		vkResetFences(BACKEND.device, 1, &commands.sync.commandBufferFinished[frameInFlightIndex]);
		vkResetCommandBuffer(commands.commandBuffers[frameInFlightIndex], 0);

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo drawSubmit = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &commands.sync.imageReady[frameInFlightIndex],
			.pWaitDstStageMask = &waitStage,
			.commandBufferCount = 1,
			.pCommandBuffers = &commands.commandBuffers[frameInFlightIndex],
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &commands.sync.imageFinished[frameInFlightIndex],
		};

		record(commands.sync.pipeline.memory.swapchain.images[imageIndex], commands.sync.pipeline.memory.swapchain.imageViews[imageIndex]);
		vkQueueSubmit(queue, 1, &drawSubmit, commands.sync.commandBufferFinished[frameInFlightIndex]);

		VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &commands.sync.imageFinished[frameInFlightIndex],
			.swapchainCount = 1,
			.pSwapchains = &commands.sync.pipeline.memory.swapchain.swapchain,
			.pImageIndices = &imageIndex,
			.pResults = nullptr,
		};
		vkQueuePresentKHR(queue, &presentInfo);

		frameInFlightIndex = (frameInFlightIndex + 1) % FRAMES_IN_FLIGHT;
	}

	void Engine::record(VkImage& image, VkImageView& colorAttachmentImageView) {
		VkCommandBuffer& targetCommandBuffer = commands.commandBuffers[frameInFlightIndex];

		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pInheritanceInfo = nullptr
		};

		if(vkBeginCommandBuffer(targetCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Command buffer begin recording failure");
		}

		insertImageMemoryBarrier(image,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

		VkRenderingAttachmentInfo colorAttachmentInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = colorAttachmentImageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = clearColor
		};

		VkRect2D scissor = {
			.offset = VkOffset2D(0, 0),
			.extent = commands.sync.pipeline.memory.swapchain.imageExtent
		};

		VkRenderingInfo renderingInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderArea = scissor,
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentInfo,
			.pDepthAttachment = nullptr,
			.pStencilAttachment = nullptr,
		};

		VkViewport viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(commands.sync.pipeline.memory.swapchain.imageExtent.width),
			.height = static_cast<float>(commands.sync.pipeline.memory.swapchain.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		vkCmdBeginRendering(targetCommandBuffer, &renderingInfo);
		vkCmdBindPipeline(targetCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, commands.sync.pipeline.graphicsPipeline);
		vkCmdSetViewport(targetCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(targetCommandBuffer, 0, 1, &scissor);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(targetCommandBuffer, 0, 1, &commands.sync.pipeline.memory.verticesBuffer, &offset);
		vkCmdBindIndexBuffer(targetCommandBuffer, commands.sync.pipeline.memory.indicesBuffer, offset, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(targetCommandBuffer, commands.sync.pipeline.memory.indices.size(), 1, 0, 0, 0);
		vkCmdEndRendering(targetCommandBuffer);

		insertImageMemoryBarrier(image,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE);

		if (vkEndCommandBuffer(targetCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Command buffer end recording failure");
		}
	}

	void Engine::insertImageMemoryBarrier(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess) {
		VkImageSubresourceRange memoryBarrierResourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		};
		
		VkImageMemoryBarrier2 imageMemoryBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.pNext = nullptr,
			.srcStageMask = sourceStage,
			.srcAccessMask = sourceAccess,
			.dstStageMask = destStage,
			.dstAccessMask = destAccess,
			.oldLayout = oldLayout,
			.newLayout = newLayout,
			.srcQueueFamilyIndex = commands.graphicsQueueFamilyIndex,
			.dstQueueFamilyIndex = commands.graphicsQueueFamilyIndex,
			.image = image,
			.subresourceRange = memoryBarrierResourceRange
		};

		VkDependencyInfo memoryBarriersInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = 0,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = 0,
			.pBufferMemoryBarriers = nullptr,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &imageMemoryBarrier,
		};

		vkCmdPipelineBarrier2(commands.commandBuffers[frameInFlightIndex], &memoryBarriersInfo);
	}
}