#include "vulkan/headers/Backend.h"
#include "vulkan/headers/Queues.h"
#include "vulkan/headers/Swapchain.h"
#include "vulkan/headers/Memory.h"
#include "vulkan/headers/Pipeline.h"
#include <iostream>
#include <stdexcept>
#include <utility>

int main() {
    try {
        Vulkan::Backend backend{};
        Vulkan::Queues queues(std::move(backend));
        Vulkan::Swapchain swapchain(std::move(queues));
        Vulkan::Memory memory(std::move(swapchain));
        Vulkan::Pipeline pipeline(std::move(memory));
    } catch(std::exception const& exception) {
        std::cout << "ERROR: " << exception.what() << '\n';
    }

    return 0;
}