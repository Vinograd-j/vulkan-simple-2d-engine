#pragma once

#include <imgui.h>

#include "scene.h"
#include "../../gui/include/im-gui.h"

struct KeyMapping {
    ImGuiKey _imguiKey;
    CameraKey _key;
};

class Gui
{

private:

    const ImGUI* const _gui;

public:

    explicit Gui(const ImGUI* gui) : _gui(gui) {}

    VkCommandBuffer PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const;

    void DrawSceneGUI(Scene& scene) const;

};
