 
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

/* 여러분들의 편의를 위해서 만든 함수입니다. */
void checkOut(const char* name, VkResult functionResult)
{
	if (functionResult == VK_SUCCESS)
	{
		printf("%s Success!\n", name);

	}
	else
	{
		printf("%s Faild! with error: %d\n", name, functionResult); //실패할 경우 에러코드를 뱉는데, 이걸 출력합니다.
	}
}

/**
* 본 프로그램은 총 5단계로 구성됩니다.
* 
* 1. 어플리케이션 정보 서술과 인스턴스 생성
* 2. 물리 장치 열거와 레이팅 매기기
* 3. 레이팅을 근거로 가장 좋은 장치 고르기
* 4. 고른 장치 안에서 큐 페밀리를 고르기
* 5. 고른 큐 페밀리와 큐 생성 정보로 논리 장치 만들기
* 6. 디바이스로부터 큐를 갖고오기
* 7. 만들어진 데이터들 삭제
*/
int main()
{
	/* C스타일을 채택하여 모든 변수는 위에다 위치시키도록 합니다. */
	VkApplicationInfo applicationInfo;
	VkInstanceCreateInfo instanceCreateInfo;
	VkInstance theVulkan;

	uint32_t physicalDeviceSelection = 0;
	VkPhysicalDevice physicalDevices[MAX_DEVICE_SUPPORT_NUMBER]; /* 장치를 최대 8개를 응용하기로 함 */
	uint32_t physicalDeviceCount = 0;

	uint32_t bestRating = 0;
	uint32_t physicalDeviceRatings[MAX_DEVICE_SUPPORT_NUMBER] = { 0 };
	VkPhysicalDeviceProperties physicalDeviceProperties;

	uint32_t queueFamilySelection = 0;
	VkQueueFamilyProperties queueFamilyProperties[8];
	uint32_t queueFamilyPropertiesCount;

	const float queuePriorities = 1.0;
	VkDevice device;
	VkDeviceQueueCreateInfo deviceQueueCreateInfos[1];
	VkDeviceCreateInfo deviceCreateInfo;

	VkResult deviceCreateResult;

	VkQueue queue;

	int i = 0;
	int j = 0;

	/*
	* 불칸 인스턴스를 생성합니다.
	* 
	* 인스턴스 생성 정보, 어플리케이션 정보가 있어야 인스턴스를 만들 수 있어요.
	*/
	
	
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
	
	checkOut("Creating instance ", vkCreateInstance(&instanceCreateInfo, NULL, &theVulkan));
	
	
	printf("\n");

	/* 
	* 물리 장치를 열거하고 갖고옵니다.
	* 이 때 물리장치를 고를 변수도 만들어줍니다. 물리 장치는 여러개일 수 있기에 배열로 가져옵니다.
	*/
	
	vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, NULL);
	vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, physicalDevices);
	

	/* 가장 좋은 물리 장치를 고릅니다. */
	
	for (i = 0; i < physicalDeviceCount; i += 1)
	{
		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
			
		printf("Physical Device: %s.", physicalDeviceProperties.deviceName);

		physicalDeviceRatings[i] = 0;

		physicalDeviceRatings[i] += physicalDeviceProperties.limits.maxImageDimension2D >> 8;

		printf(" rating: %u\n", physicalDeviceRatings[i]);
	}

	printf("\n");

	for (i = 0; i < physicalDeviceCount; i += 1)
	{
		if (bestRating < physicalDeviceRatings[i])
		{
			bestRating = physicalDeviceRatings[i];
			physicalDeviceSelection = i;
		}
	}

	vkGetPhysicalDeviceProperties(physicalDevices[physicalDeviceSelection], &physicalDeviceProperties);

	printf("selected device: %s, device rating: %i\n", physicalDeviceProperties.deviceName, physicalDeviceRatings[physicalDeviceSelection]);
	

	/* 앞서 고른 장치 안에서 큐 페밀리를 적절하게 골라줍니다. */
	

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[physicalDeviceSelection], &queueFamilyPropertiesCount, NULL);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[physicalDeviceSelection], &queueFamilyPropertiesCount, queueFamilyProperties);

	for (j = 0; j < queueFamilyPropertiesCount; j += 1)
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
	
	deviceCreateResult = vkCreateDevice(physicalDevices[physicalDeviceSelection], &deviceCreateInfo, NULL, &device);
	if (deviceCreateResult == VK_SUCCESS)
	{
		printf("Logical device created with Device '%s' and queue family index '%i'", physicalDeviceProperties.deviceName, queueFamilySelection);
	}

	/*
	* 논리 장치로부터 큐를 갖고옵니다.
	* 큐가 여러개 생성됐을 경우 여러번 호출하여 큐를 갖고올 수 있습니다.
	* 각 큐는 큐 페밀리에 의해 관리됩니다.
	* 때문에 큐를 갖고오기 위해서는 큐 페밀리를 고르고 해당 큐 페밀리의 몇번째 큐인지 알려야합니다.
	* 
	* 지금은 고른 큐에 0번째를 갖고오기로 합니다.
	*/
	
	vkGetDeviceQueue(device, queueFamilySelection, 0, &queue);

	printf("\n");

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(theVulkan, NULL);

	return 0;
}
