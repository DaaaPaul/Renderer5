#pragma once

#include "../headers/Commands.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Engine {
	private:
		const uint16_t FRAMES_IN_FLIGHT;
		uint16_t frameInFlightIndex;
		VkClearColorValue clearColor;

		Commands commands;

		void drawToQueues();
		void renderAndPresent(VkQueue& queue);

		void record(VkImage& image, VkImageView& colorAttachmentImageView);
		void insertImageMemoryBarrier(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess);
	public:
		void run();

		Engine(Commands&& salvageCommands);

		Engine(Engine const&) = delete;
		Engine& operator=(Engine const&) = delete;
	};
}