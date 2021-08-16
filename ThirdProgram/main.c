
/*
* Copyright. 2021 네프티 WTFPL
*
* 불칸을 소개하고 설명하는 프로젝트입니다.
*/

/* 프린트 할 게 많아요! */
#include <stdio.h>
#include <stdlib.h>

/* 불칸을 갖고옵니다 */
#include <vulkan/vulkan.h>

/* 디버그 모드를 키고 끕니다 */
#define DEBUG

#ifdef WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>

uint32_t isClosed = 0;

LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case 0x0010: isClosed = 1; break; // WN_CLOSE
    default : return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

#endif // WIN32


/* 최대 16개의 장치를 지원합니다! */
#define MAX_DEVICE_SUPPORT_NUMBER 16

/* 쉐이더 바이너리 코드들의 사이즈. 쉐이더마다 파일 크기를 정확히 할것! */
#define VERTEX_SHADER_BINARY_SIZE 1428
#define FRAGMENT_SHADER_BINAY_SIZE 536

#define PHYSICAL_DEVICE_SUPPORT 8

#define IMAGES_IN_FLIGHT 2

/* 활성화된 레이어들의 이름 */
const char* layers[16]; 
/* 활성화된 레이어의 개수 */
uint32_t layerCount = 0;

/* 활성화된 인스턴스 확장들의 이름 */
const char* instanceExtensions[256];
/* 활성화된 인스턴스 확장의 개수 */
uint32_t instanceExtensionCount = 0;

/* 활성화된 장치 확장들의 이름 */
const char* deviceExtensions[256];
/* 활성화된 장치 확장들의 개수 */
uint32_t deviceExtensionCount = 0;

/* 프로그램의 이름입니다 */
const char* programName = "Hello, vulkan!";

/* 윈도우의 크기입니다 */
VkExtent2D windowsDim = { 512, 512 };

/* 삼각형! */
float vertexData[6] = {
     0.0, -0.5,
     0.5,  0.5,
    -0.5,  0.5
};
uint32_t vertexDataNumber = 6;

/* Vulkan의 인스턴스입니다 */
VkInstance theVulkan;
/* Vulkan의 논리적 장치입니다 */
VkDevice theDevice;
/* Vulkan의 Queue입니다 */
VkQueue theQueue;
/* 스왑체인입니다 */
VkSwapchainKHR theSwapchain;
/* 서페이스에 사용할 크기입니다 */
VkExtent2D surfaceExtent;
/* 서페이스의 이미지 포멧입니다 */
VkSurfaceFormatKHR theSurfaceFormat;
/* 삼각형을 담을 버퍼입니다 */
VkBuffer theBuffer;
/* 삼각형을 담을 버퍼의 메모리입니다 */
VkDeviceMemory theBufferMemory;
/* 삼각형을 담을 버퍼의 뷰입니다 */
VkBufferView theBufferView;
/* 주 파이프라인입니다 */
VkPipeline thePipeline;
/* 커멘드 풀입니다 */
VkCommandPool theCommandPool;
/* 커멘드 버퍼입니다. 우리는 간단한 삼각형을 그릴 뿐이기에 하나의 커멘드버퍼만 사용합니다 */
VkCommandBuffer * theCommandBuffers;

/* 레이어를 추가합니다 */
void addLayer(const char* layerName)
{
	layers[layerCount] = layerName;
	layerCount += 1;
}

/* 인스턴스 확장을 추가합니다 */
void addInstanceExtension(const char* extensionName)
{
	instanceExtensions[instanceExtensionCount] = extensionName;
	instanceExtensionCount += 1;
}

/* 장치 확장을 추가합니다 */
void addDeviceExtension(const char* extensionName)
{
    deviceExtensions[deviceExtensionCount] = extensionName;
    deviceExtensionCount += 1;
}

/* VK_RESULT를 로깅합니다 */
void logResult(VkResult result)
{
#ifdef DEBUG
	switch (result)
	{
    case 0: printf("VK_SUCCESS!"); break;
    case 1: printf("VK_NOT_READY"); break;
    case 2: printf("VK_TIMEOUT"); break;
    case 3: printf("VK_EVENT_SET"); break;
    case 4: printf("VK_EVENT_RESET"); break;
    case 5: printf("VK_INCOMPLETE"); break;
    case -1: printf("VK_ERROR_OUT_OF_HOST_MEMORY"); break;
    case -2: printf("VK_ERROR_OUT_OF_DEVICE_MEMORY"); break;
    case -3: printf("VK_ERROR_INITIALIZATION_FAILED"); break;
    case -4: printf("VK_ERROR_DEVICE_LOST"); break;
    case -5: printf("VK_ERROR_MEMORY_MAP_FAILED"); break;
    case -6: printf("VK_ERROR_LAYER_NOT_PRESENT"); break;
    case -7: printf("VK_ERROR_EXTENSION_NOT_PRESENT"); break;
    case -8: printf("VK_ERROR_FEATURE_NOT_PRESENT"); break;
    case -9: printf("VK_ERROR_INCOMPATIBLE_DRIVER"); break;
    case -10: printf("VK_ERROR_TOO_MANY_OBJECTS"); break;
    case -11: printf("VK_ERROR_FORMAT_NOT_SUPPORTED"); break;
    case -12: printf("VK_ERROR_FRAGMENTED_POOL"); break;
    case -13: printf("VK_ERROR_UNKNOWN"); break;
    case -1000069000: printf("VK_ERROR_OUT_OF_POOL_MEMORY"); break;
    case -1000072003: printf("VK_ERROR_INVALID_EXTERNAL_HANDLE"); break;
    case -1000161000: printf("VK_ERROR_FRAGMENTATION");
    case -1000257000: printf("VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"); break;
    case -1000000000: printf("VK_ERROR_SURFACE_LOST_KHR"); break;
    case -1000000001: printf("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"); break;
    case 1000001003: printf("VK_SUBOPTIMAL_KHR");
    case -1000001004: printf("VK_ERROR_OUT_OF_DATE_KHR"); break;
    case -1000003001: printf("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"); break;
    case -1000011001: printf("VK_ERROR_VALIDATION_FAILED_EXT"); break;
    case -1000012000: printf("VK_ERROR_INVALID_SHADER_NV"); break;
    case -1000158000: printf("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"); break;
    case -1000174001: printf("VK_ERROR_NOT_PERMITTED_EXT"); break;
    case -1000255000: printf("VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"); break;
    case 1000268000: printf("VK_THREAD_IDLE_KHR"); break;
    case 1000268001: printf("VK_THREAD_DONE_KHR"); break;
    case 1000268002: printf("VK_OPERATION_DEFERRED_KHR"); break;
    case 1000268003: printf("VK_OPERATION_NOT_DEFERRED_KHR"); break;
    case 1000297000: printf("VK_PIPELINE_COMPILE_REQUIRED_EXT"); break;
    case 0x7FFFFFFF: printf("VK_RESULT_MAX_ENUM"); break;

	default: printf("Unknown result of: %i", result);
	}

    printf("\n");
#endif // DEBUG

}

/* 불칸을 초기화하는 과정의 함수입니다 */
void initVulkan(const char * appName, uint32_t apiVersion)
{

	VkApplicationInfo appInfo = {
		VK_STRUCTURE_TYPE_APPLICATION_INFO, //sType
		NULL,                               //pNext
		appName,                            //pApplicationName
		VK_MAKE_API_VERSION(0, 0, 0, 0),    //applicationVersion
		"Hello, vulkan!",                   //pEngineName
		VK_MAKE_API_VERSION(0, 0, 0, 0),    //engineVersion
		VK_MAKE_API_VERSION(1, 2, 0, 0)     //apiVersion
	};

	VkInstanceCreateInfo createInfo = {
		VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, //sType
		NULL,                                   //pNext
		0,                                      //flags
		&appInfo,                               //pApplicationInfo
		layerCount,                             //enabledLayerCount;
		layers,                                 //ppEnabledLayerNames;
		instanceExtensionCount,                         //enabledExtensionCount;
		instanceExtensions                              //ppEnabledExtensionNames;
	};

#ifdef DEBUG
    printf("Creating Vulkan Instance: ");
#endif // DEBUG
    logResult(vkCreateInstance(&createInfo, NULL, &theVulkan));
}

/* 물리 장치의 레이팅을 매깁니다 */
uint32_t ratePhysicalDevice(VkPhysicalDevice physicalDevice)
{
    /* 디바이스 프로퍼티를 갖고옵니다 */
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice , &physicalDeviceProperties);

    uint64_t avgDeviceProperties = 0;
    //deviceRating += physicalDeviceProperties.limits.maxImageDimension2D >> 8;
    /* 프로퍼티에서 최대치를 갖고옵니다 */
    VkPhysicalDeviceLimits limits = physicalDeviceProperties.limits;
    avgDeviceProperties +=
        (limits.maxImageDimension1D +
        limits.maxImageDimension2D +
        limits.maxImageDimension3D +
        limits.maxImageDimensionCube +
        limits.maxImageArrayLayers +
        limits.maxTexelBufferElements +
        limits.maxUniformBufferRange +
        limits.maxStorageBufferRange +
        limits.maxPushConstantsSize +
        limits.maxMemoryAllocationCount +
        limits.maxSamplerAllocationCount +
        limits.bufferImageGranularity +
        limits.sparseAddressSpaceSize +
        limits.maxBoundDescriptorSets +
        limits.maxPerStageDescriptorSamplers +
        limits.maxPerStageDescriptorUniformBuffers +
        limits.maxPerStageDescriptorStorageBuffers +
        limits.maxPerStageDescriptorSampledImages +
        limits.maxPerStageDescriptorStorageImages +
        limits.maxPerStageDescriptorInputAttachments +
        limits.maxPerStageResources +
        limits.maxDescriptorSetSamplers +
        limits.maxDescriptorSetUniformBuffers +
        limits.maxDescriptorSetUniformBuffersDynamic +
        limits.maxDescriptorSetStorageBuffers +
        limits.maxDescriptorSetStorageBuffersDynamic +
        limits.maxDescriptorSetSampledImages +
        limits.maxDescriptorSetStorageImages +
        limits.maxDescriptorSetInputAttachments +
        limits.maxVertexInputAttributes +
        limits.maxVertexInputBindings +
        limits.maxVertexInputAttributeOffset +
        limits.maxVertexInputBindingStride +
        limits.maxVertexOutputComponents +
        limits.maxTessellationGenerationLevel +
        limits.maxTessellationPatchSize +
        limits.maxTessellationControlPerVertexInputComponents +
        limits.maxTessellationControlPerVertexOutputComponents +
        limits.maxTessellationControlPerPatchOutputComponents +
        limits.maxTessellationControlTotalOutputComponents +
        limits.maxTessellationEvaluationInputComponents +
        limits.maxTessellationEvaluationOutputComponents +
        limits.maxGeometryShaderInvocations +
        limits.maxGeometryInputComponents +
        limits.maxGeometryOutputComponents +
        limits.maxGeometryOutputVertices +
        limits.maxGeometryTotalOutputComponents +
        limits.maxFragmentInputComponents +
        limits.maxFragmentOutputAttachments +
        limits.maxFragmentDualSrcAttachments +
        limits.maxFragmentCombinedOutputResources +
        limits.maxComputeSharedMemorySize +
        limits.maxComputeWorkGroupInvocations +
        limits.subPixelPrecisionBits +
        limits.subTexelPrecisionBits +
        limits.mipmapPrecisionBits +
        limits.maxDrawIndexedIndexValue +
        limits.maxDrawIndirectCount +
        limits.maxSamplerLodBias +
        limits.maxSamplerAnisotropy +
        limits.maxViewports +
        limits.viewportSubPixelBits +
        limits.minMemoryMapAlignment +
        limits.minTexelBufferOffsetAlignment +
        limits.minUniformBufferOffsetAlignment +
        limits.minStorageBufferOffsetAlignment +
        limits.minTexelOffset +
        limits.maxTexelOffset +
        limits.minTexelGatherOffset +
        limits.maxTexelGatherOffset +
        limits.minInterpolationOffset +
        limits.maxInterpolationOffset +
        limits.subPixelInterpolationOffsetBits +
        limits.maxFramebufferWidth +
        limits.maxFramebufferHeight +
        limits.maxFramebufferLayers +
        limits.framebufferColorSampleCounts +
        limits.framebufferDepthSampleCounts +
        limits.framebufferStencilSampleCounts +
        limits.framebufferNoAttachmentsSampleCounts +
        limits.maxColorAttachments +
        limits.sampledImageColorSampleCounts +
        limits.sampledImageIntegerSampleCounts +
        limits.sampledImageDepthSampleCounts +
        limits.sampledImageStencilSampleCounts +
        limits.storageImageSampleCounts +
        limits.maxSampleMaskWords +
        limits.timestampPeriod +
        limits.maxClipDistances +
        limits.maxCullDistances +
        limits.maxCombinedClipAndCullDistances +
        limits.discreteQueuePriorities +
        limits.pointSizeGranularity +
        limits.lineWidthGranularity +
        limits.optimalBufferCopyOffsetAlignment +
        limits.optimalBufferCopyRowPitchAlignment +
        limits.nonCoherentAtomSize) / 97;

    return avgDeviceProperties;
}

/* 큐 페밀리를 찾습니다. 지원되는 큐의 기능을 비트로 표현해 넣습니다. 만약 정확히 일치하는 페밀리가 없는 경우 -1을 반환합니다 */
int selectQueueFamily(VkPhysicalDevice physicalDevice, int queueFlagsToSupport)
{
    uint32_t queueFamilyCount = 0;
    VkQueueFamilyProperties properties[16];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, properties);

    for (int i = 0; i < queueFamilyCount; i += 1)
    {
        if (properties[i].queueFlags & queueFlagsToSupport)
        {
            return i;
        }
    }

    return -1;
}

void createDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo deviceQueueCreateInfo)
{
    VkDeviceCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO, // sType
        NULL,                                 // pNext
        0,                                    // flags
        1,                                    // queueCreateInfoCount
        &deviceQueueCreateInfo,               // pQueueCreateInfos;
        0,                                    // enabledLayerCount;
        NULL,                                 // ppEnabledLayerNames;
        deviceExtensionCount,                 // enabledExtensionCount;
        deviceExtensions,                     // ppEnabledExtensionNames
        NULL                                  // pEnabledFeatures;
    };
#ifdef DEBUG
    printf("Creating device: ");
#endif
    logResult(vkCreateDevice(physicalDevice, &createInfo, NULL, &theDevice));
}

void createSwapchain(VkSurfaceKHR surface, uint32_t imageCount, VkExtent2D extent, VkSurfaceTransformFlagBitsKHR transform)
{
    VkSwapchainCreateInfoKHR createInfo = {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,                           //sType
        NULL,                                                                  //pNext
        0,                                                                     //flags
        surface,                                                               //surface 
        imageCount,                                                            //minImageCount
        theSurfaceFormat.format,                                               //imageFormat
        theSurfaceFormat.colorSpace,                                           //imageColorSpace
        extent,                                                                //imageExtent
        1,                                                                     //imageArrayLayers
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, //imageUsage
        VK_SHARING_MODE_EXCLUSIVE,                                             //imageSharingMode
        0,                                                                     //queueFamilyIndexCount
        NULL,                                                                  //pQueueFamilyIndices
        transform,                                                             //preTransform
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,                                     //compositeAlpha
        VK_PRESENT_MODE_FIFO_KHR,                                              //presentMode
        VK_TRUE,                                                               //clipped
        NULL                                                                   //oldSwapchain
    };

#ifdef DEBUG
    printf("Creating swapchain: ");
#endif // DEBUG
    logResult(vkCreateSwapchainKHR(theDevice, &createInfo, NULL, &theSwapchain));

}

/* 
* 지원되는 메모리타입을 검사하고 원하는 기능을 모두 지원하는 메모리타입의 인덱스를 반환합니다.
* 
* 
*/
int findMemoryType(uint32_t physicalDeviceSelection, VkMemoryPropertyFlags requiredFlags)
{
    uint32_t physicalDeviceCount = 0;
    VkPhysicalDevice physicalDevice[PHYSICAL_DEVICE_SUPPORT];
    vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, NULL);
    vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, physicalDevice);

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice[physicalDeviceSelection], &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i += 1)
    {
        if ((memProperties.memoryTypes[i].propertyFlags & requiredFlags) == requiredFlags)
        {
            return i;
        }
    }

    return -1;
}

void createImage()
{

}

void createImageView2D(VkImage image, VkFormat imageFormat, VkComponentMapping componentMapping, VkImageSubresourceRange subsourceRange, VkImageView * imageView)
{
    VkImageViewCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // sType
        NULL,                                     // pNext
        0,                                        // flags
        image,                                    // image
        VK_IMAGE_VIEW_TYPE_2D,                    // viewType
        imageFormat,                              // format
        componentMapping,                         // components
        subsourceRange                            // subresourceRange
    };

#ifdef DEBUG
    printf("Creating Image view: ");
#endif // DEBUG
    logResult(vkCreateImageView(theDevice, &createInfo, NULL, imageView));
}

void createBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags bufferUsage, VkSharingMode bufferSharingMod, uint32_t queueFamilySelection, VkBuffer * buffer)
{
    VkBufferCreateInfo createInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, // sType
        NULL,                                 // pNext
        0,                                    // flags
        bufferSize,                           // size
        bufferUsage,                          // usage
        bufferSharingMod,                     // sharingMode
        0,                                    // queueFamilyIndexCount
        NULL                                  // pQueueFamilyIndices
    };

#ifdef DEBUG
    printf("Creating buffer: ");
#endif // DEBUG
    logResult(vkCreateBuffer(theDevice, &createInfo, NULL, buffer));
}

/* 버퍼를 가지고 메모리를 할당합니다. 할당된 메모리는 바인딩이 가능합니다 */
void allocBufferMemory(VkBuffer buffer, uint32_t physicalDeviceSelection, VkDeviceMemory * deviceMemory)
{
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(theDevice, buffer, &memRequirements);

    int32_t memoryTypeIndex = findMemoryType(physicalDeviceSelection, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (memoryTypeIndex < 0)
    {
        printf("Fatal error while allocating memory! maybe memory types are not supported.%i\n", memoryTypeIndex);

        return;
    }

    VkMemoryAllocateInfo allocInfo;
    {
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext = NULL;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;
    }

#ifdef DEBUG
    printf("Allocating buffer: ");
#endif // DEBUG
    logResult(vkAllocateMemory(theDevice, &allocInfo, NULL, deviceMemory));
}

/* 버퍼메모리를 가지고 데이터를 업로드합니다. 버퍼 메모리의 유형을 VK_MEMORY_TYPE_HOST_VISIBLE로 해야합니다 */
void uploadDataToBuffer(void * data, uint32_t dataLenth, VkDeviceMemory bufferMemory)
{
    void* dst;
#ifdef DEBUG
    printf("Mapping mamory: ");
#endif
    logResult(vkMapMemory(theDevice, bufferMemory, 0, VK_WHOLE_SIZE, 0, &dst));

    memcpy(dst, data, dataLenth);

    VkMappedMemoryRange memoryRange = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, //sType;
        NULL,                                  //pNext
        bufferMemory,                          //memory;
        0,                                     //offset;
        VK_WHOLE_SIZE                          //size;
    };

#ifdef DEBUG
    printf("Flushing mamory: ");
#endif
    logResult(vkFlushMappedMemoryRanges(theDevice, 1, &memoryRange));

#ifdef DEBUG
    printf("Un Mapping mamory. \n");
#endif
    vkUnmapMemory(theDevice, bufferMemory);
}

/* 컴파일된 셰이더 바이너리를 로드합니다. 중요합니다! */
void loadShader(const char * fileName, uint32_t size,char ** output)
{
    printf("Loading shader %s!\n", fileName);

    FILE* fp;

    fp = fopen(fileName, "rb");

    fread(output, size, 1, fp);
}

void createCommandPool(uint32_t queueFamilySelection)
{
    VkCommandPoolCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, // sType
            NULL,                                       // pNext
            0,                                          // flags
            queueFamilySelection                        // queueFamilyIndex
    };
#ifdef DEBUG
    printf("Creating Command Pool: ");
#endif
    logResult(vkCreateCommandPool(theDevice, &createInfo, NULL, &theCommandPool));
}

void allocCommandBuffer(uint32_t count)
{
    theCommandBuffers = malloc(sizeof(VkCommandBuffer) * count);

    VkCommandBufferAllocateInfo allocInfo = {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // sType
            NULL,                                           // pNext
            theCommandPool,                                 // commandPool
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,                // level
            count                                           // commandBufferCount
    };

#ifdef DEBUG
    printf("Allocating Command Buffer: ");
#endif
    logResult(vkAllocateCommandBuffers(theDevice, &allocInfo, theCommandBuffers));
}

void freeCommandBuffer(uint32_t cmdBufferCount)
{
    vkFreeCommandBuffers(theDevice, theCommandPool, cmdBufferCount, theCommandBuffers);
    free(theCommandBuffers);
}

void beginCommandBuffer(uint32_t index)
{
    VkCommandBufferBeginInfo beginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, // sType
        NULL,                                       // pNext
        0,                                          // flags
        NULL                                        // pInheritanceInfo
    };

#ifdef DEBUG
    printf("Beginning Command Buffer: ");
#endif
    logResult(vkBeginCommandBuffer(theCommandBuffers[index], &beginInfo));
}

void endCommandBuffer(uint32_t index)
{
    vkEndCommandBuffer(theCommandBuffers[index]);
#ifdef DEBUG
    printf("Ending Command Buffer.\n");
#endif
}

int main()
{
	/* 누구나 인사를 받으면 기분이 좋죠 */
	printf("Program started! thanks for runing!\n");

	/* 디버그 모드가 켜져있으면 Validation Layer을 추가합니다 */
#ifdef DEBUG
	addLayer("VK_LAYER_KHRONOS_validation");
#endif

	/* 윈도우 환경에서 필요로 하는 확장을 추가합니다 */
#ifdef WIN32
    addInstanceExtension(VK_KHR_SURFACE_EXTENSION_NAME);
    addInstanceExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    printf("\n");

	/* 인스턴스를 만들어냅니다 */
	initVulkan("Hello, vulkan!", VK_MAKE_API_VERSION(0, 0, 0, 0));

    /* 윈도우를 생성합니다! */
#ifdef WIN32
    /* hInstance는 윈도우를 만드는데 필요합니다 */
    HINSTANCE hInstance = GetModuleHandle(NULL);

    /* 불칸과 비슷한 코딩스텐다드로 구조체를 받는 방식으로 되어있습니다 */
    WNDCLASSW wndClass = {
        CS_HREDRAW | CS_VREDRAW,                    //style
        WndProc,                                    //lpfnWndProc
        0,                                          //cbClsExtra
        0,                                          //cbWndExtra
        hInstance,                                  //hInstance
        LoadIcon(hInstance, MAKEINTRESOURCE(107)),  //hIcon
        LoadCursor(NULL, IDC_ARROW),                //hCursor
        (HBRUSH)(COLOR_WINDOW + 1),                 //hbrBackground
        MAKEINTRESOURCEW(109),                      //lpszMenuName
        L"Hello, Vulkan"                            //lpszClassName
    };

    /* 위에 만든 클래스를 레지스터 해줍니다 */
    RegisterClassW(&wndClass);

    /* 윈도우를 만듭니다 */
    HWND hWnd = CreateWindowW(L"Hello, Vulkan", "Hello, Vulkan", WS_OVERLAPPEDWINDOW, 0, 0, windowsDim.width, windowsDim.height, NULL, NULL, hInstance, NULL);
    if (!hWnd)
    {
        printf("Faild to create window!\n");
        return -1;
    }

    ShowWindow(hWnd, 1);
#endif

    /* 물리 장치를 열거합니다 */
    printf("Enumrating physical devices...\n");
    /* 지원되는 물리장치의 개수 */
    
    
    uint32_t physicalDeviceCount = 0;
    VkPhysicalDevice physicalDevices[PHYSICAL_DEVICE_SUPPORT];

    vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, NULL);
    if (physicalDeviceCount == 0)
    {
        printf("Error: NO Physical device can support Vulkan!!!\n");
        return -1;
    }
    vkEnumeratePhysicalDevices(theVulkan, &physicalDeviceCount, physicalDevices);
    
    /* 장치들을 콘솔에 출력합니다 */
    printf("Devices: ");
    for (int i = 0; i < physicalDeviceCount; i += 1)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

        printf("%s", deviceProperties.deviceName);

        if (i != physicalDeviceCount - 1)
        {
            printf(", ");
        }
    }
    printf(".\n");

    /* 논리적 장치를 생성합니다. 논리적 장치를 생성하기 위해서는 queue를 만들 정보도 필요합니다 */
    uint32_t physicalDeviceRatings[PHYSICAL_DEVICE_SUPPORT];

    /* 장치를 선택합니다 */
    uint32_t physicalDeviceSelection = 0;

    for (int i = 0; i < physicalDeviceCount; i += 1)
    {
        physicalDeviceRatings[i] = ratePhysicalDevice(physicalDevices[i]);
        

        if (physicalDeviceRatings[physicalDeviceSelection] < physicalDeviceRatings[i])
        {
            physicalDeviceSelection = i;
        }
    }

    VkPhysicalDeviceProperties delectedPhysicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevices[physicalDeviceSelection], &delectedPhysicalDeviceProperties);

    printf("Selected device: %s\n", delectedPhysicalDeviceProperties.deviceName);

    int queueFamilySelection = selectQueueFamily(physicalDevices[physicalDeviceSelection], VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT);

    if (queueFamilySelection == -1)
    {
        printf("Fatal error while finding queue family!!!\n");
        return -1;
    }

    /* 장치에 들어갈 확장을 추가합니다. 인스턴스의 확장과 별도입니다! */
    addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    /* 큐의 우선순위입니다. 우선순위가 높을수록 더 높은 확률로 작업을 배정받고 하드웨어 리소스도 많이 할당받습니다 */
    const float queuePriorities = 0.0f;
    /* 큐를 생성할 정보입니다. 논리 장치를 생성할 때 함께 들어갑니다 */
    VkDeviceQueueCreateInfo queueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, //sType
        NULL,                                       //pNext
        0,                                          //flags;
        queueFamilySelection,                       //queueFamilyIndex;
        1,                                          //queueCount;
        &queuePriorities                            //pQueuePriorities;
    };
    
    createDevice(physicalDevices[physicalDeviceSelection], queueCreateInfo);
    printf("\n");

#ifdef WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        NULL,
        0,
        hInstance,
        hWnd
    };

    /* 윈도우 서페이스를 만듭니다 */
    VkSurfaceKHR surface;
#ifdef DEBUG
    printf("Creating win32 surface: ");
#endif // DEBUG
    logResult(vkCreateWin32SurfaceKHR(theVulkan, &surfaceCreateInfo, NULL, &surface));
#endif

    /* 디바이스로부터 생성된 큐를 갖고옵니다 */
    vkGetDeviceQueue(theDevice, queueFamilySelection, 0, &theQueue);

    /*
    * 
    * 이 부분부터는 스왑체인을 만들도록 합니다
    * 스왑체인인이란 화면에 보이기위해 대기하는 이미지의 배열입니다
    * 
    */

    /* 서페이스의 유형을 갖고옵니다. 디바이스마다 다를 수 있습니다 */
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[physicalDeviceSelection], surface, &surfaceCapabilities);

    /* 서페이스가 사용할 크기를 서페이스 유형에서부터 갖고옵니다 */
    surfaceExtent = surfaceCapabilities.currentExtent;

    /* 서페이스를 만들 포멧을 갖고옵니다. 디바이스마다 다를 수 있습니다 */
    uint32_t surfaceFormatCount;
    VkSurfaceFormatKHR surfaceFormats[16];
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[physicalDeviceSelection], surface, &surfaceFormatCount, NULL);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[physicalDeviceSelection], surface, &surfaceFormatCount, surfaceFormats);

    /* 가능한 서페이스 포멧들 중에서 필요한 서페이스가 존재하는지 확인하고 고릅니다 */
    for (int i = 0; i < surfaceFormatCount; i += 1)
    {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            theSurfaceFormat = surfaceFormats[i];
            break;
        }
    }

    /* 서페이스가 지원되는지 확인합니다 */
    VkBool32 surfaceSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[physicalDeviceSelection], queueFamilySelection, surface, &surfaceSupported);

    if (surfaceSupported == VK_FALSE)
    {
        printf("No device can support surface...\n");
        return -1;
    }

    /* 스왑체인을 생성합니다 */
    createSwapchain(surface, surfaceCapabilities.minImageCount, surfaceExtent, surfaceCapabilities.currentTransform);

    uint32_t swapchainImageCount = 0;
    VkImage swapcahinImages[5];
    vkGetSwapchainImagesKHR(theDevice, theSwapchain, &swapchainImageCount, NULL);
    vkGetSwapchainImagesKHR(theDevice, theSwapchain, &swapchainImageCount, swapcahinImages);

    uint32_t renderImageWidth = surfaceExtent.width;
    uint32_t renderImageHeight = surfaceExtent.height;

    VkComponentMapping swapchainImageViewComponentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    VkImageSubresourceRange swapchainImageSubsourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    VkImageView* swapchainImageViews = malloc(sizeof(VkImageView) * swapchainImageCount);

    for (int i = 0; i < swapchainImageCount; i += 1)
    {
        createImageView2D(swapcahinImages[i], theSurfaceFormat.format, swapchainImageViewComponentMapping, swapchainImageSubsourceRange, &swapchainImageViews[i]);
    }

    /* 버퍼를 생성하고 theBuffer이라 명명합니다 */
    createBuffer(sizeof(float) * vertexDataNumber, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, queueFamilySelection, &theBuffer);

    /* 버퍼가 사용할 메모리를 할당해줍니다 */
    allocBufferMemory(theBuffer, physicalDeviceSelection, &theBufferMemory);

    /* 버퍼에다 버퍼메모리를 바인딩 합니다 */
#ifdef DEBUG
    printf("Binding buffer: ");
#endif
    logResult(vkBindBufferMemory(theDevice, theBuffer, theBufferMemory, 0));

    /* 데이터를 버퍼의 메모리에 업로드 합니다. */
    uploadDataToBuffer(vertexData, vertexDataNumber * sizeof(float), theBufferMemory);

    printf("\n");

    /* 
    * vkCmdBeginRenderPass에 사용할 렌더패스를 준비합니다.
    * 렌더패스는 렌더링에 관여하는 작업을 모은 작업의 단위입니다.
    * 렌더패스는 디스패치라는 작업을 통해 렌더링에 사용될 자원을
    * 확보하고 확보한 자원으로 작업을 합니다
    * 
    * 랜더패스는 로컬하게 선언하기로 합니다
    */
    
    /* 렌더패스를 만듭니다 */
    VkRenderPass theRenderPass;
    {
        /* 렌더링에 사용할 컬러 어태치먼트를 작성합니다 */
        VkAttachmentDescription colorAttachment = {
            0,                                // flags
            theSurfaceFormat.format,          // format;
            VK_SAMPLE_COUNT_1_BIT,            // samples;
            VK_ATTACHMENT_LOAD_OP_CLEAR,      // loadOp;
            VK_ATTACHMENT_STORE_OP_STORE,     // storeOp;
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // stencilLoadOp;
            VK_ATTACHMENT_STORE_OP_DONT_CARE, // stencilStoreOp;
            VK_IMAGE_LAYOUT_UNDEFINED,        // initialLayout;
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR   // finalLayout;
        };

        /* 서브패스에 사용할 컬러 어태치먼트 레퍼런스를 정의합니다 */
        VkAttachmentReference colorAttachmentReference = {
            0,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        /* 서브패스를 정의합니다. 그래픽을 위한 서브패스를 만들기로 합니다 */
        VkSubpassDescription subpass = {
            0,                               // flags;
            VK_PIPELINE_BIND_POINT_GRAPHICS, // pipelineBindPoint;
            0,                               // inputAttachmentCount;
            NULL,                            // pInputAttachments;
            1,                               // colorAttachmentCount;
            &colorAttachmentReference,       // pColorAttachments;
            NULL,                            // pResolveAttachments;
            NULL,                            // pDepthStencilAttachment;
            0,                               // preserveAttachmentCount;
            NULL                             // pPreserveAttachments

        };

        VkRenderPassCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, // sType;
            NULL,                                      // pNext;
            0,                                         // flags;
            1,                                         // attachmentCount;
            &colorAttachment,                          // pAttachments;
            1,                                         // subpassCount;
            &subpass,                                  // pSubpasses;
            0,                                         // dependencyCount;
            NULL                                       // pDependencies;
        };

#ifdef DEBUG
        printf("Creating render pass: ");
#endif
        logResult(vkCreateRenderPass(theDevice, &createInfo, NULL, &theRenderPass));
    }

    /* 
    * 셰이더를 로딩합니다. 셰이더는 spv라는 형태의 자료로 저장되어있습니다.
    * 셰이더가 로딩되면 문자열 형태로 저장이 되며 불칸에게 이를 넘겨주도록 되어있습니다
    */

    /* 버틱스 셰이더를 로딩합니다. 유니폼 등 다른 데이터가 없는 단순한 셰이더입니다. */
    char vertexShaderBinary[VERTEX_SHADER_BINARY_SIZE];
    loadShader("vertex.spv", VERTEX_SHADER_BINARY_SIZE, &vertexShaderBinary);

    /* 버틱스 셰이더입니다 */
    VkShaderModule vertexShader;
    {
        VkShaderModuleCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // sType;
            NULL,                                        // pNext;
            0,                                           // flags;
            VERTEX_SHADER_BINARY_SIZE,                   // codeSize
            vertexShaderBinary                           //pCode;
        };
#ifdef DEBUG
        printf("Creating vertex shader: ");
#endif
        logResult(vkCreateShaderModule(theDevice, &createInfo, NULL, &vertexShader));
    }

    /* 프레그먼트 셰이더를 로딩합니다. */
    char fragmentShaderBinary[FRAGMENT_SHADER_BINAY_SIZE];
    loadShader("fragment.spv", FRAGMENT_SHADER_BINAY_SIZE, &fragmentShaderBinary);

    /* 프레그먼트 셰이더입니다 */
    VkShaderModule fragmentShader;
    {
        VkShaderModuleCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // sType;
            NULL,                                        // pNext;
            0,                                           // flags;
            FRAGMENT_SHADER_BINAY_SIZE,                   // codeSize
            fragmentShaderBinary                           //pCode;
        };
#ifdef DEBUG
        printf("Creating fragment shader: ");
#endif
        logResult(vkCreateShaderModule(theDevice, &createInfo, NULL, &fragmentShader));
    }


    /* 파이프라인 레이아웃과 파이프라인을 생성합니다 */

    /* 파이프라인 레이아웃입니다 */
    VkPipelineLayout pipelineLayout;
    {

        VkPipelineLayoutCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, // sType;
            NULL,                                          // pNext;
            0,                                             // flags;
            0,                                             // setLayoutCount;
            NULL,                                          // pSetLayouts;
            0,                                             // pushConstantRangeCount;
            NULL,                                             // pPushConstantRanges;
        };

#ifdef DEBUG
        printf("Creating Pipeline pipeline: ");
#endif
        logResult(vkCreatePipelineLayout(theDevice, &createInfo, NULL, &pipelineLayout));
    }

    /* 메인 파이프라인을 생성합니다. 아주 깁니다! */
    {
        /* 버틱스 셰이더의 파이프라인을 생성합니다 */
        VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, //sType;
            NULL,                                                // pNext;
            0,                                                   // flags;
            VK_SHADER_STAGE_VERTEX_BIT,                          // stage;
            vertexShader,                                        // module;
            "main",                                              // pName;
            NULL                                                 // pSpecializationInfo;
        };

        VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, //sType;
            NULL,                                                // pNext;
            0,                                                   // flags;
            VK_SHADER_STAGE_FRAGMENT_BIT,                        // stage;
            fragmentShader,                                      // module;
            "main",                                              // pName;
            NULL
        };

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

        VkVertexInputBindingDescription vertexBinding = {
            0,                          // binding
            sizeof(float) * 2,          // stride
            VK_VERTEX_INPUT_RATE_VERTEX //inputRate
        };

        VkVertexInputAttributeDescription positionVertexAttribute = {
            0,                       // location
            0,                       // binding
            VK_FORMAT_R32G32_SFLOAT, // format
            0                        // offset
        };

        VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, // sType
            NULL,                                                      // pNext 
            0,                                                         // flags
            1,                                                         // vertexBindingDescriptionCount
            &vertexBinding,                                            // pVertexBindingDescriptions
            1,                                                         // vertexAttributeDescriptionCount
            &positionVertexAttribute                                   // pVertexAttributeDescriptions
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, // sType
            NULL,                                                        // pNext
            0,                                                           // flags
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                         // topology
            VK_FALSE                                                     // primitiveRestartEnable
        };

        VkViewport viewPort = {
            0.0f,              // x
            0.0f,              // y
            renderImageWidth,  // width
            renderImageHeight, // height
            0.0f,              // minDepth
            1.0f               // maxDepth
        };

        VkRect2D scissor = {
            { 0, 0 },     // offset
            surfaceExtent //extent
        };

        VkPipelineViewportStateCreateInfo viewportState = {
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, // sType
            NULL,                                                  // pNext
            0,                                                     // flags
            1,                                                     // viewportCount
            &viewPort,                                             // pViewports;
            1,                                                     // scissorCount;
            &scissor                                               // pScissors;
        };

        VkPipelineRasterizationStateCreateInfo rasterizerStateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // sType
            NULL,                                                       // pNext;
            0,                                                          // flags;
            VK_FALSE,                                                   // depthClampEnable;
            VK_FALSE,                                                   // rasterizerDiscardEnable;
            VK_POLYGON_MODE_FILL,                                       // polygonMode;
            VK_CULL_MODE_BACK_BIT,                                      // cullMode;
            VK_FRONT_FACE_CLOCKWISE,                                    // frontFace;
            VK_FALSE,                                                   // depthBiasEnable;
            0.0,                                                        // depthBiasConstantFactor;
            0.0,                                                        // depthBiasClamp;
            0.0,                                                        // depthBiasSlopeFactor;
            1.0f                                                        // lineWidth;
        };

        VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, // sType;
            NULL,                                                     // pNext;
            0,                                                        // flags;
            VK_SAMPLE_COUNT_1_BIT,                                    // rasterizationSamples;
            VK_FALSE,                                                 // sampleShadingEnable;
            0.0,                                                      // minSampleShading;
            NULL,                                                     // pSampleMask;
            VK_FALSE,                                                 // alphaToCoverageEnable;
            VK_FALSE                                                  // alphaToOneEnable;
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {
            VK_FALSE,                  // blendEnable
            VK_BLEND_FACTOR_ONE,       // srcColorBlendFactor;
            VK_BLEND_FACTOR_ONE,       // dstColorBlendFactor;
            VK_BLEND_OP_ADD,           // colorBlendOp;
            VK_BLEND_FACTOR_ZERO,      // srcAlphaBlendFactor;
            VK_BLEND_FACTOR_ZERO,      // dstAlphaBlendFactor;
            VK_BLEND_OP_ADD,           // alphaBlendOp;
            VK_COLOR_COMPONENT_R_BIT 
            | VK_COLOR_COMPONENT_G_BIT 
            | VK_COLOR_COMPONENT_B_BIT
            | VK_COLOR_COMPONENT_A_BIT //colorWriteMask;
        };

        VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, // sType;
            NULL,                                                     // pNext;
            0,                                                        // flags;
            VK_FALSE,                                                 // logicOpEnable;
            VK_LOGIC_OP_COPY,                                         // logicOp;
            1,                                                        // attachmentCount;
            &colorBlendAttachment,                                    // pAttachments;
            0.0f,                                                     //blendConstants[0];
            0.0f,                                                     //blendConstants[1];
            0.0f,                                                     //blendConstants[2];
            0.0f,                                                     //blendConstants[3];
        };



        VkGraphicsPipelineCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, // sType;
            NULL,                                            // pNext;
            0,                                               // flags;
            2,                                               // stageCount;
            shaderStages,                                    // pStages;
            &vertexInputInfo,                                // pVertexInputState;
            &inputAssemblyCreateInfo,                        // pInputAssemblyState;
            NULL,                                            // pTessellationState;
            &viewportState,                                  // pViewportState;
            &rasterizerStateInfo,                            // pRasterizationState;
            &multisampleStateInfo,                           // pMultisampleState;
            NULL,                                            // pDepthStencilState;
            &colorBlendStateInfo,                            // pColorBlendState;
            NULL,                                            // pDynamicState;
            pipelineLayout,                                  // layout;
            theRenderPass,                                   // renderPass;
            0,                                               // subpass;
            VK_NULL_HANDLE,                                  // basePipelineHandle;
            0                                                // basePipelineIndex;
        };

#ifdef DEBUG
        printf("Creating Graphics Pipeline : ");
#endif
        logResult(vkCreateGraphicsPipelines(theDevice, VK_NULL_HANDLE, 1, &createInfo, NULL, &thePipeline));
    }

    VkFramebuffer* swapchainFrameBuffers = malloc(sizeof(VkFramebuffer) * swapchainImageCount);
    {
        VkFramebufferCreateInfo createInfo = {
            VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, // sType
            NULL,                                      // pNext
            0,                                         // flags
            theRenderPass,                             // renderPass;
            1,                                         // attachmentCount;
            NULL,                                      // pAttachments;
            surfaceExtent.width,                       // width;
            surfaceExtent.height,                      // height;
            1                                          // layers;
        };

#ifdef DEBUG
        printf("Creating swapchain frame buffers: \n");
#endif

        for (int i = 0; i < swapchainImageCount; i += 1)
        {
            createInfo.pAttachments = &swapchainImageViews[i];
            printf("\t");
            logResult(vkCreateFramebuffer(theDevice, &createInfo, NULL, &swapchainFrameBuffers[i]));
        }
    }

    /* 커멘드 풀을 생성하고 커멘드 버퍼를 작성하여 커멘드를 기록합니다. 엄청 중요한 파트입니다! */

    /* 커멘드 풀을 생성합니다 */
    createCommandPool(queueFamilySelection);

    /* 커멘드를 기록할 커멘드 버퍼를 만듭니다 */
    allocCommandBuffer(swapchainImageCount);

    
    for (int i = 0; i < swapchainImageCount; i += 1)
    {
        /* 커멘드 버퍼를 시작시킵니다. 이 때부터 기록이 이뤄집니다! */
        beginCommandBuffer(i);
    

        /* 커멘드 버퍼를 작성합니다. 각 프레임마다 사용할 커멘드버퍼를 따로 작성합니다 */

        VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        VkRenderPassBeginInfo renderPassBeginInfo = {
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,            // sType;
            NULL,                                                // pNext;
            theRenderPass,                                       // renderPass;
            swapchainFrameBuffers[i],                            // framebuffer;
            { {0, 0}, { renderImageWidth, renderImageHeight } }, // renderArea;
            1,                                                   // clearValueCount;
            &clearColor                                          // pClearValues;
        };

        vkCmdBeginRenderPass(theCommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(theCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, thePipeline);

        VkDeviceSize bufferOffsets[] = { 0 };
        vkCmdBindVertexBuffers(theCommandBuffers[i], 0, 1, &theBuffer, bufferOffsets);

        uint32_t vertexCount = 3;
        uint32_t instanceCount = 1;
        vkCmdDraw(theCommandBuffers[i], vertexCount, instanceCount, 0, i);

        vkCmdEndRenderPass(theCommandBuffers[i]);
    

        /* 커멘드 버퍼를 다 작성했습니다. 버퍼에게 수고했다고 해주죠! */
        endCommandBuffer(i);
    }
    
    

    VkSemaphore imageAvailableSemaphores[IMAGES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[IMAGES_IN_FLIGHT];

    VkFence activedFences[IMAGES_IN_FLIGHT];
    VkFence * swapImageFences = malloc(sizeof(VkFence) * swapchainImageCount);

    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, // sType;
            NULL,                                    // pNext;
            0                                        // flags;
        };

        VkFenceCreateInfo fenceCreateInfo = {
            VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, // sType;
            NULL,                                // pNext;
            VK_FENCE_CREATE_SIGNALED_BIT         // flags;
        };

        for (int i = 0; i < IMAGES_IN_FLIGHT; i += 1)
        {
            printf("Creating image available semaphore: ");
            logResult(vkCreateSemaphore(theDevice, &semaphoreCreateInfo, NULL, &imageAvailableSemaphores[i]));
            printf("Creating image render finished semaphore: ");
            logResult(vkCreateSemaphore(theDevice, &semaphoreCreateInfo, NULL, &renderFinishedSemaphores[i]));

            printf("Creating Fence: ");
            logResult(vkCreateFence(theDevice, &fenceCreateInfo, NULL, &activedFences[i]));
        }
    }

    uint32_t activedSyncIndex = 0;
    /* Win32를 위한 드로콜과 메세지 헨들링입니다. 본격적인 렌더링의 세계가 여기서 시작되죠! */
#ifdef WIN32
    MSG Message;
    memset(&Message, 0, sizeof(Message));

    while (!isClosed)
    {
        if (PeekMessage(&Message, hWnd, 0, 0, PM_REMOVE) == TRUE) {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }

        

        /* 드로콜을 호출합니다! */

        vkWaitForFences(theDevice, 1, &activedFences[activedSyncIndex], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(theDevice, theSwapchain, UINT64_MAX, imageAvailableSemaphores[activedSyncIndex], VK_NULL_HANDLE, &imageIndex);

        if (swapImageFences[imageIndex] != VK_NULL_HANDLE)
        {
            vkWaitForFences(theDevice, 1, &activedFences[imageIndex], VK_TRUE, UINT64_MAX);
        }

        swapImageFences[imageIndex] = activedFences[activedSyncIndex];

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[activedSyncIndex] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[activedSyncIndex] };

        VkSubmitInfo submitInfo = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,  // sType
            NULL,                           // pNext;
            1,                              // waitSemaphoreCount;
            waitSemaphores,                 // pWaitSemaphores;
            waitStages,                     // pWaitDstStageMask;
            1,                              // commandBufferCount;
            &theCommandBuffers[imageIndex], // pCommandBuffers;
            1,                              // signalSemaphoreCount;
            signalSemaphores                // pSignalSemaphores;
        };

        vkResetFences(theDevice, 1, &activedFences[activedSyncIndex]);

        VkResult submit = vkQueueSubmit(theQueue, 1, &submitInfo, activedFences[activedSyncIndex]);
        if (submit != VK_SUCCESS)
        {
            printf("Some thing was wrong while submiting : %i\n", submit);
        }

        VkPresentInfoKHR presentInfo = {
            VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, // sType
            NULL,                               // pNext;
            1,                                  // waitSemaphoreCount;
            signalSemaphores,                   // pWaitSemaphores;
            1,                                  // swapchainCount;
            &theSwapchain,                      // pSwapchains;
            &imageIndex,                        // pImageIndices;
            NULL                                // pResults;
        };

        vkQueuePresentKHR(theQueue, &presentInfo);

        activedSyncIndex = (activedSyncIndex + 1) % IMAGES_IN_FLIGHT;

        
    }

    vkDeviceWaitIdle(theDevice);

#endif // WIN32

    for (uint32_t idx = 0; idx < IMAGES_IN_FLIGHT; idx++) {
        vkDestroySemaphore(theDevice, imageAvailableSemaphores[idx], NULL);
        vkDestroySemaphore(theDevice, renderFinishedSemaphores[idx], NULL);
        vkDestroyFence(theDevice, activedFences[idx], NULL);
    }

    freeCommandBuffer(swapchainImageCount);
    vkDestroyCommandPool(theDevice, theCommandPool, NULL);

    for (int i = 0; i < swapchainImageCount; i += 1)
    {
        vkDestroyFramebuffer(theDevice, swapchainFrameBuffers[i], NULL);
    }

    vkDestroyPipeline(theDevice, thePipeline, NULL);

    vkDestroyShaderModule(theDevice, fragmentShader, NULL);
    vkDestroyShaderModule(theDevice, vertexShader, NULL);

    vkDestroyPipelineLayout(theDevice, pipelineLayout, NULL);

    vkDestroyRenderPass(theDevice, theRenderPass, NULL);

    vkFreeMemory(theDevice, theBufferMemory, NULL);

    vkDestroyBuffer(theDevice, theBuffer, NULL);

    for (int i = 0; i < swapchainImageCount; i += 1)
    {
        vkDestroyImageView(theDevice, swapchainImageViews[i], NULL);
    }

    vkDestroySwapchainKHR(theDevice, theSwapchain, NULL);

    

    vkDestroyBufferView(theDevice, theBufferView, NULL);

    vkDestroyDevice(theDevice, NULL);

    vkDestroySurfaceKHR(theVulkan, surface, NULL);

    vkDestroyInstance(theVulkan, NULL);

    free(swapImageFences);
    free(swapchainFrameBuffers);
    free(swapchainImageViews);

#ifdef WIN32
    DestroyWindow(hWnd);
#endif

	return 0;
}
