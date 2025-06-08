#include "App.h"
#include "Window.h"

namespace PBRRenderer
{

App::App(const std::string &commandLine)
    : m_CommandLine(commandLine), m_Window(GetModuleHandle(nullptr), L"PBRRenderer", 800, 600)
{
}

App::~App()
{
}

int App::Run()
{
    MSG msg = {0};
    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return static_cast<int>(msg.wParam);
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Perform frame updates and input handling
        HandleInput(0.0f);
        DoFrame(0.0f);
    }
}

void App::DoFrame(float dt)
{
}

void App::HandleInput(float dt)
{
}

} // namespace PBRRenderer