#include <chrono>
#include <imgui.h>
#include <iostream>
#include <vector>

#include "vulkan-application.h"

#include <GLFW/glfw3.h>

#include "logical-device.h"
#include "physical-device.h"
#include "surface.h"
#include "window.h"
#include "engine/pipeline/include/graphics-pipeline.h"
#include "engine/swapchain/include/present-swapchain.h"
#include "allocator.h"
#include "engine/descriptors/include/descriptor-set-layout.h"
#include "engine/gui/include/im-gui.h"
#include "engine/scene/include/circle-drawer.h"

std::vector<const char*> GetRequiredExtensions()
{
    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUser)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

inline void PopulateDebugUtilsCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo, const VkDebugUtilsMessageSeverityFlagsEXT& messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT& messageType,
                               PFN_vkDebugUtilsMessengerCallbackEXT debugCallback)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = messageSeverity;
    createInfo.messageType = messageType;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

std::vector<const char*> vl = {"VK_LAYER_KHRONOS_validation" };

bool CheckValidationLayersSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : vl)
    {
        bool layerFound = false;

        for (const auto & layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

bool debugMode = true;

double lastTime = 0.0;
int frameCount = 0;
void updateFPS() {
    double currentTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();

    frameCount++;

    if (currentTime - lastTime >= 1.0) {
        std::cout << "FPS: " << frameCount << std::endl;
        frameCount = 0;
        lastTime = currentTime;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    VulkanApplication application("Vulkan", VK_MAKE_VERSION(1, 0, 0),"No engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_3, nullptr);

    std::unique_ptr<Instance> instance;
    if (CheckValidationLayersSupport() && debugMode)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugUtilsCreateInfo(createInfo, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, DebugCallback);

        instance = std::make_unique<Instance>(application, GetRequiredExtensions(), vl, &createInfo, 0);
    }else
    {
        instance = std::make_unique<Instance>(application, GetRequiredExtensions(), vl, nullptr, 0);
    }

    instance->CreateInstance();

    Window window(600, 800, "Vulkan");
    window.CreateWindow();

    std::unique_ptr<Surface> surface = std::make_unique<Surface>(instance->GetInstance(), window.WindowPointer());

    std::unique_ptr<PhysicalDevice> device = std::make_unique<PhysicalDevice>(instance->GetInstance(), surface->GetSurface(), deviceExtensions);
    device->ChoosePhysicalDevice(7942, 4318);

    std::unique_ptr<LogicalDevice> logicalDevice = std::make_unique<LogicalDevice>(*device);

    std::unique_ptr<PresentSwapchain> swapchain = std::make_unique<PresentSwapchain>(device.get(), logicalDevice.get(), window.WindowPointer(), surface->GetSurface(), VK_NULL_HANDLE);

// START DESCRIPTORS


    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::vector bindings { uboLayoutBinding };
    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout = std::make_unique<DescriptorSetLayout>(bindings, logicalDevice.get());
    std::vector descriptorSetLayouts { descriptorSetLayout->GetDescriptorLayout() };

    std::unique_ptr<PipelineLayout> layout = std::make_unique<PipelineLayout>(descriptorSetLayouts, std::vector<VkPushConstantRange>(), logicalDevice.get());


// END DESCRIPTORS

    std::unique_ptr<ShaderModule> module1 = std::make_unique<ShaderModule>(logicalDevice.get(), VK_SHADER_STAGE_VERTEX_BIT, "vert");
    std::unique_ptr<ShaderModule> module2 = std::make_unique<ShaderModule>(logicalDevice.get(), VK_SHADER_STAGE_FRAGMENT_BIT, "frag");
    std::vector<ShaderModule*> shaderModules;
    shaderModules.push_back(module1.get());
    shaderModules.push_back(module2.get());

    std::unique_ptr<GraphicsPipeline> pipeline = std::make_unique<GraphicsPipeline>(shaderModules, layout.get(), logicalDevice.get());

    std::unique_ptr<CommandPool> pool = std::make_unique<CommandPool>(logicalDevice.get(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    std::unique_ptr<CommandBuffers> commandBuffers = std::make_unique<CommandBuffers>(2, VK_COMMAND_BUFFER_LEVEL_PRIMARY, pool.get()->GetCommandPool(), logicalDevice.get());

    std::unique_ptr<Allocator> allocator = std::make_unique<Allocator>(device.get(), logicalDevice.get(), instance.get());

    std::unique_ptr<ImGUI> gui = std::make_unique<ImGUI>(swapchain.get(), &window, device.get(), logicalDevice.get(), instance.get(), pool.get()->GetCommandPool());

    std::unique_ptr<CircleDrawer> circleDrawer = std::make_unique<CircleDrawer>(allocator.get(), pool.get(), *commandBuffers.get(), pipeline.get(), swapchain.get(), logicalDevice.get(), descriptorSetLayout->GetDescriptorLayout(), gui.get());


    while (!glfwWindowShouldClose(window.WindowPointer()))
    {

        circleDrawer.get()->DrawFrame();
        glfwPollEvents();

        if (debugMode)
            updateFPS();
    }

    vkDeviceWaitIdle(logicalDevice.get()->GetDevice());

    gui.reset();
    pool.reset();
    commandBuffers.reset();
    circleDrawer.reset();
    allocator.reset();
    pipeline.reset();
    layout.reset();
    descriptorSetLayout.reset();
    swapchain.reset();
    surface.reset();
    module1.reset();
    module2.reset();
    logicalDevice.reset();

    return 0;
}