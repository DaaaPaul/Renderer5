#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <climits>
#include <array>

#define DELETE_COPYING(className) \
		className(className const&) = delete; \
		className& operator=(className const&) = delete;