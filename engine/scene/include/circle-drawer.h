#pragma once

#include <memory>

#include "allocator.h"
#include "circle-command-buffer-recorder.h"
#include "synchronization.h"
#include "../../../backend/vulkan/buffers/include/uniform-buffer.h"
#include "../../gui/include/im-gui.h"
#include "../../pipeline/include/graphics-pipeline.h"
#include "../../swapchain/include/present-swapchain.h"
#include "../../renderer/include/renderer.h"

constexpr int FRAMES_IN_FLIGHT = 2;

class CircleDrawer : Renderer
{

private:

    const Allocator* _allocator;

    const CommandPool* const _commandPool;

    CommandBuffers _commandBuffers;

    const ImGUI* const _gui;

private:

    Synchronization _syncObjects;

    std::vector<VkImageView> _imageViews;

    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<IndexBuffer> _indexBuffer;
    std::vector<std::unique_ptr<UniformBuffer>> _uniformBuffers;

    std::vector<VkImageLayout> _swapchainImageLayouts;

    std::unique_ptr<CircleCommandBufferRecorder> _recorder;

    VkDescriptorSetLayout _descriptorSetLayout;
    VkDescriptorPool _descriptorPool;
    std::vector<VkDescriptorSet> _descriptorSets;

    glm::vec3 _circleColor;

private:

    uint32_t _currentFrame = 0;

public:

    explicit CircleDrawer(const Allocator* allocator, const CommandPool* pool, const CommandBuffers& buffers, const GraphicsPipeline* pipeline, PresentSwapchain* swapchain, const LogicalDevice* device, const VkDescriptorSetLayout& layout, const ImGUI* gui);

    void DrawFrame() override;

    void GuiFrame();

    ~CircleDrawer() override;

    std::vector<VkImageView> GetImageViews() const { return _imageViews; }

private:

    void CreateVertexBuffer(float radius, int segmentCount);
    void CreateIndexBuffer(int segmentCount);

    VkImageSubresourceRange GetImageSubresourceRange() const;

    VkComponentMapping GetComponentMapping() const;

    void RecreateSwapchain();

    void CreateBufferRecorder();

    void CreateUniformBuffers();
    void CreateDescriptorSets();
    void CreateDescriptorPool();

    void UpdateUniformBuffer(uint32_t currentFrame) const;
};