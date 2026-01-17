#pragma once

#include "../headers/Commands.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Engine {
	private:
		const uint16_t GRAPHICS_QUEUE_COUNT;
		const uint16_t FLIGHT_COUNT;
		uint16_t queueIndex;
		uint16_t flightIndex;
		VkClearColorValue clearColor;

		uint16_t framesLastSecond;

		Commands commands;

		void queuesDrawFrame();
		void renderAndPresent(VkQueue& queue);

		void writeUniformBuffer();
		void record(VkImage& image, VkImageView& colorAttachmentImageView);
		void insertImageMemoryBarrier(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess);
		uint16_t convertDoubleToSingleIndex(uint16_t queueIndex, uint16_t flightIndex);
	public:
		void run();

		Engine(Commands&& salvageCommands);

		Engine(Engine const&) = delete;
		Engine& operator=(Engine const&) = delete;
	};
}