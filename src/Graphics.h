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

  private:
    ID3D11Device *m_pDevice = nullptr;
    ID3D11DeviceContext *m_pContext = nullptr;
    IDXGISwapChain *m_pSwapChain = nullptr;
};

} // namespace PBRRenderer