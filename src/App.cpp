#include "App.h"
#include "Window.h"

namespace PBRRenderer
{

App::App(const std::string &commandLine)
    : m_CommandLine(commandLine), m_Wnd(GetModuleHandle(nullptr), L"PBRRenderer", 800, 600),
      m_LastFrameTime(std::chrono::steady_clock::now()),
      m_SinceAppStart(std::chrono::steady_clock::now())

{
}

App::~App()
{
}

float App::CalculateDeltaTime()
{
    const auto currentTime = std::chrono::steady_clock::now();
    const float deltaTime = std::chrono::duration<float>(currentTime - m_LastFrameTime).count();
    m_LastFrameTime = currentTime;
    return deltaTime;
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

        const float dt = CalculateDeltaTime();
        HandleInput(dt);
        DoFrame(dt);
    }
}

void App::DoFrame(float dt)
{
    m_Wnd.Gfx().ClearBuffer(0.5f, 0.5f, 1.0f);
    m_Wnd.Gfx().DrawTestTriangle(
        std::chrono::duration<float>(std::chrono::steady_clock::now() - m_SinceAppStart).count(),
        0.0f,
        0.0f);
    m_Wnd.Gfx().DrawTestTriangle(
        -std::chrono::duration<float>(std::chrono::steady_clock::now() - m_SinceAppStart).count(),
        0.5f,
        0.5f);
    m_Wnd.Gfx().EndFrame();
}

void App::HandleInput(float dt)
{
}

} // namespace PBRRenderer