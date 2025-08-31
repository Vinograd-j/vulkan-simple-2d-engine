#include "../include/gui.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "../include/scene.h"
#include "../objects/shapes/include/circle.h"
#include "../objects/shapes/include/square.h"
#include "../objects/shapes/include/triangle.h"
#include "magic_enum.hpp"

VkCommandBuffer Gui::PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const
{
    return _gui->PrepareCommandBuffer(imageIndex, imageView);
}

void Gui::DrawSceneGUI(Scene& scene) const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Objects on the scene");

    auto& objects = scene.GetBufferObjects();

    for (size_t i = 0; i < objects.size(); ++i)
    {
        auto& obj = objects[i];

        ImGui::Separator();
        std::string label = "Object " + std::to_string(i + 1) + " " +
                            std::string(magic_enum::enum_name(static_cast<ShapeType>(obj._objectId)));
        ImGui::Text("%s", label.c_str());


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
        ImGui::SliderFloat("X", &obj._model[3].x, -5, 5);
        ImGui::SliderFloat("Y", &obj._model[3].y, -5, 5);
        ImGui::PopID();
    }

    if (ImGui::Button("Add New Circle##add_circle"))
        scene.AddObject(std::make_shared<Circle>(0.2, 1024));
    if (ImGui::Button("Add New Triangle##add_triangle"))
        scene.AddObject(std::make_shared<Triangle>());
    if (ImGui::Button("Add New Square##add_square"))
        scene.AddObject(std::make_shared<Square>());

    static constexpr KeyMapping keyMappings[] = {
        { ImGuiKey_W, CameraKey::W },
        { ImGuiKey_S, CameraKey::S },
        { ImGuiKey_A, CameraKey::A },
        { ImGuiKey_D, CameraKey::D }
    };

    for (auto& m : keyMappings) {
        if (ImGui::IsKeyPressed(m._imguiKey))
            scene.GetCamera().Process(m._key, true);
        if (ImGui::IsKeyReleased(m._imguiKey))
            scene.GetCamera().Process(m._key, false);
    }

    ImGui::End();
    ImGui::Render();
}