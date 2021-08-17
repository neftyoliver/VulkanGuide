

/*
* Copyright. 2021 네프티 MIT.
*
* 불칸을 소개하고 설명하는 프로젝트입니다.
*/

/* 프린트 할 게 많아요! */
#include <stdio.h>

/* 불칸을 갖고옵니다 */
#include <vulkan/vulkan.h>

/* 최대 16개의 장치를 지원합니다! */
#define MAX_DEVICE_SUPPORT_NUMBER 8

int main()
{
	/*
	* 불칸 인스턴스를 생성합니다.
	*
	* 인스턴스 생성 정보, 어플리케이션 정보가 있어야 인스턴스를 만들 수 있어요.
	*/
	VkApplicationInfo applicationInfo;
	VkInstanceCreateInfo instanceCreateInfo;
	VkInstance theVulkan;
	{
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);
		applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		applicationInfo.pApplicationName = "Hello, world";
		applicationInfo.pEngineName = "None";

		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = NULL;
		instanceCreateInfo.enabledExtensionCount = 0;
		instanceCreateInfo.ppEnabledExtensionNames = NULL;

		VkResult instanceCreateResult = vkCreateInstance(&instanceCreateInfo, NULL, &theVulkan);
		if (instanceCreateResult == VK_SUCCESS)
		{
			printf("Hello, Vulkan!\n");

		}
		else
		{
			printf("Create instance faild! with error: %d\n", instanceCreateResult); //실패할 경우 에러코드를 뱉는데, 이걸 출력합니다.
		}
	}
	printf("\n");

	/*
	* 물리 장치를 열거하고 갖고옵니다.
	* 이 때 물리장치를 고를 변수도 만들어줍니다. 물리 장치는 여러개일 수 있기에 배열로 가져옵니다.
	*/
	uint32_t physicalDeviceSelection = 0;
	VkPhysicalDevice physicalDevices[MAX_DEVICE_SUPPORT_NUMBER]; /* 장치 16개를 응용하기로 함 */
	uint32_t physicalDeviceCount = 0;
	{
		vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, NULL);
		vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, physicalDevices);
	}

	/* 가장 좋은 물리 장치를 고릅니다. */
	uint32_t physicalDeviceRatings[MAX_DEVICE_SUPPORT_NUMBER] = { 0 };
	VkPhysicalDeviceProperties physicalDeviceProperties;
	{

		for (int i = 0; i < physicalDeviceCount; i += 1)
		{
			vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

			printf("Physical Device: %s.", physicalDeviceProperties.deviceName);

			physicalDeviceRatings[i] = 0;

			physicalDeviceRatings[i] += physicalDeviceProperties.limits.maxImageDimension2D >> 8;

			printf(" rating: %u\n", physicalDeviceRatings[i]);
		}

		printf("\n");

		uint32_t bestRating = 0;
		for (int i = 0; i < physicalDeviceCount; i += 1)
		{
			if (bestRating < physicalDeviceRatings[i])
			{
				bestRating = physicalDeviceRatings[i];
				physicalDeviceSelection = i;
			}
		}

		vkGetPhysicalDeviceProperties(physicalDevices[physicalDeviceSelection], &physicalDeviceProperties);

		printf("selected device: %s, device rating: %i\n", physicalDeviceProperties.deviceName, physicalDeviceRatings[physicalDeviceSelection]);
	}

	/* 앞서 고른 장치 안에서 큐 페밀리를 적절하게 골라줍니다. */
	uint32_t queueFamilySelection = 0;
	VkQueueFamilyProperties queueFamilyProperties[8];
	uint32_t queueFamilyPropertiesCount;

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[physicalDeviceSelection], &queueFamilyPropertiesCount, NULL);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[physicalDeviceSelection], &queueFamilyPropertiesCount, queueFamilyProperties);

	for (int j = 0; j < queueFamilyPropertiesCount; j += 1)
	{
		if (queueFamilyProperties[j].queueFlags & (VK_QUEUE_GRAPHICS_BIT & VK_QUEUE_COMPUTE_BIT & VK_QUEUE_TRANSFER_BIT))
		{
			queueFamilySelection = j;
			break;
		}
	}

	printf("Queue family selection: %i\n", queueFamilySelection);
	printf("\n");

	/* 논리 장치를 생성합니다. 이 때 앞서 취합한 정보들을 사용하게 되죠. */
	const float queuePriorities = 1.0;
	VkDevice device;
	VkDeviceQueueCreateInfo deviceQueueCreateInfos[1];
	VkDeviceCreateInfo deviceCreateInfo;
	{

		deviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfos[0].pNext = NULL;
		deviceQueueCreateInfos[0].flags = 0;
		deviceQueueCreateInfos[0].pQueuePriorities = &queuePriorities;
		deviceQueueCreateInfos[0].queueCount = 1; //큐는 하나만 만들기로 합니다.
		deviceQueueCreateInfos[0].queueFamilyIndex = queueFamilySelection;

		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = NULL;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pEnabledFeatures = NULL;
		deviceCreateInfo.enabledExtensionCount = 0;



		VkResult deviceCreateResult = vkCreateDevice(physicalDevices[physicalDeviceSelection], &deviceCreateInfo, NULL, &device);
		if (deviceCreateResult == VK_SUCCESS)
		{
			printf("Logical device created with Device '%s' and queue family index '%i'", physicalDeviceProperties.deviceName, queueFamilySelection);
		}
	}

	VkQueue queue;
	{
		vkGetDeviceQueue(device, queueFamilySelection, 0, &queue);
	}

	printf("\n");

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(theVulkan, NULL);

	return 0;
}
