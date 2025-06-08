#pragma once
#include <PBRRenderer/KrisWin.h>
#include <Window.h>
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

  private:
    std::string m_CommandLine;
    Window m_Window;
};

} // namespace PBRRenderer