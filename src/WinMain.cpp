#include <App.h>
#include <PBRRenderer/KrisWin.h>
#include <Window.h>

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR pArgs, INT)
{
    std::wstring WideArgs(pArgs);
    std::string CommandLine(WideArgs.begin(), WideArgs.end());
    return PBRRenderer::App(CommandLine).Run();
}