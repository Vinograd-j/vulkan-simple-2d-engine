#include "../include/gui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../objects/object-data.h"

VkCommandBuffer Gui::PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const
{
    return _gui->PrepareCommandBuffer(imageIndex, imageView);
}

void Gui::DrawSceneGUI(std::vector<ObjectData>& objects) const
{
    // ImGui_ImplVulkan_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    //
    // ImGui::Begin("Objects on the scene");
    //
    // for (size_t i = 0; i < objects.size(); ++i)
    // {
    //     auto& obj = objects[i];
    //
    //     // Секция для объекта
    //     ImGui::Separator();
    //     ImGui::Text("Object %zu", i + 1);
    //     ImGui::Text("Type: %d", obj._type);
    //
    //     // Отображаем текущий цвет
    //     ImGui::SameLine();
    //     ImGui::ColorButton(("##preview" + std::to_string(i)).c_str(),
    //                        ImVec4(obj._color.x, obj._color.y, obj._color.z, 1.0f),
    //                        ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,
    //                        ImVec2(20, 20));
    //
    //     // Кнопка "Change Color"
    //     ImGui::SameLine();
    //     if (ImGui::Button(("Change Color##" + std::to_string(i)).c_str()))
    //     {
    //         ImGui::OpenPopup(("ColorPicker##" + std::to_string(i)).c_str());
    //     }
    //
    //     // Всплывающее окно с палитрой
    //     if (ImGui::BeginPopup(("ColorPicker##" + std::to_string(i)).c_str()))
    //     {
    //         ImGui::Text("Pick a color:");
    //         ImGui::ColorEdit3(("##edit" + std::to_string(i)).c_str(), &obj._color.x);
    //         ImGui::EndPopup();
    //     }
    // }
    //
    // ImGui::End();
    // ImGui::Render();
}

