
/*
* Copyright. 2021 네프티 WTFPL
*
* 불칸을 소개하고 설명하는 프로젝트입니다.
*/

/* 프린트 할 게 많아요! */
#include <stdio.h>

/* 불칸을 갖고옵니다 */
#include <vulkan/vulkan.h>

#include <vector>
#include <fstream>

#include <GLFW/glfw3.h>

/* 최대 16개의 장치를 지원합니다! */
#define MAX_DEVICE_SUPPORT_NUMBER 16

/* 쉐이더 바이너리 코드들의 사이즈. 쉐이더마다 파일 크기를 정확히 할것! */
#define VERTEX_SHADER_BINARY_SIZE 1428 * 4
//#define VERTEX_SHADER_BINARY_SIZE 1468 * 4
#define FRAGMENT_SHADER_BINAY_SIZE 536 * 4

const char* actived_layers[1] = { "VK_LAYER_KHRONOS_validation" };
const char* actived_extensions[16];

const char* outputFileName = "out.ppm";


int main()
{
	/*
	* GLFW를 초기화하고 필요한 익스텐션을 갖고옵니다.
	*/
	uint32_t windowWidth = 512;
	uint32_t windowHeight = 512;

	glfwInit();

	GLFWwindow* theWindow;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	theWindow = glfwCreateWindow(windowWidth, windowHeight, "Hello, vulkan!", NULL, NULL);

	uint32_t extensionCount = 0;
	const char** glfwNeededExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	printf("GLFW Need extension: ");
	for (int i = 0; i < extensionCount; i += 1)
	{
		printf("%s", glfwNeededExtensions[i]);
		printf(" ");

		actived_extensions[i] = glfwNeededExtensions[i];
	}
	printf("\n");
	actived_extensions[extensionCount] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

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
		instanceCreateInfo.ppEnabledLayerNames = actived_layers;
		instanceCreateInfo.enabledExtensionCount = 1;
		instanceCreateInfo.ppEnabledExtensionNames = actived_extensions;

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

	/*
	* 물리 장치를 열거하고 갖고옵니다.
	* 이 때 물리장치를 고를 변수도 만들어줍니다. 물리 장치는 여러개일 수 있기에 배열로 가져옵니다.
	*/
	uint32_t physicalDeviceSelection = 0;
	VkPhysicalDevice physicalDevices[MAX_DEVICE_SUPPORT_NUMBER] = { 0, }; /* 장치 16개를 응용하기로 함 */
	uint32_t physicalDeviceCount = 0;
	{
		vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, NULL);
		vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, physicalDevices);
	}

	if (physicalDeviceCount == 0)
	{
		printf("No device can support Vulkan...\n");
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
		deviceCreateInfo.enabledLayerCount = 1;
		deviceCreateInfo.ppEnabledLayerNames = actived_layers;



		VkResult deviceCreateResult = vkCreateDevice(physicalDevices[physicalDeviceSelection], &deviceCreateInfo, NULL, &device);
		if (deviceCreateResult == VK_SUCCESS)
		{
			printf("Logical device created with Device '%s' and queue family index '%i'\n", physicalDeviceProperties.deviceName, queueFamilySelection);
		}
	}

	VkQueue queue;
	{
		vkGetDeviceQueue(device, queueFamilySelection, 0, &queue);
	}

	printf("\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* 디바이스의 사용 가능한 익스텐션을 갖고옵니다. */
	uint32_t physicalDeviceUsableExtensionCount = 0;

	vkEnumerateDeviceExtensionProperties(physicalDevices[physicalDeviceSelection], NULL, &physicalDeviceUsableExtensionCount, NULL);
	VkExtensionProperties usableExtensions[512];
	vkEnumerateDeviceExtensionProperties(physicalDevices[physicalDeviceSelection], NULL, &physicalDeviceUsableExtensionCount, usableExtensions);

	/* 서피스를 만듭니다. */
	VkSurfaceKHR theSurface;
	{
		VkResult surfaceCreation = glfwCreateWindowSurface(theVulkan, theWindow, NULL, &theSurface);

		if (surfaceCreation != VK_SUCCESS)
		{
			printf("failed to create surface with error code : % i\n", surfaceCreation);
		}
		else
		{
			printf("Hello, window surface!\n");
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<float> vertexCoordinates = {
		 0.0, -0.5,
		 0.5,  0.5,
		-0.5,  0.5
	};

	VkBuffer theBuffer;
	{
		VkBufferCreateInfo bufferCreationInfo;
		{
			bufferCreationInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreationInfo.pNext = NULL;
			bufferCreationInfo.flags = 0;
			bufferCreationInfo.size = sizeof(float) * vertexCoordinates.size();//원하는 모든 사이즈 전부!
			bufferCreationInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			bufferCreationInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			bufferCreationInfo.queueFamilyIndexCount = 0;
			bufferCreationInfo.pQueueFamilyIndices = NULL;
		}

		VkResult bufferCreation = vkCreateBuffer(device, &bufferCreationInfo, NULL, &theBuffer);
		if (bufferCreation != VK_SUCCESS) {
			printf("failed to create buffer with error code: %i\n", bufferCreation);
		}
		else {
			printf("Hello, buffer!\n");
		}
	}

	VkDeviceMemory theBufferDeviceMemory;
	{
		VkMemoryRequirements theBufferMemoryRequirements;
		vkGetBufferMemoryRequirements(device, theBuffer, &theBufferMemoryRequirements);

		printf("The buffer need %llu size, and bits of %i\n", theBufferMemoryRequirements.size, theBufferMemoryRequirements.memoryTypeBits);

		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevices[physicalDeviceSelection], &physicalDeviceMemoryProperties);
		printf("And the physical memory can handle %i types with bits of ", physicalDeviceMemoryProperties.memoryTypeCount);

		for (int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i += 1)
		{
			printf("%d ", physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags);
		}

		printf("\n");



		VkMemoryAllocateInfo theBufferMemoryAllocateInfo;
		{
			theBufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			theBufferMemoryAllocateInfo.pNext = NULL;
			theBufferMemoryAllocateInfo.allocationSize = theBufferMemoryRequirements.size;
			theBufferMemoryAllocateInfo.memoryTypeIndex = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; //예시일 뿐 절대 이렇게 하지 마세요!
		}

		VkResult theBufferMemoryAllocation = vkAllocateMemory(device, &theBufferMemoryAllocateInfo, NULL, &theBufferDeviceMemory);

		if (theBufferMemoryAllocation != VK_SUCCESS)
		{
			printf("failed to allocate the buffers memory with error code: %i\n", theBufferMemoryAllocation);
		}
		else
		{
			printf("Hello, buffer memory!\n");
		}

		vkBindBufferMemory(device, theBuffer, theBufferDeviceMemory, 0);
	}

	printf("Uploading data to Buffer.");

	void* varData;
	vkMapMemory(device, theBufferDeviceMemory, 0, VK_WHOLE_SIZE, 0, &varData);

	::memcpy(varData, vertexCoordinates.data(), vertexCoordinates.size()); //데이터 전송!

	VkMappedMemoryRange memoryRange;
	{
		memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryRange.pNext = NULL;
		memoryRange.memory = theBufferDeviceMemory;
		memoryRange.offset = 0;
		memoryRange.size = VK_WHOLE_SIZE;
	}
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);

	vkUnmapMemory(device, theBufferDeviceMemory);

	printf("\n");

	uint32_t renderingImageWidth = 512;
	uint32_t renderingImageHeight = 512;
	VkFormat renderingFormat = VK_FORMAT_R8G8B8A8_UNORM;
	VkImageCreateInfo imageCreateInfo;
	VkImage theImage;
	{
		VkExtent3D extent3D;

		extent3D.width = renderingImageWidth;
		extent3D.height = renderingImageHeight;
		extent3D.depth = 1;

		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = NULL;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = renderingFormat;
		imageCreateInfo.extent = extent3D;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = NULL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkResult imageCreation = vkCreateImage(device, &imageCreateInfo, NULL, &theImage);

		if (imageCreation != VK_SUCCESS)
		{
			printf("Some things wrong with creating image... with code: %i\n", imageCreation);
		}
		else
		{
			printf("Hello, Image!\n");
		}
	}



	VkDeviceMemory theImageMemory;
	{
		VkMemoryRequirements theImageMemoryRequirements;
		vkGetImageMemoryRequirements(device, theImage, &theImageMemoryRequirements);

		VkMemoryAllocateInfo theImageAllocateInfo;
		{
			theImageAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			theImageAllocateInfo.pNext = NULL;
			theImageAllocateInfo.allocationSize = theImageMemoryRequirements.size;
			theImageAllocateInfo.memoryTypeIndex = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		}

		VkResult theImageMemoryAllocation = vkAllocateMemory(device, &theImageAllocateInfo, NULL, &theImageMemory);

		if (theImageMemoryAllocation != VK_SUCCESS)
		{
			printf("Some things wrong with allocating image... with code: %i\n", theImageMemoryAllocation);
		}
		else
		{
			printf("Hello, Image memory!\n");
		}

		vkBindImageMemory(device, theImage, theImageMemory, 0);
	}



	printf("\n");

	VkImageView theImageView;
	VkImageViewCreateInfo imageViewCreateInfo;
	{
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = NULL;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = theImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = renderingFormat;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
	}

	VkResult imageViewCreation = vkCreateImageView(device, &imageViewCreateInfo, NULL, &theImageView);

	if (imageViewCreation != VK_SUCCESS)
	{
		printf("Some things wrong with creating image view... with code: %i\n", imageViewCreation);
	}
	else
	{
		printf("Hello, Image view!\n");
	}

	VkFormat readableImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	VkImage readableImage;
	{
		VkImageCreateInfo imageInfo;
		{
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.pNext = NULL;
			imageInfo.flags = 0;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.format = readableImageFormat;
			imageInfo.extent = { (uint32_t)renderingImageHeight, (uint32_t)renderingImageWidth, 1 };
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
			imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.queueFamilyIndexCount = 0;
			imageInfo.pQueueFamilyIndices = NULL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		}

		VkResult readableImageCreation = vkCreateImage(device, &imageInfo, NULL, &readableImage);

		if (readableImageCreation != VK_SUCCESS)
		{
			printf("failed to create 2D image!\n");
		}
		else
		{
			printf("Hello, Readable Image!");
		}
	}

	VkDeviceMemory readableImageMemory;
	{
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, readableImage, &memRequirements);

		VkMemoryAllocateInfo allocInfo;
		{
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.pNext = NULL;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		}

		if (vkAllocateMemory(device, &allocInfo, NULL, &readableImageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, readableImage, readableImageMemory, 0);
	}

	printf("\n");



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkRenderPass renderPass;
	{
		VkAttachmentDescription colorAttatchment;
		{
			colorAttatchment.flags = 0;
			colorAttatchment.format = renderingFormat;
			colorAttatchment.samples = VK_SAMPLE_COUNT_1_BIT; //샘플이 하나라는 의미
			colorAttatchment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //시작시 상수값으로 지우기 (예를 들어 검정색)
			colorAttatchment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //랜더링 작업 후 내용을 저장하기 
			colorAttatchment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttatchment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttatchment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttatchment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		}

		VkAttachmentReference colorAttachmentRef;
		{
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
		}

		VkSubpassDescription subpassDescription;
		{
			subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescription.flags = 0;
			subpassDescription.inputAttachmentCount = 0;
			subpassDescription.pInputAttachments = NULL;
			subpassDescription.colorAttachmentCount = 1;
			subpassDescription.pColorAttachments = &colorAttachmentRef;
			subpassDescription.pResolveAttachments = NULL;
			subpassDescription.pDepthStencilAttachment = NULL;
			subpassDescription.preserveAttachmentCount = 0;
			subpassDescription.pPreserveAttachments = NULL;
		}

		VkRenderPassCreateInfo renderPassCreateInfo;
		{
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassCreateInfo.pNext = NULL;
			renderPassCreateInfo.flags = 0;
			renderPassCreateInfo.attachmentCount = 1;
			renderPassCreateInfo.pAttachments = &colorAttatchment;
			renderPassCreateInfo.subpassCount = 1;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
			renderPassCreateInfo.dependencyCount = 0;
			renderPassCreateInfo.pDependencies = NULL;
		}

		VkResult renderPassCreation = vkCreateRenderPass(device, &renderPassCreateInfo, NULL, &renderPass);

		if (imageViewCreation != VK_SUCCESS)
		{
			printf("Some things wrong with creating RenderPass... with code: %i\n", imageViewCreation);
		}
		else
		{
			printf("Hello, RenderPass!\n");
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkShaderModule vertexShaderModule;
	{
		char vertexShader[VERTEX_SHADER_BINARY_SIZE];

		FILE* filePointerVertex = fopen("vertex.spv", "rb");

		fread(vertexShader, sizeof(vertexShader), 1, filePointerVertex);


		if (filePointerVertex != NULL) {
			fclose(filePointerVertex);
		}


		VkShaderModuleCreateInfo vertexShaderCreateInfo;
		{
			vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vertexShaderCreateInfo.pNext = NULL;
			vertexShaderCreateInfo.flags = 0;
			vertexShaderCreateInfo.codeSize = VERTEX_SHADER_BINARY_SIZE / 4;
			vertexShaderCreateInfo.pCode = reinterpret_cast<uint32_t*>(vertexShader);
		}

		VkResult vertexShaderCreation = vkCreateShaderModule(device, &vertexShaderCreateInfo, NULL, &vertexShaderModule);

		if (vertexShaderCreation != VK_SUCCESS)
		{
			printf("Some things wrong with vertex shader... with code: %i\n", vertexShaderCreation);
		}
		else
		{
			printf("Hello, vertex shader!\n");
		}
	}

	VkShaderModule fragmentShaderModule;
	{
		char fragmentShader[FRAGMENT_SHADER_BINAY_SIZE];

		FILE* filePointerfragment = fopen("fragment.spv", "rb");

		fread(fragmentShader, sizeof(fragmentShader), 1, filePointerfragment);


		if (filePointerfragment != NULL) {
			fclose(filePointerfragment);
		}


		VkShaderModuleCreateInfo fragmenShaderCreateInfo;
		{
			fragmenShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			fragmenShaderCreateInfo.pNext = NULL;
			fragmenShaderCreateInfo.flags = 0;
			fragmenShaderCreateInfo.codeSize = FRAGMENT_SHADER_BINAY_SIZE / 4;
			fragmenShaderCreateInfo.pCode = reinterpret_cast<uint32_t*>(fragmentShader);
		}

		VkResult fragmentShaderCreation = vkCreateShaderModule(device, &fragmenShaderCreateInfo, NULL, &fragmentShaderModule);

		if (fragmentShaderCreation != VK_SUCCESS)
		{
			printf("Some things wrong with fragment shader... with code: %i\n", fragmentShaderCreation);
		}
		else
		{
			printf("Hello, fragment shader!\n");
		}
	}

	VkPipelineLayout piplineLayout;
	{
		VkPipelineLayoutCreateInfo piplineCreateInfo;
		{
			piplineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			piplineCreateInfo.pNext = NULL;
			piplineCreateInfo.flags = 0;
			piplineCreateInfo.setLayoutCount = 0;
			piplineCreateInfo.pSetLayouts = NULL;
			piplineCreateInfo.pushConstantRangeCount = 0;
		}

		VkResult piplineLayoutCreation = vkCreatePipelineLayout(device, &piplineCreateInfo, NULL, &piplineLayout);

		if (piplineLayoutCreation != VK_SUCCESS)
		{
			printf("Some things wrong with pipline layout... with code: %i\n", piplineLayoutCreation);
		}
		else
		{
			printf("Hello, pipline layout!\n");
		}
	}

	//파이프라인 만들기! 굉장히 길다.
	VkPipeline thePipline;
	{
		VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo;
		{
			vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertexShaderStageCreateInfo.pNext = NULL;
			vertexShaderStageCreateInfo.flags = 0;
			vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertexShaderStageCreateInfo.module = vertexShaderModule;
			vertexShaderStageCreateInfo.pName = "main";
			vertexShaderStageCreateInfo.pSpecializationInfo = NULL;
		}

		VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo;
		{
			fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragmentShaderStageCreateInfo.pNext = NULL;
			fragmentShaderStageCreateInfo.flags = 0;
			fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragmentShaderStageCreateInfo.module = fragmentShaderModule;
			fragmentShaderStageCreateInfo.pName = "main";
			fragmentShaderStageCreateInfo.pSpecializationInfo = NULL;
		}

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

		//업로드된 데이터를 셰이더에게 전달한다!
		VkVertexInputBindingDescription vex2VertexBinding;
		{
			vex2VertexBinding.binding = 0;
			vex2VertexBinding.stride = sizeof(float) * 2;
			vex2VertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		}

		VkVertexInputAttributeDescription positionVertexAttribute;
		{
			positionVertexAttribute.binding = 0;
			positionVertexAttribute.location = 0;
			positionVertexAttribute.format = VK_FORMAT_R32G32_SFLOAT;
			positionVertexAttribute.offset = 0;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
		{
			vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputStateCreateInfo.pNext = NULL;
			vertexInputStateCreateInfo.flags = 0;
			vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
			vertexInputStateCreateInfo.pVertexBindingDescriptions = &vex2VertexBinding;
			vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
			vertexInputStateCreateInfo.pVertexAttributeDescriptions = &positionVertexAttribute;
		}

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
		{
			inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyStateCreateInfo.pNext = NULL;
			inputAssemblyStateCreateInfo.flags = 0;
			inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
		}

		VkViewport viewport;
		{
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)renderingImageWidth;
			viewport.height = (float)renderingImageHeight;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
		}

		VkRect2D scissor;
		{
			scissor.offset = { 0, 0 };
			scissor.extent = { (uint32_t)viewport.width, (uint32_t)viewport.height };
		}

		VkPipelineViewportStateCreateInfo viewportState{};
		{
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;
		}

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
		{
			rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationStateCreateInfo.pNext = NULL;
			rasterizationStateCreateInfo.flags = 0;
			rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
			rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
			rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0;
			rasterizationStateCreateInfo.depthBiasClamp = 0.0;
			rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0;
			rasterizationStateCreateInfo.lineWidth = 1.0f;
		}

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
		{
			multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleStateCreateInfo.pNext = NULL;
			multisampleStateCreateInfo.flags = 0;
			multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			multisampleStateCreateInfo.minSampleShading = 0.0;
			multisampleStateCreateInfo.pSampleMask = NULL;
			multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
			multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
		}


		VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
		{
			colorBlendAttachmentState.blendEnable = VK_FALSE;
			colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		}

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
		{
			colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendStateCreateInfo.pNext = NULL;
			colorBlendStateCreateInfo.flags = 0;
			colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
			colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
			colorBlendStateCreateInfo.attachmentCount = 1;
			colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
			colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[3] = 0.0f;
		}

		VkGraphicsPipelineCreateInfo piplineCreatInfo;
		{
			piplineCreatInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			piplineCreatInfo.pNext = NULL;
			piplineCreatInfo.flags = 0;
			piplineCreatInfo.stageCount = 2;
			piplineCreatInfo.pStages = shaderStages;
			piplineCreatInfo.pVertexInputState = &vertexInputStateCreateInfo;
			piplineCreatInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
			piplineCreatInfo.pTessellationState = NULL;
			piplineCreatInfo.pViewportState = &viewportState;
			piplineCreatInfo.pRasterizationState = &rasterizationStateCreateInfo;
			piplineCreatInfo.pMultisampleState = &multisampleStateCreateInfo;
			piplineCreatInfo.pDepthStencilState = NULL;
			piplineCreatInfo.pColorBlendState = &colorBlendStateCreateInfo;
			piplineCreatInfo.layout = piplineLayout;
			piplineCreatInfo.renderPass = renderPass;
			piplineCreatInfo.subpass = 0;
			piplineCreatInfo.basePipelineHandle = VK_NULL_HANDLE;
			piplineCreatInfo.basePipelineIndex = 0;
			piplineCreatInfo.pDynamicState = NULL;
		}
		VkResult piplineCreation = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &piplineCreatInfo, NULL, &thePipline);

		if (piplineCreation != VK_SUCCESS)
		{
			printf("Some things wrong with pipline creation... with code: %i\n", piplineCreation);
		}
		else
		{
			printf("Hello, graphics pipline!\n");
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkFramebuffer theFrameBuffer;
	{
		VkFramebufferCreateInfo frameBufferCreateInfo;
		{
			frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			frameBufferCreateInfo.pNext = NULL;
			frameBufferCreateInfo.flags = 0;
			frameBufferCreateInfo.renderPass = renderPass;
			frameBufferCreateInfo.attachmentCount = 1;
			frameBufferCreateInfo.pAttachments = &theImageView;
			frameBufferCreateInfo.width = renderingImageWidth;
			frameBufferCreateInfo.height = renderingImageHeight;
			frameBufferCreateInfo.layers = 1;
		}

		VkResult frameBufferCreation = vkCreateFramebuffer(device, &frameBufferCreateInfo, NULL, &theFrameBuffer);

		if (frameBufferCreation != VK_SUCCESS)
		{
			printf("Some things wrong with Frame buffer... with code: %i\n", frameBufferCreation);
		}
		else
		{
			printf("Hello, graphics Frame buffer!\n");
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkCommandPool commandPool;
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo;
		{
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = NULL;
			commandPoolCreateInfo.flags = 0;
			commandPoolCreateInfo.queueFamilyIndex = queueFamilySelection;
		}

		VkResult commandPoolCreation = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);

		if (commandPoolCreation != VK_SUCCESS)
		{
			printf("Some things wrong with Command pool... with code: %i\n", commandPoolCreation);
		}
		else
		{
			printf("Hello, graphics Command pool!\n");
		}
	}

	VkCommandBuffer commandBuffer;
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		{
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = NULL;
			commandBufferAllocateInfo.commandPool = commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
		}

		VkResult commandBufferAllocation = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

		if (commandBufferAllocation != VK_SUCCESS)
		{
			printf("Some things wrong with Command buffer... with code: %i\n", commandBufferAllocation);
		}
		else
		{
			printf("Hello, graphics Command buffer!\n");
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkCommandBufferBeginInfo beginCommandBuffer;
	{
		beginCommandBuffer.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginCommandBuffer.pNext = NULL;
		beginCommandBuffer.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginCommandBuffer.pInheritanceInfo = NULL;
	}

	VkResult commandBufferBegin = vkBeginCommandBuffer(commandBuffer, &beginCommandBuffer);
	if (commandBufferBegin != VK_SUCCESS)
	{
		printf("Cannot start command buffer with code: %i\n", commandBufferBegin);
	}
	else
	{
		printf("Command buffer started!\n");
	}

	//Commands!
	{
		VkClearValue clearColor = { { { 0.3f, 0.3f, 0.3f, 0.3f } } };
		VkRenderPassBeginInfo beginRenderPass;
		{
			beginRenderPass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginRenderPass.pNext = NULL;
			beginRenderPass.renderPass = renderPass;
			beginRenderPass.framebuffer = theFrameBuffer;
			beginRenderPass.renderArea.offset = { 0, 0 };
			beginRenderPass.renderArea.extent = { (uint32_t)renderingImageWidth, (uint32_t)renderingImageHeight };
			beginRenderPass.clearValueCount = 1;
			beginRenderPass.pClearValues = &clearColor;
		}

		vkCmdBeginRenderPass(commandBuffer, &beginRenderPass, VK_SUBPASS_CONTENTS_INLINE);
		{

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, thePipline);

			VkDeviceSize bufferOffsets[] = { 0 };
			uint32_t vertexCount = 3;
			uint32_t instanceCount = 1;

			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &theBuffer, bufferOffsets);

			vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);

		}
		vkCmdEndRenderPass(commandBuffer);

		/*
		VkImageMemoryBarrier imageMemoryBarrierDst;
		{
			imageMemoryBarrierDst.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierDst.pNext = NULL;
			imageMemoryBarrierDst.srcAccessMask = 0;
			imageMemoryBarrierDst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrierDst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrierDst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrierDst.srcQueueFamilyIndex = 0;
			imageMemoryBarrierDst.dstQueueFamilyIndex = 0;
			imageMemoryBarrierDst.image = readableImage;
			imageMemoryBarrierDst.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		}
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrierDst);

		VkImageMemoryBarrier imageMemoryBarrierSrc;
		{
			imageMemoryBarrierSrc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierSrc.pNext = NULL;
			imageMemoryBarrierSrc.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			imageMemoryBarrierSrc.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			imageMemoryBarrierSrc.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			imageMemoryBarrierSrc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			imageMemoryBarrierSrc.srcQueueFamilyIndex = 0;
			imageMemoryBarrierSrc.dstQueueFamilyIndex = 0;
			imageMemoryBarrierSrc.image = theImage;
			imageMemoryBarrierSrc.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		}
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrierSrc);

		VkImageCopy imageCopyRegion;
		{
			imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.srcSubresource.mipLevel = 0;
			imageCopyRegion.srcSubresource.baseArrayLayer = 0;
			imageCopyRegion.srcSubresource.layerCount = 1;
			imageCopyRegion.srcOffset = { 0, 0, 0 };
			imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopyRegion.dstSubresource.mipLevel = 0;
			imageCopyRegion.dstSubresource.baseArrayLayer = 0;
			imageCopyRegion.dstSubresource.layerCount = 1;
			imageCopyRegion.dstOffset = { 0, 0, 0 };
			imageCopyRegion.extent.width = renderingImageWidth;
			imageCopyRegion.extent.height = renderingImageHeight;
			imageCopyRegion.extent.depth = 1;
		}
		vkCmdCopyImage(commandBuffer, theImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, readableImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);
		*/
	}

	VkResult commandBufferEnding = vkEndCommandBuffer(commandBuffer);
	if (commandBufferEnding != VK_SUCCESS)
	{
		printf("Some things wrong while ending command buffer with code: %i\n", commandBufferEnding);
	}
	else
	{
		printf("Command buffer ended!\n");
	}

	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	VkFence theFence;
	{
		VkFenceCreateInfo fenceCreateInfo;
		{
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = NULL;
			fenceCreateInfo.flags = 0;
		}

		VkResult fenceCreation = vkCreateFence(device, &fenceCreateInfo, NULL, &theFence);

		if (fenceCreation != VK_SUCCESS)
		{
			printf("Some things wrong while creating fence with code: %i\n", fenceCreation);
		}
		else
		{
			printf("Hello, fence!\n");
		}
	}

	VkSubmitInfo submission;
	{
		submission.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submission.pNext = NULL;
		submission.waitSemaphoreCount = 0;
		submission.pWaitSemaphores = NULL;
		submission.pWaitDstStageMask = NULL;
		submission.commandBufferCount = 1;
		submission.pCommandBuffers = &commandBuffer;
		submission.signalSemaphoreCount = 0;
		submission.pSignalSemaphores = NULL;
	}

	VkResult submiting = vkQueueSubmit(queue, 1, &submission, theFence);

	if (submiting != VK_SUCCESS)
	{
		printf("Some things wrong while submiting with code: %i\n", submiting);
	}
	else
	{
		printf("submited!!\n");
	}

	vkWaitForFences(device, 1, &theFence, VK_TRUE, -1);

	printf("\n");
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	while (!glfwWindowShouldClose(theWindow))
	{
		glfwPollEvents();
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("Copy image...\n");

	VkImageSubresource imageSubsource;
	{
		imageSubsource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubsource.mipLevel = 0;
		imageSubsource.arrayLayer = 0;
	}
	VkSubresourceLayout imageSubsourceLayout;
	vkGetImageSubresourceLayout(device, readableImage, &imageSubsource, &imageSubsourceLayout);

	const uint8_t* data;
	{
		vkMapMemory(device, readableImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&data);
		data += imageSubsourceLayout.offset;
	}

	printf("Writing image...\n");
	{
		std::ofstream file(outputFileName, std::ios::out | std::ios::binary);
		// ppm header
		file << "P6\n" << renderingImageWidth << "\n" << renderingImageHeight << "\n" << 255 << "\n";

		// ppm binary pixel data
		// As the image format is R8G8B8A8 one "pixel" size is 4 bytes (uint32_t)
		for (uint32_t y = 0; y < renderingImageHeight; y++) {
			uint32_t* row = (uint32_t*)data;
			for (uint32_t x = 0; x < renderingImageWidth; x++) {
				// Only copy the RGB values (3)
				file.write((const char*)row, 3);
				row++;
			}

			data += imageSubsourceLayout.rowPitch;
		}
		file.close();
	}
	printf("Done!\n");

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n");

	printf("Good bye!\n");

	vkDestroyFence(device, theFence, NULL);

	vkDestroyCommandPool(device, commandPool, NULL);

	vkDestroyFramebuffer(device, theFrameBuffer, NULL);

	vkDestroyPipeline(device, thePipline, NULL);

	vkDestroyPipelineLayout(device, piplineLayout, NULL);

	vkDestroyShaderModule(device, vertexShaderModule, NULL);
	vkDestroyShaderModule(device, fragmentShaderModule, NULL);

	vkDestroyRenderPass(device, renderPass, NULL);

	vkFreeMemory(device, theBufferDeviceMemory, NULL);
	vkDestroyBuffer(device, theBuffer, NULL);

	vkFreeMemory(device, theImageMemory, NULL);
	vkDestroyImage(device, theImage, NULL);

	vkFreeMemory(device, readableImageMemory, NULL);
	vkDestroyImage(device, readableImage, NULL);

	vkDestroyImageView(device, theImageView, NULL);

	vkDestroyDevice(device, NULL);
	vkDestroySurfaceKHR(theVulkan, theSurface, NULL);
	vkDestroyInstance(theVulkan, NULL);
	glfwDestroyWindow(theWindow);

	return 0;
}
