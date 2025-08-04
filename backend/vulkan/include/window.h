#pragma once

#include <string>
#include <utility>
#include <GLFW/glfw3.h>

class Window
{

private:

    int _width;
    int _height;

    std::string _title;

private:

    GLFWwindow* _window {};

public:

    explicit Window(int height, int width, std::string  title) : _width(width), _height(height), _title(std::move(title)) {}

    void CreateWindow();

    GLFWwindow* WindowPointer() const { return _window; }

    ~Window();

};
