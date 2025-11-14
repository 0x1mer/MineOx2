#pragma once

#include <string>
#include <cstdint>

enum class WindowMode {
    Windowed,          // Normal window
    BorderlessWindow,  // Bordeles
    Fullscreen         // Full screen
};

class IWindow {
public:
    virtual ~IWindow() = default;

    // --- Initialize / lifecycle ---
    virtual bool Create(int width, int height, const std::string& title) = 0;
    virtual bool ShouldClose() const = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;

    // --- Window properties ---
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual void Resize(int width, int height) = 0;

    // --- Display Mode ---
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSyncEnabled() const = 0;

    virtual void SetWindowMode(WindowMode mode) = 0;
    virtual WindowMode GetWindowMode() const = 0;
};