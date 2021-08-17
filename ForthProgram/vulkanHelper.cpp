
#include "vulkanHelper.hpp"

namespace vkhelp {

    uint32_t ratePhysicalDevice(vk::PhysicalDevice physicalDevice)
    {
        vk::PhysicalDeviceProperties physicalDeviceProperties;
        physicalDeviceProperties = physicalDevice.getProperties();

        uint64_t avgDeviceProperties = 0;
        vk::PhysicalDeviceLimits limits = physicalDeviceProperties.limits;

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
                limits.maxColorAttachments +
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

    VkHelperResult VulkanHelper::addLayer(const char * paramLayerName)
    {
        

        for(auto layer : vecAvailableLayers)
        {
            if(strcmp(layer.layerName, paramLayerName) == 0)
            {

                this->vecLayers.push_back(paramLayerName);

                std::cout << "Adding layer: " << paramLayerName << std::endl;

                return vkhelp::VkHelperResult::SUCCESS;
            }
        }

        std::cout << "Skip adding layer which is unvailable: " << paramLayerName << std::endl;
        return vkhelp::VkHelperResult::LAYER_UNEXIST;
    }

    VkHelperResult VulkanHelper::addExtension(const char* paramExtensionName)
    {
        for(auto extension : vecAvailableExtensions)
        {
            if(strcmp(extension.extensionName, paramExtensionName) == 0)
            {
                this->vecExtensions.push_back(paramExtensionName);

                std::cout << "Adding extension: " << paramExtensionName << std::endl;

                return vkhelp::VkHelperResult::SUCCESS;
            }
        }

        std::cout << "Skip adding extension which is unvailable: " << paramExtensionName << std::endl;
        return vkhelp::VkHelperResult::EXTENSION_UNEXIST;
    }

    VulkanHelper::VulkanHelper(const char* programName)
    {
        this->vecAvailableLayers = vk::enumerateInstanceLayerProperties();
        this->vecAvailableExtensions = vk::enumerateInstanceExtensionProperties();


#ifdef DEBUG
        std::cout << "Available layers: " << std::endl;
        for (auto layer : this->vecAvailableLayers)
        {
            std::cout << "\t" << layer.layerName << std::endl;
        }
        std::cout << std::endl;

        std::cout << "Available extensions:  " << std::endl;
        for (auto extension : this->vecAvailableExtensions)
        {
            std::cout << "\t" << extension.extensionName << std::endl;
        }

#ifdef LINUX
        addLayer("VK_LAYER_LUNARG_standard_validation");
#else
        addLayer("VK_LAYER_KHRONOS_validation");
#endif
#endif

        vk::ApplicationInfo appInfo(programName, VK_MAKE_API_VERSION(0, 0, 0, 0), "VulkanHelper");

        vk::InstanceCreateInfo instnaceCreateInfo({}, &appInfo,
            static_cast<uint32_t>(this->vecLayers.size()), this->vecLayers.data(),
            static_cast<uint32_t>(this->vecExtensions.size()), this->vecExtensions.data());

        auto result = vk::createInstance(&instnaceCreateInfo, nullptr, &(this->vulkanInstance));

        if (result == vk::Result::eSuccess)
        {
            std::cout << "Hello, Vulkan!" << std::endl;
        }
        else
        {
            std::cout << "Some thing was wrong while creating instance with code: " << result << std::endl;
            exit(-1);
        }

        this->vecPhysicalDevices = this->vulkanInstance.enumeratePhysicalDevices();

        std::cout << std::endl;


    }

    std::vector<vk::PhysicalDevice> VulkanHelper::getPhysicalDevices()
    {
        return this->vecPhysicalDevices;
    }

    VkHelperResult VulkanHelper::selectPhysicalDevice(uint32_t index)
    {


        if (this->vecPhysicalDevices.size() <= index)
        {
            return vkhelp::VkHelperResult::OUT_OF_INDEX_BOUND;
        }

        this->selectedDevice = this->vecPhysicalDevices[index];

#ifdef DEBUG
        std::cout << "Device selected: " << this->selectedDevice.getProperties().deviceName << std::endl;
#endif

        return vkhelp::VkHelperResult::SUCCESS;
    }

    vk::PhysicalDevice VulkanHelper::getSelectedPhysicalDevice()
    {
        return this->selectedDevice;
    }

    VulkanHelper::~VulkanHelper()
    {
        vulkanInstance.destroy();
    }
}