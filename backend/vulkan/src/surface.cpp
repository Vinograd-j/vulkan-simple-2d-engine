#include "../include/surface.h"

Surface::Surface(VkInstance instance, GLFWwindow* window) : _instance(instance), _window(window)
{
    CreateSurface();
}

void Surface::CreateSurface()
{
    if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface");
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
}