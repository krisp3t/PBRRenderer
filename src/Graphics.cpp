#include "Graphics.h"

namespace PBRRenderer
{

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

    D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &desc,
        &m_pSwapChain,
        &m_pDevice,
        nullptr,
        &m_pContext);

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
    m_pSwapChain->Present(1, 0);
}

void Graphics::ClearBuffer(float r, float g, float b, float a)
{
    const float clearColor[4] = {r, g, b, a};
    m_pContext->OMSetRenderTargets(1, &m_pTarget, nullptr);
    m_pContext->ClearRenderTargetView(m_pTarget, clearColor);
}

void Graphics::Present()
{
    m_pSwapChain->Present(1, 0);

}

} // namespace PBRRenderer
