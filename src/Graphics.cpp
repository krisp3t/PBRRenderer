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

    ID3D11Texture2D *pBackBuffer = nullptr;
    m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
    m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pTarget);
    pBackBuffer->Release();
}

Graphics::~Graphics()
{
    m_pTarget->Release();
    m_pTarget = nullptr;
    m_pContext->Release();
    m_pContext = nullptr;
    m_pDevice->Release();
    m_pDevice = nullptr;
    m_pSwapChain->Release();
    m_pSwapChain = nullptr;
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
    HRESULT hr;
    // Create an invalid texture to force an error
    ID3D11Texture2D* pTexture = nullptr;
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 0;  // Invalid width to force error
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

    // This should trigger an error and DXTrace message
    GFX_RETURN_FAILED(m_pDevice->CreateTexture2D(&texDesc, nullptr, &pTexture));

    // Normal rendering code
    const float clearColor[4] = {r, g, b, a};
    m_pContext->OMSetRenderTargets(1, &m_pTarget, nullptr);
    m_pContext->ClearRenderTargetView(m_pTarget, clearColor);

    if (pTexture)
    {
        pTexture->Release();
    }
}

void Graphics::Present()
{
    m_pSwapChain->Present(1, 0);

}

} // namespace PBRRenderer
