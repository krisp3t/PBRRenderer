#pragma once
#include <PBRRenderer/KrisWin.h>
#include <string>

namespace PBRRenderer
{
class Window
{
  public:
    Window(HINSTANCE hInst, wchar_t *pArgs, int width, int height);
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;

    void Kill()
    {
        PostQuitMessage(0);
    };

    const wchar_t *GetName() noexcept;
    HINSTANCE GetInstance() noexcept;

  private:
    static LRESULT CALLBACK SetupMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

  private:
    const wchar_t *m_pClassName = L"PBRRendererWindow";
    HINSTANCE m_hInst = nullptr;
    std::wstring m_pArgs;
    HWND m_hWnd = nullptr;
};
} // namespace PBRRenderer
