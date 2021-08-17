#pragma once

#include <vulkan/vulkan.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "forthProgram.hpp"



/* the vulkan helper!
 * using single vulkan instance and using multiple device and queues.
 * 
 * logical device will created with pre-
 */
namespace vkhelp {
	typedef enum class VkHelperResult {
		SUCCESS = 0,
		OUT_OF_INDEX_BOUND = 1,
		LAYER_UNEXIST = 2,
		EXTENSION_UNEXIST = 3,

	};

	uint32_t findQueueFamily();

	uint32_t ratePhysicalDevice(vk::PhysicalDevice physicalDevice);

	uint32_t findMemoryType();

	class VulkanHelper {
	private:
		std::vector<vk::LayerProperties> vecAvailableLayers;
		std::vector<vk::ExtensionProperties> vecAvailableExtensions;

		std::vector<const char *> vecLayers;
		std::vector<const char *> vecExtensions;

        vk::Instance vulkanInstance;
		std::vector<vk::PhysicalDevice> vecPhysicalDevices;
		vk::PhysicalDevice selectedDevice;

		std::vector<vk::Device> vecDevices;
		std::vector<vk::Queue> vecQueues;

		VkHelperResult addLayer(const char * paramLayerName);
		VkHelperResult addExtension(const char* paramExtensionName);

	public:
		VulkanHelper(const char* programName);
		
		std::vector<vk::PhysicalDevice> getPhysicalDevices();
		VkHelperResult selectPhysicalDevice(uint32_t index);
		vk::PhysicalDevice getSelectedPhysicalDevice();


		//TODO: 큐와 로지컬 디바이스 

		~VulkanHelper();
	};
};
