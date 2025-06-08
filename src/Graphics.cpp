#include "Graphics.h"
#include <corecrt_wstdio.h>
#include <dxerr/dxerr.h>

#define GFX_RETURN_FAILED(hrcall) \
do { \
HRESULT hr = (hrcall); \
if (FAILED(hr)) { \
CheckAndLogError(hr, __FILEW__, __LINE__); \
} \
} while(0)

namespace wrl = Microsoft::WRL;

namespace PBRRenderer
{

bool Graphics::CheckAndLogError(HRESULT hr, const wchar_t* file, DWORD line)
{
    GraphicsErrorInfo errorInfo;
    errorInfo.errorCode = hr;
    errorInfo.errorString = DXGetErrorStringW(hr);
    errorInfo.file = file;
    errorInfo.line = line;
    DXGetErrorDescriptionW(hr, errorInfo.errorDesc, 256);

    wchar_t errorMsg[512];
    swprintf_s(errorMsg, 512,
        L"Graphics Error occurred!\n"
        L"File: %s\n"
        L"Line: %lu\n"
        L"Error Code: 0x%08X\n"
        L"Error: %s\n"
        L"Description: %s",
        errorInfo.file,
        errorInfo.line,
        errorInfo.errorCode,
        errorInfo.errorString,
        errorInfo.errorDesc);

    MessageBoxW(nullptr, errorMsg, L"Graphics Error", MB_OK | MB_ICONERROR);
    return false;
}

Graphics::Graphics(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = 2;
    desc.BufferDesc.Width = 800;
    desc.BufferDesc.Height = 600;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.RefreshRate.Numerator = 0;
    desc.BufferDesc.RefreshRate.Denominator = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.OutputWindow = hWnd;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Windowed = TRUE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    desc.Flags = 0;

    HRESULT hr;

    GFX_RETURN_FAILED(D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        &m_pSwapChain,
        &m_pDevice,
        nullptr,
        &m_pContext));

    wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
    GFX_RETURN_FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (LPVOID *)&pBackBuffer));
    GFX_RETURN_FAILED(m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_pTarget));
}

void Graphics::EndFrame()
{
    HRESULT hr;
    if (FAILED(m_pSwapChain->Present(1, 0)))
    {
        GFX_RETURN_FAILED(hr);
    }
}

void Graphics::ClearBuffer(float r, float g, float b, float a)
{
    const float clearColor[4] = {r, g, b, a};
    m_pContext->OMSetRenderTargets(1, m_pTarget.GetAddressOf(), nullptr);
    m_pContext->ClearRenderTargetView(m_pTarget.Get(), clearColor);
}

void Graphics::Present()
{
    m_pSwapChain->Present(1, 0);

}

} // namespace PBRRenderer
