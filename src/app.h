#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char *> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class App
{
private:
	GLFWwindow *window;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
public:
	void run ();

private:
	void initWindow ();
	void initVulkan ();
	void mainLoop ();
	void cleanup ();

	/* VK methods */
	void createInstance();
	bool checkValidationLayerSupport ();
	std::vector<const char *> getRequiredExtensions ();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
		    VkDebugReportFlagsEXT flags,
		    VkDebugReportObjectTypeEXT objType,
		    uint64_t obj,
		    size_t location,
		    int32_t code,
		    const char* layerPrefix,
		    const char* msg,
		    void* userData);
	void setupDebugCallback ();
	VkResult CreateDebugReportCallbackEXT (
			VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback);
	void DestroyDebugReportCallbackEXT (
			VkInstance instance,
			VkDebugReportCallbackEXT callback,
			const VkAllocationCallbacks* pAllocator);
};
