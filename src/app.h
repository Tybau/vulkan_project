#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>

class App
{
private:
	GLFWwindow *window;
	VkInstance instance;
public:
	void run ();

private:
	void initWindow ();
	void initVulkan ();
	void mainLoop ();
	void cleanup ();

	/* VK methods */
	void createInstance();
};
