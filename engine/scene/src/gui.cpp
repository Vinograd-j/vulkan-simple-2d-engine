#include "../include/gui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

VkCommandBuffer Gui::PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const
{
    return _gui->PrepareCommandBuffer(imageIndex, imageView);
}

void Gui::DrawSceneGUI(const std::function<void()>& _onColorChangeButtonClicked) const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Test Window");
    ImGui::Text("Color changer");

    if (ImGui::Button("Change color", {150, 30}))
        _onColorChangeButtonClicked();

    ImGui::End();

    ImGui::Render();
}