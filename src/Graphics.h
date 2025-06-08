#pragma once
#include <d3d11.h>

namespace PBRRenderer
{

class Graphics
{
  public:
    Graphics(HWND hWnd);
    ~Graphics();
    Graphics(const Graphics &) = delete;
    Graphics &operator=(const Graphics &) = delete;
    Graphics(Graphics &&) = delete;

    void EndFrame();
    void ClearBuffer(float r, float g, float b, float a);
    void Present();

  private:
    ID3D11Device *m_pDevice = nullptr;
    ID3D11DeviceContext *m_pContext = nullptr;
    IDXGISwapChain *m_pSwapChain = nullptr;
    ID3D11RenderTargetView *m_pTarget = nullptr;
};

} // namespace PBRRenderer