#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char *> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef DEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices
{
    int graphicsFamily = -1;
	int presentFamily = -1;

    bool isComplete() {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
        VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
	    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

	    return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
};

class App
{
private:
	GLFWwindow *window;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkSurfaceKHR surface;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	inline static void onWindowResized (GLFWwindow *window, int width, int height)
	{
		if(width == 0 || height == 0) return;

		App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
		app->recreateSwapChain();
	}

public:
	void run ();

private:
	void initWindow ();
	void initVulkan ();
	void mainLoop ();
	void cleanup ();
	void drawFrame ();

	/* VK methods */
	void createInstance ();
	bool checkValidationLayerSupport ();
	void setupDebugCallback ();
	void pickPhysicalDevice ();
	void createLogicalDevice ();
	void createSurface ();
	void createSwapChain ();
	void createImageViews ();
	void createGraphicsPipeline ();
	void createRenderPass ();
	void createFramebuffers ();
	void createCommandPool ();
	void createCommandBuffers ();
    void createSemaphores ();
	void createVertexBuffer ();

	void cleanupSwapChain ();
	void recreateSwapChain ();

	uint32_t findMemoryType (uint32_t typeFilter, VkMemoryPropertyFlags properties);

	/* VK validation layers methods */
	std::vector<const char *> getRequiredExtensions ();
	VkResult CreateDebugReportCallbackEXT (
			VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback);
	void DestroyDebugReportCallbackEXT (
			VkInstance instance,
			VkDebugReportCallbackEXT callback,
			const VkAllocationCallbacks* pAllocator);

	/* PhysicalDevices methods */
	bool isDeviceSuitable (VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies (VkPhysicalDevice device);

	/* Swap Chain methods */
	bool checkDeviceExtensionSupport (VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport (VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat (const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode (const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent (const VkSurfaceCapabilitiesKHR& capabilities);

	/* Graphics Pipeline methods */
	VkShaderModule createShaderModule (const std::vector<char>& code);
};
