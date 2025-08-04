#include "../include/im-gui.h"

#include "image-memory-barrier.h"
#include "../../../cmake-build-debug/_deps/imgui-src/imgui.h"
#include "../../../cmake-build-debug/_deps/imgui-src/backends/imgui_impl_glfw.h"
#include "../../../cmake-build-debug/_deps/imgui-src/backends/imgui_impl_vulkan.h"

ImGUI::ImGUI(const PresentSwapchain* swapchain, const Window* window, const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice,
    const Instance* instance, const VkCommandPool& commandPool) : _swapchain(swapchain),
                                                                  _window(window),
                                                                  _physicalDevice(physicalDevice),
                                                                  _device(logicalDevice),
                                                                  _instance(instance),
                                                                  _commandPool(commandPool)
{
    CreateDescriptorPool();
    InitImGUI();
    CreateCommandBufferRecorder();
}

void ImGUI::InitImGUI()
{
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    io.DisplaySize.x = _swapchain->GetExtent().width;
    io.DisplaySize.y = _swapchain->GetExtent().height;

    ImGui::GetStyle().FontScaleMain = 1.5;

    ImGui::StyleColorsLight();

    bool installGLFWCallbacks = true;
    ImGui_ImplGlfw_InitForVulkan(_window->WindowPointer(), installGLFWCallbacks);

    ImGui_ImplVulkan_InitInfo initInfo {};
    initInfo.ApiVersion = _instance->GetInstanceVersion();
    initInfo.Instance = _instance->GetInstance();
    initInfo.PhysicalDevice = _physicalDevice->GetPhysicalDevice();
    initInfo.Device = _device->GetDevice();
    initInfo.QueueFamily = _physicalDevice->GetFamilies(_physicalDevice->GetPhysicalDevice()).at(GRAPHICS);
    initInfo.Queue = _device->GetQueues().at(GRAPHICS);
    initInfo.RenderPass = nullptr;
    initInfo.Subpass = 0;
    initInfo.PipelineCache = nullptr;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.UseDynamicRendering = true;
    initInfo.MinImageCount = _physicalDevice->QuerySwapChainSupportDetails(_physicalDevice->GetPhysicalDevice())._capabilities.minImageCount;
    initInfo.ImageCount = _swapchain->GetImageCount();
    initInfo.DescriptorPool = _descriptorPool;

    VkPipelineRenderingCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    createInfo.viewMask = 0;
    createInfo.colorAttachmentCount = 1;

    VkFormat format = _swapchain->GetImageFormat();
    createInfo.pColorAttachmentFormats = &format;
    createInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
    createInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

    initInfo.PipelineRenderingCreateInfo = createInfo;
    initInfo.Allocator = nullptr;

    ImGui_ImplVulkan_Init(&initInfo);

    _commandBuffers = std::make_unique<CommandBuffers>(_swapchain->GetImageCount(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, _commandPool, _device);
}

void ImGUI::CreateDescriptorPool()
{
    VkDescriptorPoolSize poolSizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo poolCreateInfo {};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolCreateInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolCreateInfo.poolSizeCount = IM_ARRAYSIZE(poolSizes);
    poolCreateInfo.pPoolSizes = poolSizes;

    if (vkCreateDescriptorPool(_device->GetDevice(), &poolCreateInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor pool for imgui!");

}

void ImGUI::CreateCommandBufferRecorder()
{
    _recorder = std::make_unique<ImGUICommandBufferRecorder>(_commandBuffers.get(), _swapchain->GetExtent());
}

VkCommandBuffer ImGUI::PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const
{
    _recorder->RecordCommandBuffer(imageIndex, imageView);

    ImDrawData* drawData = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(drawData, _commandBuffers->GetCommandBuffers()[imageIndex]);

    vkCmdEndRendering(_commandBuffers->GetCommandBuffers()[imageIndex]);

    vkEndCommandBuffer(_commandBuffers->GetCommandBuffers()[imageIndex]);

    return _commandBuffers->GetCommandBuffers()[imageIndex];
}

void ImGUI::Destroy()
{
    _recorder.reset();
    _commandBuffers.reset();

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(_device->GetDevice(), _descriptorPool, nullptr);
}

ImGUI::~ImGUI()
{
    Destroy();
}