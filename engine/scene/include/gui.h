#pragma once

#include <vector>

#include "../../gui/include/im-gui.h"
#include "../../struct/storage-buffer.h"

class Gui
{

private:

    const ImGUI* const _gui;

public:

    explicit Gui(const ImGUI* gui) : _gui(gui) {}

    VkCommandBuffer PrepareCommandBuffer(uint32_t imageIndex, const VkImageView& imageView) const;

    void DrawSceneGUI(std::vector<StorageBufferObject>& objects);

};
