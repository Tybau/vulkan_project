#include "app.h"

void App::run ()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void App::initWindow ()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if(!enableValidationLayers)
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	else
		window = glfwCreateWindow(WIDTH, HEIGHT, "[DEBUG] Vulkan", nullptr, nullptr);
}

void App::initVulkan ()
{
	createInstance();
	setupDebugCallback();
	pickPhysicalDevice();
	createLogicalDevice();
}

void App::mainLoop ()
{
	while (!glfwWindowShouldClose(window))
	{
        glfwPollEvents();
    }
}

void App::cleanup ()
{
	vkDestroyDevice(device, nullptr);
	DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);
    glfwTerminate();
}

/* VK methods */

void App::createInstance ()
{
	if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("validation layers requested, but not available!");

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	createInfo.enabledLayerCount = 0;

	if (enableValidationLayers)
	{
	    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	    createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	    throw std::runtime_error("failed to create instance!");
}

bool App::checkValidationLayerSupport ()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : validationLayers)
	{
	    bool layerFound = false;

	    for (const auto& layerProperties : availableLayers)
		{
	        if (strcmp(layerName, layerProperties.layerName) == 0)
			{
	            layerFound = true;
	            break;
	        }
	    }

	    if (!layerFound)
	        return false;
	}

	return true;
}

std::vector<const char *> App::getRequiredExtensions ()
{
    std::vector<const char *> extensions;

    unsigned int glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; i++)
        extensions.push_back(glfwExtensions[i]);

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
}

void App::setupDebugCallback ()
{
	if (!enableValidationLayers) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
	    throw std::runtime_error("failed to set up debug callback!");
}

void App::pickPhysicalDevice ()
{
	physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("failed to find GPUs with Vulkan support!");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
	    if (isDeviceSuitable(device))
		{
	        physicalDevice = device;
	        break;
	    }
	}

	if (physicalDevice == VK_NULL_HANDLE)
	    throw std::runtime_error("failed to find a suitable GPU!");
}

void App::createLogicalDevice ()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;


	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	createInfo.enabledLayerCount = 0;

	if (enableValidationLayers)
	{
	    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	    createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
    	throw std::runtime_error("failed to create logical device!");

	vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
}

/* VK validation layers methods */

VKAPI_ATTR VkBool32 VKAPI_CALL App::debugCallback(
	    VkDebugReportFlagsEXT flags,
	    VkDebugReportObjectTypeEXT objType,
	    uint64_t obj,
	    size_t location,
	    int32_t code,
	    const char* layerPrefix,
	    const char* msg,
	    void* userData)
{
    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

VkResult App::CreateDebugReportCallbackEXT (
		VkInstance instance,
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugReportCallbackEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pCallback);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void App::DestroyDebugReportCallbackEXT (
		VkInstance instance,
		VkDebugReportCallbackEXT callback,
		const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
        func(instance, callback, pAllocator);
}

/* PhysicalDevices methods */

bool App::isDeviceSuitable (VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader && indices.isComplete();
}

QueueFamilyIndices App::findQueueFamilies (VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
	    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
	        indices.graphicsFamily = i;

	    if (indices.isComplete())
	        break;

	    i++;
	}

    return indices;
}
