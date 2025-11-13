#pragma once

#include <string>
#include <cstdint>

enum class WindowMode {
    Windowed,          // обычное окно
    BorderlessWindow,  // "полноэкранное в окне" (borderless)
    Fullscreen         // полноэкранное
};

class IWindow {
public:
    virtual ~IWindow() = default;

    // --- Инициализация / жизненный цикл ---
    virtual bool Create(int width, int height, const std::string& title) = 0;
    virtual bool ShouldClose() const = 0;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;

    // --- Параметры окна ---
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual void SetTitle(const std::string& title) = 0;
    virtual void Resize(int width, int height) = 0;

    // --- Режим отображения ---
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSyncEnabled() const = 0;

    virtual void SetWindowMode(WindowMode mode) = 0;
    virtual WindowMode GetWindowMode() const = 0;
};