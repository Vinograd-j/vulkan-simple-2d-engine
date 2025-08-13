#include "../include/gui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../../../backend/vulkan/buffers/include/storage-buffer.h"
#include "../objects/object-data.h"

VkCommandBuffer Gui::PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const
{
    return _gui->PrepareCommandBuffer(imageIndex, imageView);
}

void Gui::DrawSceneGUI(std::vector<StorageBufferObject>& objects)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Objects on the scene");

    for (size_t i = 0; i < objects.size(); ++i)
    {
        auto& obj = objects[i];

        ImGui::Separator();
        ImGui::Text("Object %zu", i + 1);

        ImGui::SameLine();
        ImGui::ColorButton(("##preview" + std::to_string(i)).c_str(),
                           ImVec4(obj._color.x, obj._color.y, obj._color.z, 1.0f),
                           ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,
                           ImVec2(20, 20));

        ImGui::SameLine();
        if (ImGui::Button(("Change Color##" + std::to_string(i)).c_str()))
        {
            ImGui::OpenPopup(("ColorPicker##" + std::to_string(i)).c_str());
        }

        if (ImGui::BeginPopup(("ColorPicker##" + std::to_string(i)).c_str()))
        {
            ImGui::Text("Pick a color:");
            ImGui::ColorEdit3(("##edit" + std::to_string(i)).c_str(), &obj._color.x);
            ImGui::EndPopup();
        }

        ImGui::PushID(i);
        ImGui::SliderFloat("X", &obj._model[3].x, -0.39, 0.39);
        ImGui::SliderFloat("Y", &obj._model[3].y, -0.4, 0.4);
        ImGui::PopID();
    }

    ImGui::End();
    ImGui::Render();
}