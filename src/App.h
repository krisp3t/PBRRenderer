#pragma once
#include <Window.h>
#include <chrono>
#include <string>

namespace PBRRenderer
{

class App
{
  public:
    App(const std::string &commandLine);
    ~App();
    App(const App &) = delete;
    App &operator=(const App &) = delete;
    App(App &&) = delete;

    int Run();

  private:
    void DoFrame(float dt);
    void HandleInput(float dt);
    float CalculateDeltaTime();

  private:
    std::string m_CommandLine;
    Window m_Wnd;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    std::chrono::steady_clock::time_point m_SinceAppStart;
};

} // namespace PBRRenderer