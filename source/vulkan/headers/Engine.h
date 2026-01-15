#pragma once

#include "../headers/Commands.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Vulkan {
	class Engine {
	private:
		bool isSalvagedRemains;

		uint16_t frameInFlightIndex;
		VkClearColorValue clearColor;

		Commands commands;

		void drawFrame();
		void renderImage();
		void presentImage();

		void record(VkImage& image, VkImageView& colorAttachmentImageView);
		void insertImageMemoryBarrier(VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags2 sourceStage, VkAccessFlags2 sourceAccess, VkPipelineStageFlags2 destStage, VkAccessFlags2 destAccess);
	public:
		void run();

		Engine(Commands&& salvageCommands);
		Engine(Engine&& salvageEngine);
		~Engine();

		Engine(Engine const&) = delete;
		Engine& operator=(Engine const&) = delete;
	};
}