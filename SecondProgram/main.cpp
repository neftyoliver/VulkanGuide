
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

int main()
{
	/**
	* 이제 최신 C의 코딩 스타일을 적용합니다.
	* 
	* 먼저 불칸을 사용하기 위한 준비 작업을 합니다.
	* 
	* 1. 인스턴스 생성
	* 2. 물리 장치 열거
	* 3. 물리 장치 고르기
	* 4. 논리 장치 생성
	* 
	*/

	/*
	* 불칸 인스턴스를 생성합니다.
	*
	* 인스턴스 생성 정보, 어플리케이션 정보가 있어야 인스턴스를 만들 수 있어요.
	*/

	VkApplicationInfo applicationInfo;
	VkInstanceCreateInfo instanceCreateInfo;
	VkInstance theVulkan;

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

	VkPhysicalDevice physicalDevices[MAX_DEVICE_SUPPORT_NUMBER]; /* 장치를 최대 8개를 응용하기로 함 */
	uint32_t physicalDeviceSelection = 0;
	uint32_t physicalDeviceCount = 0;

	vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, NULL);
	vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, physicalDevices);


	/* 가장 좋은 물리 장치를 고릅니다. */

	uint32_t physicalDeviceRatings[MAX_DEVICE_SUPPORT_NUMBER] = { 0 };
	uint32_t bestRating = 0;
	VkPhysicalDeviceProperties physicalDeviceProperties;

	for (int i = 0; i < physicalDeviceCount; i += 1)
	{
		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

		printf("Physical Device: %s.", physicalDeviceProperties.deviceName);

		physicalDeviceRatings[i] = 0;

		physicalDeviceRatings[i] += physicalDeviceProperties.limits.maxImageDimension2D >> 8;

		printf(" rating: %u\n", physicalDeviceRatings[i]);
	}

	printf("\n");

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
		printf("Logical device created with Device '%s' and queue family index '%i\n'", physicalDeviceProperties.deviceName, queueFamilySelection);
	}
	else
	{
		printf("Some thing was wrong while creating device with code: %i\n", deviceCreateResult);
	}

	/*
	* 논리 장치로부터 큐를 갖고옵니다.
	* 큐가 여러개 생성됐을 경우 여러번 호출하여 큐를 갖고올 수 있습니다.
	* 각 큐는 큐 페밀리에 의해 관리됩니다.
	* 때문에 큐를 갖고오기 위해서는 큐 페밀리를 고르고 해당 큐 페밀리의 몇번째 큐인지 알려야합니다.
	* 
	* 지금은 고른 큐에 0번째를 갖고오기로 합니다.
	*/
	VkQueue queue;
	vkGetDeviceQueue(device, queueFamilySelection, 0, &queue);

	/* 준비작업이 완료됐습니다! 이제 불칸의 요소들을 마음껏 쓸 수 있겠군요. */

	/* 이제 리소스를 만들어 할당해주겠습니다. 리소스는 버퍼와 이미지로 나뉩니다. */
	VkFormat theImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	VkExtent3D theImageExtent = { 256, 256, 0};
	VkImage theImage;
	VkDeviceMemory theImageMemory;
	VkImageView theImageView;

	VkBuffer theBuffer;
	VkDeviceMemory theBufferMemory;
	VkBufferView theBufferView;

	/* 이미지를 먼저 다뤄줍니다. 이미지를 먼저 만들고 이미지를 할당한다음 이미지 뷰를 만들어 뷰를 통해 사용하죠. */

	VkImageCreateInfo theImageCreateInfo;
	theImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	theImageCreateInfo.pNext = NULL;
	theImageCreateInfo.flags = 0;
	theImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	theImageCreateInfo.format = theImageFormat;
	theImageCreateInfo.extent = theImageExtent;
	theImageCreateInfo.mipLevels = 1;
	theImageCreateInfo.arrayLayers = 1;
	theImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	theImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	theImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	theImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theImageCreateInfo.queueFamilyIndexCount = 0;
	theImageCreateInfo.pQueueFamilyIndices = NULL;
	theImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	checkOut("Creating image ", vkCreateImage(device, &theImageCreateInfo, NULL, &theImage));

	VkMemoryRequirements theImageMemoryRequirements;
	vkGetImageMemoryRequirements(device, theImage, &theImageMemoryRequirements);

	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevices[physicalDeviceSelection], &physicalDeviceMemoryProperties);

	uint32_t memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	uint32_t memoryIndex = 0;

	/*
	* 버퍼에 적합한 메모리 유형을 찾아냅니다. 여러번 복습하길 권하는 부분입니다!
	* 
	* 장치에는 VkMemoryPropertyFlagBits의 조합으로 이뤄진 여러 유형의 메모리 타입이 존재합니다.
	* 이들 중 우리가 필요로 하는 플레그를 전부 혹은 그 이상 지원하는 메모리타입에다 메모리를 할당하죠.
	* 
	* 다음 과정을 통해서 메모리 타입을 습니다.
	* 
	* 버퍼에게서 쿼리한 메모리 유형 i번째가 가 0이 아닌데 장치의 메모리타입 i번째.propertyFlags가 memoryFlags와 동치이거나 포괄할 때 그 메모리 타입을 사용한다.
	* 
	* 쉽지 않은 알고리즘입니다. 요약하자면 { 버퍼의 요구 & 사용자가 신청한 타입 & 메모리 타입 } 으로 하여
	* 버퍼가 지원하는 기능과 메모리 타입에서 가능한 기능의 교집합 중 하나를 선택한다는 것이죠.
	* 
	* 예를 들어
	* 
	* 버퍼의 지원:   0110
	* 메모리의 지원: 0011
	* 사용자의 요청: 0010
	* 
	* 결과:         0010
	* 
	* 이렇게 되는겁니다.
	* 
	* 이러한 방식은 이미지에도 적용되는 만큼 숙지를 위해 함수로 만들지 않겠습니다.
	*/
	for (int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i += 1)
	{//((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)


		if ( (theImageMemoryRequirements.memoryTypeBits & 1 << i) &&
			 (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags )
		{
			memoryIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo theImageMemoryAllocateInfo;
	theImageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	theImageMemoryAllocateInfo.pNext = NULL;
	theImageMemoryAllocateInfo.allocationSize = theImageMemoryRequirements.size;
	theImageMemoryAllocateInfo.memoryTypeIndex = 2;

	checkOut("Allocating memory ", vkAllocateMemory(device, &theImageMemoryAllocateInfo, NULL, &theImageMemory));




















	/* 모든 작업이 끝났고, 생성된 데이터들을 파괴합니다. */

	printf("\n");

	vkDestroyImage(device, theImage, NULL);

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(theVulkan, NULL);

	return 0;
}
