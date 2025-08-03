#pragma once

#include "vulkan/vulkan.hpp"

#include "GLFW/glfw3.h"

class Surface
{

private:

    VkSurfaceKHR _surface;

    VkInstance _instance;

    GLFWwindow* const _window;

public:

    explicit Surface(VkInstance instance, GLFWwindow* window);

    VkSurfaceKHR GetSurface() const { return _surface; }

    ~Surface();

private:

    void CreateSurface();

};
