#pragma once
#include "Graphics.h"
#include <memory>
#include <string>

namespace PBRRenderer
{
class Window
{
  public:
    Window(HINSTANCE hInst, const wchar_t *pArgs, int width, int height);
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;

    void Kill()
    {
        PostQuitMessage(0);
    };

    static const wchar_t *GetName() noexcept;
    HINSTANCE GetInstance() const noexcept;
    HWND GetHandle() noexcept;
    bool IsValid() noexcept;
    Graphics &Gfx();

  private:
    static LRESULT WINAPI _HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT WINAPI _HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  private:
    static constexpr auto s_ClassName = L"PBRRendererWindow";
    HINSTANCE m_hInst = nullptr;
    std::wstring m_pArgs;
    HWND m_hWnd = nullptr;
    std::unique_ptr<Graphics> m_pGraphics;
};
} // namespace PBRRenderer
