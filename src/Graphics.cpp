#include "Graphics.h"
#include <corecrt_wstdio.h>
#include <dxerr/dxerr.h>
#include <iterator>

#define GFX_RETURN_FAILED(hrcall)                                                                  \
    do                                                                                             \
    {                                                                                              \
        HRESULT hr = (hrcall);                                                                     \
        if (FAILED(hr))                                                                            \
        {                                                                                          \
            CheckAndLogError(hr, __FILEW__, __LINE__);                                             \
        }                                                                                          \
    } while (0)

namespace wrl = Microsoft::WRL;

namespace PBRRenderer
{

bool Graphics::CheckAndLogError(HRESULT hr, const wchar_t *file, DWORD line)
{
    GraphicsErrorInfo errorInfo;
    errorInfo.errorCode = hr;
    errorInfo.errorString = DXGetErrorStringW(hr);
    errorInfo.file = file;
    errorInfo.line = line;
    DXGetErrorDescriptionW(hr, errorInfo.errorDesc, 256);

    wchar_t errorMsg[512];
    swprintf_s(errorMsg,
        512,
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

    HRESULT hr = 0;

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
    HRESULT hr = 0;
    if (FAILED(m_pSwapChain->Present(1, 0)))
    {
        GFX_RETURN_FAILED(hr);
    }
}

void Graphics::ClearBuffer(float r, float g, float b, float a)
{
    const float clearColor[4] = {r, g, b, a};

    m_pContext->ClearRenderTargetView(m_pTarget.Get(), clearColor);
}

void Graphics::Present()
{
    m_pSwapChain->Present(1, 0);
}

void Graphics::DrawTestTriangle()
{
    namespace wrl = Microsoft::WRL;
    struct Vertex
    {
        struct
        {
            float x;
            float y;
        } pos;
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        } color;
    };
    const Vertex vertices[] = {
        {0.0f, 0.5f, 0, 0, 0, 255},
        {0.5f, -0.5f, 0, 255, 0, 255},
        {-0.5f, -0.5f, 255, 0, 255, 255},
        {-0.3f, 0.3f, 0, 255, 0, 0},
        {0.3f, 0.3f, 0, 0, 255, 0},
        {0.0f, -0.8f, 255, 0, 0, 0},
    };
    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0;
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {
        .pSysMem = &vertices[0],
    };
    HRESULT hr = 0;
    GFX_RETURN_FAILED(m_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    wrl::ComPtr<ID3DBlob> pBlob;

    wrl::ComPtr<ID3D11PixelShader> pPixelShader = nullptr;
    D3DReadFileToBlob(L"../shaders/PixelShader.cso", &pBlob);
    GFX_RETURN_FAILED(m_pDevice->CreatePixelShader(pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        nullptr,
        &pPixelShader));

    wrl::ComPtr<ID3D11VertexShader> pVertexShader = nullptr;
    D3DReadFileToBlob(L"../shaders/VertexShader.cso", &pBlob);
    GFX_RETURN_FAILED(m_pDevice->CreateVertexShader(pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        nullptr,
        &pVertexShader));

    m_pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);
    m_pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
    m_pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

    const unsigned short indices[] = {
        0,
        1,
        2,
        0,
        2,
        3,
        0,
        4,
        1,
        2,
        1,
        5,
    };
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.ByteWidth = sizeof(indices);
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.CPUAccessFlags = 0u;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;
    GFX_RETURN_FAILED(m_pDevice->CreateBuffer(&indexBufferDesc, &indexData, &pIndexBuffer));
    m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    D3D11_INPUT_ELEMENT_DESC layout[]
        = {{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {
                "Color",
                0,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                0,
                8u,
                D3D11_INPUT_PER_VERTEX_DATA,
                0,
            }};
    GFX_RETURN_FAILED(m_pDevice->CreateInputLayout(layout,
        (UINT)std::size(layout),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout));

    m_pContext->IASetInputLayout(pInputLayout.Get());
    m_pContext->OMSetRenderTargets(1, m_pTarget.GetAddressOf(), nullptr);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT vp = {};
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &vp);

    m_pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
}

} // namespace PBRRenderer
