#include "GLFWWindow.h"

#include <iostream>

GLFWWindow::GLFWWindow() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
    }
}

GLFWWindow::~GLFWWindow() {
    if (window_) glfwDestroyWindow(window_);
    glfwTerminate();
}

bool GLFWWindow::Create(int width, int height, const std::string& title) {
    width_ = width;
    height_ = height;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    SetVSync(false);
    return true;
}

bool GLFWWindow::ShouldClose() const {
    return glfwWindowShouldClose(window_);
}

void GLFWWindow::PollEvents() {
    glfwPollEvents();
}

void GLFWWindow::SwapBuffers() {
    glfwSwapBuffers(window_);
}

int GLFWWindow::GetWidth() const {
    return width_;
}

int GLFWWindow::GetHeight() const {
    return height_;
}

void GLFWWindow::SetTitle(const std::string& title) {
    if (window_) glfwSetWindowTitle(window_, title.c_str());
}

void GLFWWindow::Resize(int width, int height) {
    width_ = width;
    height_ = height;
    if (window_) glfwSetWindowSize(window_, width, height);
}

void GLFWWindow::SetVSync(bool enabled) {
    vsync_ = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

bool GLFWWindow::IsVSyncEnabled() const {
    return vsync_;
}

void GLFWWindow::SetWindowMode(WindowMode mode) {
    if (mode == mode_) return;
    mode_ = mode;
    ApplyWindowMode(mode_);
}

WindowMode GLFWWindow::GetWindowMode() const {
    return mode_;
}

void GLFWWindow::ApplyWindowMode(WindowMode mode) {
    GLFWmonitor* monitor = nullptr;
    const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    if (!window_ || !vidMode) return;

    switch (mode) {
        case WindowMode::Windowed:
            glfwSetWindowMonitor(window_, nullptr, 100, 100, width_, height_, vidMode->refreshRate);
            glfwSetWindowAttrib(window_, GLFW_DECORATED, GLFW_TRUE);
            break;

        case WindowMode::BorderlessWindow:
            glfwSetWindowMonitor(window_, nullptr, 0, 0, vidMode->width, vidMode->height, vidMode->refreshRate);
            glfwSetWindowAttrib(window_, GLFW_DECORATED, GLFW_FALSE);
            break;

        case WindowMode::Fullscreen:
            monitor = glfwGetPrimaryMonitor();
            glfwSetWindowMonitor(window_, monitor, 0, 0, vidMode->width, vidMode->height, vidMode->refreshRate);
            break;
    }
}