#pragma once

#include "IWindow.h"
#include <GLFW/glfw3.h>

class GLFWWindow : public IWindow {
public:
    GLFWWindow();
    ~GLFWWindow() override;

    bool Create(int width, int height, const std::string& title) override;
    bool ShouldClose() const override;
    void PollEvents()  override;
    void SwapBuffers() override;

    int GetWidth()  const override;
    int GetHeight() const override;
    void SetTitle(const std::string& title) override;
    void Resize(int width, int height) override;

    void SetVSync(bool enabled) override;
    bool IsVSyncEnabled() const override;

    void SetWindowMode(WindowMode mode) override;
    WindowMode GetWindowMode()    const override;

    GLFWwindow* GetNativeHandle() const { return window_; }

private:
    GLFWwindow* window_ = nullptr;
    int width_  = 0;
    int height_ = 0;
    bool vsync_ = false;
    WindowMode mode_ = WindowMode::Windowed;

    void ApplyWindowMode(WindowMode mode);
};