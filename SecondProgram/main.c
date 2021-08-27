
/*
* Copyright. 2021 네프티 MIT.
*
* 불칸을 소개하고 설명하는 프로젝트입니다.
*/

/* 프린트 할 게 많아요! */
#include <stdio.h>

/* 불칸을 갖고옵니다 */
#include <vulkan/vulkan.h>

/* 최대 8개의 장치를 지원합니다! */
#define MAX_DEVICE_SUPPORT_NUMBER 8

/* 쉐이더 바이너리 코드들의 사이즈. 쉐이더마다 파일 크기를 정확히 할것! */
#define VERTEX_SHADER_BINARY_SIZE 1428
#define FRAGMENT_SHADER_BINAY_SIZE 536

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

void loadShader(const char* fileName, uint32_t size, char** output)
{
	printf("Loading shader %s!\n", fileName);

	FILE* fp;

	fp = fopen(fileName, "rb");

	fread(output, size, 1, fp);
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

	const char* layers = { "VK_LAYER_KHRONOS_validation" };

	/*
	* 불칸 인스턴스를 생성합니다.
	*
	* 인스턴스 생성 정보, 어플리케이션 정보가 있어야 인스턴스를 만들 수 있어요.
	*/

	VkApplicationInfo applicationInfo;
	VkInstanceCreateInfo instanceCreateInfo;
	VkInstance theVulkan;

	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = NULL;
	applicationInfo.apiVersion = VK_MAKE_VERSION(1, 1, 0);
	applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	applicationInfo.pApplicationName = "Hello, world";
	applicationInfo.pEngineName = "None";

	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = 1;
	instanceCreateInfo.ppEnabledLayerNames = &layers;
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
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = NULL;
	deviceCreateInfo.enabledExtensionCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = NULL;
	deviceCreateInfo.pEnabledFeatures = NULL;

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
	VkExtent3D theImageExtent = { 256, 256, 1};
	VkImage theImage;
	VkDeviceMemory theImageMemory;
	VkImageView theImageView;

	float vertexData[] = {
		 0.0, -0.5,
		 0.5,  0.5,
		-0.5,  0.5
	};

	VkBuffer theBuffer;
	VkDeviceMemory theBufferMemory;
	VkBufferView theBufferView = NULL;

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
	* 리소스에서 가능하면서 메모리에서도 가능한 메모리 유형을 찾아냅니다. 여러번 복습하길 권하는 부분입니다!
	* 
	* 장치에는 VkMemoryPropertyFlagBits의 조합으로 이뤄진 여러 유형의 메모리 타입이 존재합니다.
	* 이들 중 우리가 필요로 하는 플레그를 전부 혹은 그 이상 지원하는 메모리타입에다 메모리를 할당하죠.
	* 
	* 이 때 만들려고 하는 리소스 또한 해당 플레그를 지원해야 합니다.
	* 
	* 이 둘의 지원여부를 확인한 후에 해당 인덱스의 메모리를 사용하겠다고 결정하는 것이죠.
	* 
	* 다음 과정을 통해서 메모리 타입을 습니다.
	* 
	* 쉽지 않은 알고리즘입니다. { 리소스에서 가능 & 사용자가 신청한 타입 & 메모리에서 가능 } 으로 하여
	* 리소스가 지원하는 기능과 메모리 타입에서 가능한 기능의 교집합 중 하나를 선택한다는 것이죠.
	* 
	* 예를 들어
	* 
	* 버퍼의 지원:   01110
	* 메모리의 지원: 00111
	* 사용자의 요청: 00100
	* 가능함 :      00110
	* 결과:         00100
	* 
	* 이렇게 되는겁니다.
	* 
	*/
	for (int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i += 1)
	{
		if ( (theImageMemoryRequirements.memoryTypeBits & 1 << i) &&
			 (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags )
		{
			memoryIndex = i;
			break;
		}
	}

	/* 메모리를 본격적으로 할당해줍니다. */
	VkMemoryAllocateInfo theImageMemoryAllocateInfo;
	theImageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	theImageMemoryAllocateInfo.pNext = NULL;
	theImageMemoryAllocateInfo.allocationSize = theImageMemoryRequirements.size;
	theImageMemoryAllocateInfo.memoryTypeIndex = memoryIndex;

	checkOut("Allocating image memory ", vkAllocateMemory(device, &theImageMemoryAllocateInfo, NULL, &theImageMemory));

	/* 이미지와 메모리를 바인딩 해줘야 뷰를 생성할 수 있습니다. */
	checkOut("Binding image memory ", vkBindImageMemory(device, theImage, theImageMemory, 0));

	VkImageViewCreateInfo theImageViewCreateInfo;
	theImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	theImageViewCreateInfo.pNext = NULL;
	theImageViewCreateInfo.flags = 0;
	theImageViewCreateInfo.image = theImage;
	theImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	theImageViewCreateInfo.format = theImageFormat;
	theImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	theImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	theImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	theImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	theImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	theImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	theImageViewCreateInfo.subresourceRange.levelCount = 1;
	theImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	theImageViewCreateInfo.subresourceRange.layerCount = 1;

	checkOut("Creating image view ", vkCreateImageView(device, &theImageViewCreateInfo, NULL, &theImageView));

	/* 이제 버틱스 버퍼를 만들어줍니다. */
	VkBufferCreateInfo theBufferCreateInfo;
	theBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	theBufferCreateInfo.pNext = NULL;
	theBufferCreateInfo.flags = 0;
	theBufferCreateInfo.size = sizeof(vertexData);
	theBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	theBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	theBufferCreateInfo.queueFamilyIndexCount = 0;
	theBufferCreateInfo.pQueueFamilyIndices = NULL;

	checkOut("Buffer creation ", vkCreateBuffer(device, &theBufferCreateInfo, NULL, &theBuffer));

	VkMemoryRequirements theBufferMemoryRequirements;
	vkGetBufferMemoryRequirements(device, theBuffer, &theBufferMemoryRequirements);

	uint32_t memoryFlags2 = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	memoryIndex = 0;
	for (int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i += 1)
	{
		if ( (theBufferMemoryRequirements.memoryTypeBits & (1 << i)) &&
			  (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryFlags2) == memoryFlags2 )
		{
			memoryIndex = i;
			break;
		}
	}

	VkMemoryAllocateInfo theBufferMemoryAllocateInfo;
	theBufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	theBufferMemoryAllocateInfo.pNext = NULL;
	theBufferMemoryAllocateInfo.allocationSize = theBufferMemoryRequirements.size;
	theBufferMemoryAllocateInfo.memoryTypeIndex = memoryIndex;

	checkOut("Allocating buffer memory ", vkAllocateMemory(device, &theImageMemoryAllocateInfo, NULL, &theBufferMemory));

	/* 
	* 이제 렌더링과 관련된 것들을 설정해줍니다.
	* 
	* 
	*/

	VkRenderPass theRenderPass;
	VkAttachmentDescription colorAttachment;
	VkAttachmentReference colorAttachmentRefereance;
	VkSubpassDescription theSubpass;
	VkRenderPassCreateInfo theRenderPassCreateInfo;

	colorAttachment.flags = 0;
	colorAttachment.format = theImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	colorAttachmentRefereance.attachment = 0;
	colorAttachmentRefereance.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	theSubpass.flags = 0;
	theSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	theSubpass.inputAttachmentCount = 0;
	theSubpass.pInputAttachments = NULL;
	theSubpass.colorAttachmentCount = 1;
	theSubpass.pColorAttachments = &colorAttachmentRefereance;
	theSubpass.pResolveAttachments = NULL;
	theSubpass.pDepthStencilAttachment = NULL;
	theSubpass.preserveAttachmentCount = 0;
	theSubpass.pPreserveAttachments = NULL;

	theRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	theRenderPassCreateInfo.pNext = NULL;
	theRenderPassCreateInfo.flags = 0;
	theRenderPassCreateInfo.attachmentCount = 1;
	theRenderPassCreateInfo.pAttachments = &colorAttachment;
	theRenderPassCreateInfo.subpassCount = 1;
	theRenderPassCreateInfo.pSubpasses = &theSubpass;
	theRenderPassCreateInfo.dependencyCount = 0;
	theRenderPassCreateInfo.pDependencies = NULL;

	checkOut("Creating renderpass", vkCreateRenderPass(device, &theRenderPassCreateInfo, NULL, &theRenderPass));

	/* 셰이더 모듈을 로딩해서 메모리에 올려야 합니다. */

	/* 버틱스 셰이더를 로딩합니다. 유니폼 등 다른 데이터가 없는 단순한 셰이더입니다. */
	char vertexShaderBinary[VERTEX_SHADER_BINARY_SIZE];
	VkShaderModule vertexShader = NULL;
	VkShaderModuleCreateInfo vertexShaderCreateInfo;

	loadShader("vertex.spv", VERTEX_SHADER_BINARY_SIZE, &vertexShaderBinary);

	vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertexShaderCreateInfo.pNext = NULL;
	vertexShaderCreateInfo.flags = 0;
	vertexShaderCreateInfo.codeSize = VERTEX_SHADER_BINARY_SIZE;
	vertexShaderCreateInfo.pCode = vertexShaderBinary;

	checkOut("Creating vertex shader", vkCreateShaderModule(device, &vertexShaderCreateInfo, NULL, &vertexShader));


	/* 프레그먼트 셰이더를 만듭니다. */
	char fragmentShaderBinary[FRAGMENT_SHADER_BINAY_SIZE];
	VkShaderModule fragmentShader = NULL;
	VkShaderModuleCreateInfo fragmentShaderCreateInfo;

	loadShader("fragment.spv", FRAGMENT_SHADER_BINAY_SIZE, &fragmentShaderBinary);

	fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentShaderCreateInfo.pNext = NULL;
	fragmentShaderCreateInfo.flags = 0;
	fragmentShaderCreateInfo.codeSize = FRAGMENT_SHADER_BINAY_SIZE;
	fragmentShaderCreateInfo.pCode = fragmentShaderBinary;

	checkOut("Creating fragment shader", vkCreateShaderModule(device, &fragmentShaderCreateInfo, NULL, &fragmentShader));

	/* 이제 파이프라인 레이아웃과 파이프라인을 생성해줄 차례입니다 */

	/* 먼저 파이프라인 레이아웃을 만들어야 합니다. */
	VkPipelineLayout pipelineLayout;
	VkPipelineLayoutCreateInfo pipelineCreateInfo;

	/* 셰이더 스테이지 중 버틱스 셰이더를 정의합니다. */
	VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo;
	/* 셰이더 스테이지 중 프레그먼트 셰이더를 정의합니다. */
	VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo;
	/* 만들어둔 셰이더를 배열로 묶습니다. */
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageCreateInfo , fragmentShaderStageCreateInfo };

	/* 파이프라인 스테이트들을 하나씩 정의해줍니다. */

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo;
	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	VkPipeline pipeline;

	





















	/* 모든 작업이 끝났고, 생성된 데이터들을 파괴합니다. */

	printf("\n");

	vkDestroyShaderModule(device, fragmentShader, NULL);
	vkDestroyShaderModule(device, vertexShader, NULL);

	vkDestroyRenderPass(device, theRenderPass, NULL);

	vkDestroyBufferView(device, theBufferView, NULL);
	vkFreeMemory(device, theBufferMemory, NULL);
	vkDestroyBuffer(device, theBuffer, NULL);

	vkDestroyImageView(device, theImageView, NULL);
	vkFreeMemory(device, theImageMemory, NULL);
	vkDestroyImage(device, theImage, NULL);

	vkDestroyDevice(device, NULL);
	vkDestroyInstance(theVulkan, NULL);

	return 0;
}
