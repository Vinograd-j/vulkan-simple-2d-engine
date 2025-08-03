#include "../include/window.h"

void Window::CreateWindow()
{
    _window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
}

Window::~Window()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}
