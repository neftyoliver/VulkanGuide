#include "forthProgram.hpp"
#include "vulkanHelper.hpp"

int main() {
	
	vkhelp::VulkanHelper vulkanHelper("Hello?");

	auto physicalDevices = vulkanHelper.getPhysicalDevices();

	uint32_t bestDeviceIndex = 0;
	uint32_t deviceRating = 0;
	uint32_t currentRating = 0;

	for (int i = 0; i < physicalDevices.size(); i += 1)
	{
		currentRating = vkhelp::ratePhysicalDevice(physicalDevices[i]);
		if (currentRating > deviceRating)
		{
			deviceRating = currentRating;
			bestDeviceIndex = i;
		}
	}

	vulkanHelper.selectPhysicalDevice(bestDeviceIndex);
	
}