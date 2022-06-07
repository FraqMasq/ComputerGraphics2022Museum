// This has been adapted from the Vulkan tutorial

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// New in Lesson 23 - to load images
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//

const int MAX_FRAMES_IN_FLIGHT = 2;

// Lesson 22.0
const std::vector<const char*> validationLayers = {	"VK_LAYER_KHRONOS_validation"
};

// Lesson 13
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Lesson 17
struct Vertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 texCoord;
	
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return bindingDescription;	}
	
	static std::array<VkVertexInputAttributeDescription, 3>
						getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3>
						attributeDescriptions{};
		
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;		attributeDescriptions[0].offset = offsetof(Vertex, pos);
						
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;		attributeDescriptions[1].offset = offsetof(Vertex, norm);
		
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
						
		return attributeDescriptions;	}
};


// Lesson 13
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() &&
			   presentFamily.has_value();	}
};

// Lesson 14
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;	std::vector<VkPresentModeKHR> presentModes;
};


//// For debugging - Lesson 22.0
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(instance,
					"vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//// For debugging - Lesson 22.0
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator) {	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(instance,
					"vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
//// For debugging - Lesson 22.0
struct errorcode {
	VkResult resultCode;
	std::string meaning;
}
ErrorCodes[ ] = {
	{ VK_NOT_READY, "Not Ready" },
	{ VK_TIMEOUT, "Timeout" },
	{ VK_EVENT_SET, "Event Set" },
	{ VK_EVENT_RESET, "Event Reset" },
	{ VK_INCOMPLETE, "Incomplete" },
	{ VK_ERROR_OUT_OF_HOST_MEMORY, "Out of Host Memory" },
	{ VK_ERROR_OUT_OF_DEVICE_MEMORY, "Out of Device Memory" },
	{ VK_ERROR_INITIALIZATION_FAILED, "Initialization Failed" },
	{ VK_ERROR_DEVICE_LOST, "Device Lost" },
	{ VK_ERROR_MEMORY_MAP_FAILED, "Memory Map Failed" },
	{ VK_ERROR_LAYER_NOT_PRESENT, "Layer Not Present" },
	{ VK_ERROR_EXTENSION_NOT_PRESENT, "Extension Not Present" },
	{ VK_ERROR_FEATURE_NOT_PRESENT, "Feature Not Present" },
	{ VK_ERROR_INCOMPATIBLE_DRIVER, "Incompatible Driver" },
	{ VK_ERROR_TOO_MANY_OBJECTS, "Too Many Objects" },
	{ VK_ERROR_FORMAT_NOT_SUPPORTED, "Format Not Supported" },
	{ VK_ERROR_FRAGMENTED_POOL, "Fragmented Pool" },
	{ VK_ERROR_SURFACE_LOST_KHR, "Surface Lost" },
	{ VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "Native Window in Use" },
	{ VK_SUBOPTIMAL_KHR, "Suboptimal" },
	{ VK_ERROR_OUT_OF_DATE_KHR, "Error Out of Date" },
	{ VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "Incompatible Display" },
	{ VK_ERROR_VALIDATION_FAILED_EXT, "Valuidation Failed" },
	{ VK_ERROR_INVALID_SHADER_NV, "Invalid Shader" },
	{ VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "Out of Pool Memory" },
	{ VK_ERROR_INVALID_EXTERNAL_HANDLE, "Invalid External Handle" },

};
void PrintVkError( VkResult result ) {
	const int numErrorCodes = sizeof( ErrorCodes ) / sizeof( struct errorcode );
	std::string meaning = "";
	for( int i = 0; i < numErrorCodes; i++ ) {
		if( result == ErrorCodes[i].resultCode ) {
			meaning = ErrorCodes[i].meaning;
			break;
		}
	}
	std::cout << "Error: " << result << ", " << meaning << "\n";
}

class BaseProject;

struct Model {
	BaseProject *BP;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	
	void loadModel(std::string file);
	void createIndexBuffer();
	void createVertexBuffer();

	void init(BaseProject *bp, std::string file);
	void cleanup();
};

struct Texture {
	BaseProject *BP;
	uint32_t mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
	
	void createTextureImage(std::string file);
	void createTextureImageView();
	void createTextureSampler();

	void init(BaseProject *bp, std::string file);
	void cleanup();
};

struct DescriptorSetLayoutBinding {
	uint32_t binding;
	VkDescriptorType type;
	VkShaderStageFlags flags;
};


struct DescriptorSetLayout {
	BaseProject *BP;
 	VkDescriptorSetLayout descriptorSetLayout;
 	
 	void init(BaseProject *bp, std::vector<DescriptorSetLayoutBinding> B);
	void cleanup();
};

struct Pipeline {
	BaseProject *BP;
	VkPipeline graphicsPipeline;
  	VkPipelineLayout pipelineLayout;
  	
  	void init(BaseProject *bp, const std::string& VertShader, const std::string& FragShader,
  			  std::vector<DescriptorSetLayout *> D);
  	VkShaderModule createShaderModule(const std::vector<char>& code);
  	static std::vector<char> readFile(const std::string& filename);  	
	void cleanup();
};

enum DescriptorSetElementType {UNIFORM, TEXTURE};

struct DescriptorSetElement {
	int binding;
	DescriptorSetElementType type;
	int size;
	Texture *tex;
};

struct DescriptorSet {
	BaseProject *BP;

	std::vector<std::vector<VkBuffer>> uniformBuffers;	std::vector<std::vector<VkDeviceMemory>> uniformBuffersMemory;
	std::vector<VkDescriptorSet> descriptorSets;
	
	std::vector<bool> toFree;

	void init(BaseProject *bp, DescriptorSetLayout *L,
		std::vector<DescriptorSetElement> E);
	void cleanup();
};


// MAIN ! 
class BaseProject {
	friend class Model;
	friend class Texture;
	friend class Pipeline;
	friend class DescriptorSetLayout;
	friend class DescriptorSet;
public:
	virtual void setWindowParameters() = 0;
    void run() {
    	setWindowParameters();
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

protected:
	uint32_t windowWidth;
	uint32_t windowHeight;
	std::string windowTitle;
	VkClearColorValue initialBackgroundColor;
	int uniformBlocksInPool;
	int texturesInPool;
	int setsInPool;

	// Lesson 12
    GLFWwindow* window;
    VkInstance instance;

    // Lesson 13
	VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

    // Lesson 14
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	
	// Lesson 19
	VkRenderPass renderPass;
	
 	VkDescriptorPool descriptorPool;

	// Lesson 22
	// L22.0 --- Debugging
	VkDebugUtilsMessengerEXT debugMessenger;
	
	// L22.1 --- depth buffer allocation (Z-buffer)
	VkImage depthImage;	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	// L22.2 --- Frame buffers
	std::vector<VkFramebuffer> swapChainFramebuffers;
	size_t currentFrame = 0;

	// L22.3 --- Synchronization objects
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	
	// Lesson 12
    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(windowWidth, windowHeight, windowTitle.c_str(), nullptr, nullptr);
    }

	virtual void localInit() = 0;

	// Lesson 12
    void initVulkan() {
		createInstance();				// L12
		setupDebugMessenger();			// L22.0
		createSurface();				// L13
		pickPhysicalDevice();			// L14
		createLogicalDevice();			// L14
		createSwapChain();				// L15
		createImageViews();				// L15
		createRenderPass();				// L19
		createCommandPool();			// L13
		createDepthResources();			// L22.1
		createFramebuffers();			// L22.2
		createDescriptorPool();			// L21

		localInit();

		createCommandBuffers();			// L22.5 (13)
		createSyncObjects();			// L22.3 
    }

	// Lesson 12 and 22.0
    void createInstance() {
    	VkApplicationInfo appInfo{};
       	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    	appInfo.pApplicationName = windowTitle.c_str();
    	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    	appInfo.pEngineName = "No Engine";
    	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);		appInfo.apiVersion = VK_API_VERSION_1_0;
		
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount =
			static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();		
		
		// For debugging [Lesson 22] - Start
		if (!checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");		}

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
			createInfo.enabledLayerCount =
				static_cast<uint32_t>(validationLayers.size());			createInfo.ppEnabledLayerNames = validationLayers.data();
			
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)
									&debugCreateInfo;
		// For debugging [Lesson 22] - End
		
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		
		if(result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create instance!");
		}
    }
    
    // Lesson 12 and L22.0
    std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;		glfwExtensions =
			glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions,
			glfwExtensions + glfwExtensionCount);			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		
		return extensions;	}
	
	// Lesson 22.0 - debug support
	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);		vkEnumerateInstanceLayerProperties(&layerCount,
					availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;
			
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;					break;
				}
			}
		
			if (!layerFound) {
				return false;			}
		}
		
		return true;    
	}

	// Lesson 22.0 - debug support
    void populateDebugMessengerCreateInfo(
    		VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    	createInfo = {};
		createInfo.sType =
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
    }

	// Lesson 22.0 - debug support
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;				return VK_FALSE;
	}

	// Lesson 22.0 - debug support
	void setupDebugMessenger() {

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);
		
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
				&debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	// Lesson 13
    void createSurface() {
    	if (glfwCreateWindowSurface(instance, window, nullptr, &surface)
    			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
    }

	// Lesson 13
    void pickPhysicalDevice() {
    	uint32_t deviceCount = 0;
    	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    	 
    	if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		
		std::cout << "Physical devices found: " << deviceCount << "\n";
		
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;			}
		}
		
		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
    }

	// Lesson 13
    bool isDeviceSuitable(VkPhysicalDevice device) {
 		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);			swapChainAdequate = !swapChainSupport.formats.empty() &&
								!swapChainSupport.presentModes.empty();
		}
		
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
		
		return indices.isComplete() && extensionsSupported && swapChainAdequate &&
						supportedFeatures.samplerAnisotropy;
	}
    
    // Lesson 13
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {		QueueFamilyIndices indices;
		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
						nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
								queueFamilies.data());
								
		int i=0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
				
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
												 &presentSupport);			if (presentSupport) {
			 	indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}			
			i++;
		}

		return indices;
	}

	// Lesson 13
	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr,					&extensionCount, nullptr);
					
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr,					&extensionCount, availableExtensions.data());
					
		std::set<std::string> requiredExtensions(deviceExtensions.begin(),
					deviceExtensions.end());
							for (const auto& extension : availableExtensions){
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	// Lesson 14
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		
		 vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
		 		&details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
				nullptr);
						if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
					&formatCount, details.formats.data());
		}
		
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
			&presentModeCount, nullptr);
		
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
					&presentModeCount, details.presentModes.data());
		}
		 		return details;
	}

	// Lesson 13
	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
				{indices.graphicsFamily.value(), indices.presentFamily.value()};		
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		
		createInfo.pQueueCreateInfos = queueCreateInfos.data();		createInfo.queueCreateInfoCount = 
			static_cast<uint32_t>(queueCreateInfos.size());
		
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount =				static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

			createInfo.enabledLayerCount = 
					static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		
		VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create logical device!");
		}
		
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	
	// Lesson 14
	void createSwapChain() {
		SwapChainSupportDetails swapChainSupport =
				querySwapChainSupport(physicalDevice);		VkSurfaceFormatKHR surfaceFormat =
				chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode =
				chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
		
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
				imageCount > swapChainSupport.capabilities.maxImageCount) {			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
										 indices.presentFamily.value()};
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional		}
		
		 createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		 createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		 createInfo.presentMode = presentMode;
		 createInfo.clipped = VK_TRUE;
		 createInfo.oldSwapchain = VK_NULL_HANDLE;
		 
		 VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
		 if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create swap chain!");
		}
		
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);		vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
				swapChainImages.data());
				
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;	}

	// Lesson 14
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
				const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		
		return availableFormats[0];
	}

	// Lesson 14
	VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
	// Lesson 14
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};			actualExtent.width = std::max(capabilities.minImageExtent.width,
					std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height,
					std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}	}

	// Lesson 14
	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			swapChainImageViews[i] = createImageView(swapChainImages[i],
													 swapChainImageFormat,
													 VK_IMAGE_ASPECT_COLOR_BIT, 1);		}
	}
	
	// Lesson 14
	VkImageView createImageView(VkImage image, VkFormat format,
								VkImageAspectFlags aspectFlags,
								uint32_t mipLevels // New in Lesson 23
								) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;		viewInfo.subresourceRange.layerCount = 1;
		VkImageView imageView;

		VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
				&imageView);
		if (result != VK_SUCCESS) {		 	PrintVkError(result);
			throw std::runtime_error("failed to create image view!");
		}
		return imageView;
	}
	
	// Lesson 19
    void createRenderPass() {
		VkAttachmentDescription depthAttachment{};		depthAttachment.format = VK_FORMAT_D32_SFLOAT;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout =
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;		depthAttachmentRef.layout = 
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    	VkAttachmentDescription colorAttachment{};		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout =
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
				VkSubpassDependency dependency{};		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments =
								{colorAttachment, depthAttachment};

		VkRenderPassCreateInfo renderPassInfo{};		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr,
					&renderPass);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create render pass!");
		}		
	}

	// Lesson 22.2 
    void createFramebuffers() {		swapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachments = {
				swapChainImageViews[i],
				depthImageView
			};			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType =
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount =
							static_cast<uint32_t>(attachments.size());;			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width; 
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;
			
			VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr,						&swapChainFramebuffers[i]);
			if (result != VK_SUCCESS) {
			 	PrintVkError(result);
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	// Lesson 13
    void createCommandPool() {
    	QueueFamilyIndices queueFamilyIndices = 
    			findQueueFamilies(physicalDevice);
    			
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = 0; // Optional
		
		VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
		if (result != VK_SUCCESS) {		 	PrintVkError(result);
			throw std::runtime_error("failed to create command pool!");
		}	}

	// Lesson 22.1
	void createDepthResources() {
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
		
		createImage(swapChainExtent.width, swapChainExtent.height, 1, depthFormat,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					depthImage, depthImageMemory);
		depthImageView = createImageView(depthImage, depthFormat,
										 VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	}

	// Lesson 22.1
	void createImage(uint32_t width, uint32_t height,
					 uint32_t mipLevels, // New in Lesson 23
					 VkFormat format,				 	 VkImageTiling tiling, VkImageUsageFlags usage,
				 	 VkMemoryPropertyFlags properties, VkImage& image,
				 	 VkDeviceMemory& imageMemory) {		
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;		imageInfo.flags = 0; // Optional
		
		VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
		if (result != VK_SUCCESS) {		 	PrintVkError(result);
		 	throw std::runtime_error("failed to create image!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
											properties);		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
								VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}		vkBindImageMemory(device, image, imageMemory, 0);
	}

	// New - Lesson 23
	void generateMipmaps(VkImage image, VkFormat imageFormat,
						 int32_t texWidth, int32_t texHeight,
						 uint32_t mipLevels) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat,
							&formatProperties);

		if (!(formatProperties.optimalTilingFeatures &					VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
		
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;		int32_t mipWidth = texWidth;		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) { 
			barrier.subresourceRange.baseMipLevel = i - 1;			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			
			vkCmdPipelineBarrier(commandBuffer,								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
								 0, nullptr, 0, nullptr,
								 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,
								   mipHeight > 1 ? mipHeight/2:1, 1};
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;
			
			vkCmdBlitImage(commandBuffer, image,
						   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						   image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
						   &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			
			vkCmdPipelineBarrier(commandBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,								 0, nullptr, 0, nullptr,
								 1, &barrier);
			if (mipWidth > 1) mipWidth /= 2;			if (mipHeight > 1) mipHeight /= 2;
		}		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;		vkCmdPipelineBarrier(commandBuffer,
							 VK_PIPELINE_STAGE_TRANSFER_BIT,
							 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
							 0, nullptr, 0, nullptr,
							 1, &barrier);
		endSingleTimeCommands(commandBuffer);
	}
	
	// New - Lesson 23
	void transitionImageLayout(VkImage image, VkFormat format,
					VkImageLayout oldLayout, VkImageLayout newLayout,
					uint32_t mipLevels) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		barrier.srcAccessMask = 0;		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(commandBuffer,
								VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
								VK_ACCESS_TRANSFER_WRITE_BIT, 0,
								0, nullptr, 0, nullptr, 1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}
	
	// New - Lesson 23
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t						   width, uint32_t height) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
		
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;		region.imageOffset = {0, 0, 0};
		region.imageExtent = {width, height, 1};
		
		vkCmdCopyBufferToImage(commandBuffer, buffer, image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);
	}
	
	// New - Lesson 23
	VkCommandBuffer beginSingleTimeCommands() { 
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;		
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
		
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		
		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
		return commandBuffer;
	}
	
	// New - Lesson 23
	void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);
		
		VkSubmitInfo submitInfo{};		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);
		
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}
	


	// Lesson 22.4
	
	// Lesson 21
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
					  VkMemoryPropertyFlags properties,
					  VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		VkResult result =
				vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);		if (result != VK_SUCCESS) {		 	PrintVkError(result);
			throw std::runtime_error("failed to create vertex buffer!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
		
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex =				findMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(device, &allocInfo, nullptr,
				&bufferMemory);		if (result != VK_SUCCESS) {		 	PrintVkError(result);
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}
		
		vkBindBufferMemory(device, buffer, bufferMemory, 0);	
	}
	
	// Lesson 21
	uint32_t findMemoryType(uint32_t typeFilter,
							VkMemoryPropertyFlags properties) {
		 VkPhysicalDeviceMemoryProperties memProperties;
		 vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
		 
		 for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		 	if ((typeFilter & (1 << i)) && 
		 		(memProperties.memoryTypes[i].propertyFlags & properties) ==
		 				properties) {
				return i;
			}
		}
		
		throw std::runtime_error("failed to find suitable memory type!");
	}
    
    // Lesson 21
	void createDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(uniformBlocksInPool *
															 swapChainImages.size());
		// New - Lesson 23
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(texturesInPool *
															 swapChainImages.size());
		//

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());;		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(setsInPool * swapChainImages.size());
		
		VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr,
									&descriptorPool);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}
	
	virtual void populateCommandBuffer(VkCommandBuffer commandBuffer, int i) = 0;

	// Lesson 22.5 (and 13)
    void createCommandBuffers() {
    	// Lesson 13
    	commandBuffers.resize(swapChainFramebuffers.size());
    	
    	VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
		
		VkResult result = vkAllocateCommandBuffers(device, &allocInfo,				commandBuffers.data());
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to allocate command buffers!");
		}
		
		// Lesson 22.5 --- Draw calls
		// This is where the commands that actually draw something on screen are!
		for (size_t i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) !=
						VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");			}
			
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;			renderPassInfo.renderPass = renderPass; 
			renderPassInfo.framebuffer = swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = swapChainExtent;
	
			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = initialBackgroundColor;
			clearValues[1].depthStencil = {1.0f, 0};
	
			renderPassInfo.clearValueCount =
							static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
					VK_SUBPASS_CONTENTS_INLINE);			
	

			populateCommandBuffer(commandBuffers[i], i);
			

			vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");			}
		}
	}
    
    // Lesson 22.5
    void createSyncObjects() {
    	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    	    	
    	VkSemaphoreCreateInfo semaphoreInfo{};		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkResult result1 = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
								&imageAvailableSemaphores[i]);
			VkResult result2 = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
								&renderFinishedSemaphores[i]);
			VkResult result3 = vkCreateFence(device, &fenceInfo, nullptr,
								&inFlightFences[i]);
			if (result1 != VK_SUCCESS ||				result2 != VK_SUCCESS ||
				result3 != VK_SUCCESS) {
			 	PrintVkError(result1);
			 	PrintVkError(result2);
			 	PrintVkError(result3);
				throw std::runtime_error("failed to create synchronization objects for a frame!!");
			}
		}
	}
    
    // Lesson 22.6 --- Main Rendering Loop
    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }
        
        vkDeviceWaitIdle(device);
    }
    
    // Lesson 22.6
    void drawFrame() {
		vkWaitForFences(device, 1, &inFlightFences[currentFrame],
						VK_TRUE, UINT64_MAX);
		
		uint32_t imageIndex;
		
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
				imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {			vkWaitForFences(device, 1, &imagesInFlight[imageIndex],
							VK_TRUE, UINT64_MAX);
		}
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];
		
		updateUniformBuffer(imageIndex);
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;		VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] =
			{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
		VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		
		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
				inFlightFences[currentFrame]) != VK_SUCCESS) {			throw std::runtime_error("failed to submit draw command buffer!");
		}
		
		VkPresentInfoKHR presentInfo{};		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		
		VkSwapchainKHR swapChains[] = {swapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional
		
		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

	virtual void updateUniformBuffer(uint32_t currentImage) = 0;

	virtual void localCleanup() = 0;
	
	// All lessons
	
    void cleanup() {
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);		vkFreeMemory(device, depthImageMemory, nullptr);

		for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
		}
		
		vkFreeCommandBuffers(device, commandPool,				static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		vkDestroyRenderPass(device, renderPass, nullptr);		for (size_t i = 0; i < swapChainImageViews.size(); i++){
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
		}
		
		vkDestroySwapchainKHR(device, swapChain, nullptr);
		
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    	
    	
		localCleanup();
    	
    	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
    	}
    	
    	vkDestroyCommandPool(device, commandPool, nullptr);
    	
 		vkDestroyDevice(device, nullptr);
		
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		
		vkDestroySurfaceKHR(instance, surface, nullptr);
    	vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }
	
};




void Model::loadModel(std::string file) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;	std::vector<tinyobj::material_t> materials;
	std::string warn, err;
	
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
						  file.c_str())) {
		throw std::runtime_error(warn + err);	}
	
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {			Vertex vertex{};
			
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			
			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],				1 - attrib.texcoords[2 * index.texcoord_index + 1] 
			};

			vertex.norm = {
				attrib.normals[3 * index.normal_index + 0],				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};
			
			vertices.push_back(vertex);
			indices.push_back(vertices.size()-1);
		}
	}
	
	
}

// Lesson 21
void Model::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	
	BP->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						vertexBuffer, vertexBufferMemory);

	void* data;
	vkMapMemory(BP->device, vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(BP->device, vertexBufferMemory);			
}

void Model::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	BP->createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
							 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							 indexBuffer, indexBufferMemory);	void* data;
	vkMapMemory(BP->device, indexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(BP->device, indexBufferMemory);}

void Model::init(BaseProject *bp, std::string file) {
	BP = bp;
	loadModel(file);
	createVertexBuffer();
	createIndexBuffer();
}

void Model::cleanup() {
   	vkDestroyBuffer(BP->device, indexBuffer, nullptr);
   	vkFreeMemory(BP->device, indexBufferMemory, nullptr);
	vkDestroyBuffer(BP->device, vertexBuffer, nullptr);
   	vkFreeMemory(BP->device, vertexBufferMemory, nullptr);
}







void Texture::createTextureImage(std::string file) {
	int texWidth, texHeight, texChannels;	stbi_uc* pixels = stbi_load(file.c_str(), &texWidth, &texHeight,
						&texChannels, STBI_rgb_alpha);
	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");	}

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	mipLevels = static_cast<uint32_t>(std::floor(
					std::log2(std::max(texWidth, texHeight)))) + 1;
	
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	 
	BP->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	  						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
	  						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	  						stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(BP->device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(BP->device, stagingBufferMemory);
	
	stbi_image_free(pixels);
	
	BP->createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage,
				textureImageMemory);
				
	BP->transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
	BP->copyBufferToImage(stagingBuffer, textureImage,			static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	BP->generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
					texWidth, texHeight, mipLevels);

	vkDestroyBuffer(BP->device, stagingBuffer, nullptr);
	vkFreeMemory(BP->device, stagingBufferMemory, nullptr);
}

void Texture::createTextureImageView() {
	textureImageView = BP->createImageView(textureImage,
									   VK_FORMAT_R8G8B8A8_SRGB,
									   VK_IMAGE_ASPECT_COLOR_BIT,
									   mipLevels);
}
	
void Texture::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(mipLevels);
	
	VkResult result = vkCreateSampler(BP->device, &samplerInfo, nullptr,
									  &textureSampler);
	if (result != VK_SUCCESS) {	 	PrintVkError(result);
	 	throw std::runtime_error("failed to create texture sampler!");
	}
}
	


void Texture::init(BaseProject *bp, std::string file) {
	BP = bp;
	createTextureImage(file);
	createTextureImageView();
	createTextureSampler();
}

void Texture::cleanup() {
   	vkDestroySampler(BP->device, textureSampler, nullptr);
   	vkDestroyImageView(BP->device, textureImageView, nullptr);
	vkDestroyImage(BP->device, textureImage, nullptr);
	vkFreeMemory(BP->device, textureImageMemory, nullptr);
}





void Pipeline::init(BaseProject *bp, const std::string& VertShader, const std::string& FragShader,
					std::vector<DescriptorSetLayout *> D) {
	BP = bp;
	
	auto vertShaderCode = readFile(VertShader);
	auto fragShaderCode = readFile(FragShader);
	
	std::cout << "Vertex shader len: " <<
				vertShaderCode.size() << "\n";
	std::cout << "Fragment shader len: " <<
				fragShaderCode.size() << "\n";
	
	VkShaderModule vertShaderModule =
			createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule =
			createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType =
    		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType =
    		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] =
    		{vertShaderStageInfo, fragShaderStageInfo};

	// Lesson 17
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription = Vertex::getBindingDescription();	auto attributeDescriptions = Vertex::getAttributeDescriptions();
			
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount =
			static_cast<uint32_t>(attributeDescriptions.size());	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions =
			attributeDescriptions.data();		

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType =		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Lesson 19
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;	viewport.width = (float) BP->swapChainExtent.width;
	viewport.height = (float) BP->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	VkRect2D scissor{};
	scissor.offset = {0, 0};	scissor.extent = BP->swapChainExtent;
	
	VkPipelineViewportStateCreateInfo viewportState{};	viewportState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	
	VkPipelineRasterizationStateCreateInfo rasterizer{};	rasterizer.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;	colorBlendAttachment.srcColorBlendFactor =
			VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor =
			VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp =
			VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor =
			VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor =
			VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp =
			VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
	
	// Lesson 21
	std::vector<VkDescriptorSetLayout> DSL(D.size());
	for(int i = 0; i < D.size(); i++) {
		DSL[i] = D[i]->descriptorSetLayout;
	}
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = DSL.size();	pipelineLayoutInfo.pSetLayouts = DSL.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	
	VkResult result = vkCreatePipelineLayout(BP->device, &pipelineLayoutInfo, nullptr,
				&pipelineLayout);
	if (result != VK_SUCCESS) {
	 	PrintVkError(result);
		throw std::runtime_error("failed to create pipeline layout!");
	}
	
	// Lesson 19
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = 
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType =
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = BP->renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional	pipelineInfo.basePipelineIndex = -1; // Optional
	
	result = vkCreateGraphicsPipelines(BP->device, VK_NULL_HANDLE, 1,
			&pipelineInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS) {	 	PrintVkError(result);
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	
	vkDestroyShaderModule(BP->device, fragShaderModule, nullptr);
	vkDestroyShaderModule(BP->device, vertShaderModule, nullptr);
}

// Lesson 18
std::vector<char> Pipeline::readFile(const std::string& filename) {		std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}
	
	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);
	 
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	 
	file.close();	 
	return buffer;
}

// Lesson 18
VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
	VkShaderModule shaderModule;

	VkResult result = vkCreateShaderModule(BP->device, &createInfo, nullptr,
					&shaderModule);
	if (result != VK_SUCCESS) {
	 	PrintVkError(result);
		throw std::runtime_error("failed to create shader module!");
	}
	
	return shaderModule;
}

void Pipeline::cleanup() {
		vkDestroyPipeline(BP->device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(BP->device, pipelineLayout, nullptr);
}

void DescriptorSetLayout::init(BaseProject *bp, std::vector<DescriptorSetLayoutBinding> B) {
	BP = bp;
	
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	bindings.resize(B.size());
	for(int i = 0; i < B.size(); i++) {
		bindings[i].binding = B[i].binding;
		bindings[i].descriptorType = B[i].type;
		bindings[i].descriptorCount = 1;
		bindings[i].stageFlags = B[i].flags;		bindings[i].pImmutableSamplers = nullptr;
	}
	
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
	layoutInfo.pBindings = bindings.data();
	
	VkResult result = vkCreateDescriptorSetLayout(BP->device, &layoutInfo,
								nullptr, &descriptorSetLayout);	if (result != VK_SUCCESS) {
		PrintVkError(result);
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void DescriptorSetLayout::cleanup() {
    	vkDestroyDescriptorSetLayout(BP->device, descriptorSetLayout, nullptr);	
}

void DescriptorSet::init(BaseProject *bp, DescriptorSetLayout *DSL,
						 std::vector<DescriptorSetElement> E) {
	BP = bp;
	
	// Create uniform buffer
	uniformBuffers.resize(E.size());
	uniformBuffersMemory.resize(E.size());
	toFree.resize(E.size());	for (int j = 0; j < E.size(); j++) {
		uniformBuffers[j].resize(BP->swapChainImages.size());
		uniformBuffersMemory[j].resize(BP->swapChainImages.size());		if(E[j].type == UNIFORM) {			for (size_t i = 0; i < BP->swapChainImages.size(); i++) {
				VkDeviceSize bufferSize = E[j].size;
				BP->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
									 	 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
									 	 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
									 	 uniformBuffers[j][i], uniformBuffersMemory[j][i]);
			}
			toFree[j] = true;
		} else {
			toFree[j] = false;
		}
	}
	
	// Create Descriptor set
	std::vector<VkDescriptorSetLayout> layouts(BP->swapChainImages.size(),
											   DSL->descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = BP->descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(BP->swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();
	
	descriptorSets.resize(BP->swapChainImages.size());
	
	VkResult result = vkAllocateDescriptorSets(BP->device, &allocInfo,
										descriptorSets.data());
	if (result != VK_SUCCESS) {
		PrintVkError(result);
		throw std::runtime_error("failed to allocate descriptor sets!");	}
	
	for (size_t i = 0; i < BP->swapChainImages.size(); i++) {
		std::vector<VkWriteDescriptorSet> descriptorWrites(E.size());		for (int j = 0; j < E.size(); j++) {
			if(E[j].type == UNIFORM) {				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniformBuffers[j][i];
				bufferInfo.offset = 0;				bufferInfo.range = E[j].size;
				
				descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[j].dstSet = descriptorSets[i];
				descriptorWrites[j].dstBinding = E[j].binding;
				descriptorWrites[j].dstArrayElement = 0;
				descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;				descriptorWrites[j].descriptorCount = 1;
				descriptorWrites[j].pBufferInfo = &bufferInfo;
			} else if(E[j].type == TEXTURE) {
				VkDescriptorImageInfo imageInfo{};				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = E[j].tex->textureImageView;
				imageInfo.sampler = E[j].tex->textureSampler;
		
				descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[j].dstSet = descriptorSets[i];
				descriptorWrites[j].dstBinding = E[j].binding;				descriptorWrites[j].dstArrayElement = 0;
				descriptorWrites[j].descriptorType =
											VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[j].descriptorCount = 1;
				descriptorWrites[j].pImageInfo = &imageInfo;
			}
		}		
		vkUpdateDescriptorSets(BP->device,
						static_cast<uint32_t>(descriptorWrites.size()),
						descriptorWrites.data(), 0, nullptr);	}
}

void DescriptorSet::cleanup() {
	for(int j = 0; j < uniformBuffers.size(); j++) {
		if(toFree[j]) {
			for (size_t i = 0; i < BP->swapChainImages.size(); i++) {
				vkDestroyBuffer(BP->device, uniformBuffers[j][i], nullptr);
				vkFreeMemory(BP->device, uniformBuffersMemory[j][i], nullptr);
			}
		}
	}
}