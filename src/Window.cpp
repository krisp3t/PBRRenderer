#include "Window.h"

#include <cassert>
#include <iostream>

namespace PBRRenderer
{

Window::Window(HINSTANCE hInst, const wchar_t *pArgs, int width, int height) : m_hInst(hInst), m_pArgs(pArgs)
{
    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = _HandleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = nullptr;
    if (!RegisterClassEx(&wc))
    {
        std::cerr << "Failed to register window class." << std::endl;
        return;
    }

    // Create window, get hWnd
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
    m_hWnd =
        CreateWindowEx(0, GetName(), L"PBRRenderer", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
                       CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, GetInstance(), this);

    if (!m_hWnd)
    {
        std::cerr << "Failed to get valid window handle" << std::endl;
        return;
    }
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);
}

Window::~Window()
{
    if (m_hInst)
    {
        UnregisterClass(s_ClassName, m_hInst);
    }
}

const wchar_t *Window::GetName() noexcept
{
    return s_ClassName;
}

HINSTANCE Window::GetInstance() const noexcept
{
    return m_hInst;
}

LRESULT WINAPI Window::_HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        // Get CREATESTRUCT passed to CreateWindowEx, containing window ptr
        const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
        auto *const pWnd = reinterpret_cast<Window *>(pCreate->lpCreateParams);
        assert(pWnd != nullptr);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::_HandleMsgThunk));
        // Forward message to window class handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::_HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window *const pWnd = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

HWND Window::GetHandle() noexcept
{
    return m_hWnd;
}

bool Window::IsValid() noexcept
{
    return m_hWnd != nullptr;
}

} // namespace PBRRenderer