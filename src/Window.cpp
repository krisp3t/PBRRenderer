#include "Window.h"

#include <iostream>

namespace PBRRenderer
{

Window::Window(HINSTANCE hInst, wchar_t *pArgs, int width, int height) : m_hInst(hInst), m_pArgs(pArgs)
{
    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = Window::SetupMessageHandler;
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
    if (m_hInst && m_pClassName)
    {
        UnregisterClass(m_pClassName, m_hInst);
    }
}

const wchar_t *Window::GetName() noexcept
{
    return m_pClassName;
}

HINSTANCE Window::GetInstance() noexcept
{
    return m_hInst;
}

LRESULT CALLBACK Window::SetupMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        DestroyWindow(hWnd);
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

} // namespace PBRRenderer