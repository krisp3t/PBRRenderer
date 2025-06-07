#include <PBRRenderer/KrisWin.h>
#include <Window.h>

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR pArgs, INT)
{
    PBRRenderer::Window wnd(hInst, pArgs, 800, 600);
    if (!wnd.IsValid())
    {
        return -1;
    }
    // Message pump
    MSG msg = {0};
    BOOL result;
    while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (result == -1)
    {
        return -1;
    }
    return msg.wParam;
}