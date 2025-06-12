#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

namespace PBRRenderer
{

class Graphics
{
  public:
    struct GraphicsErrorInfo
    {
        HRESULT errorCode;
        const wchar_t *errorString;
        wchar_t errorDesc[256];
        const wchar_t *file;
        DWORD line;
    };

    Graphics(HWND hWnd);
    ~Graphics() = default;
    Graphics(const Graphics &) = delete;
    Graphics &operator=(const Graphics &) = delete;
    Graphics(Graphics &&) = delete;

    void EndFrame();
    void ClearBuffer(float r, float g, float b, float a = 1.0);
    void Present();
    void DrawTestTriangle(float angle);

  private:
    bool CheckAndLogError(HRESULT hr, const wchar_t *file, DWORD line);

  private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTarget = nullptr;
};

} // namespace PBRRenderer