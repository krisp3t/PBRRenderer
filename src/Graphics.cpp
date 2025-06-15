#include "Graphics.h"

#include <DirectXMath.h>
#include <cmath>
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
namespace dx = DirectX;

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

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    wrl::ComPtr<ID3D11DepthStencilState> pDSState;
    GFX_RETURN_FAILED(m_pDevice->CreateDepthStencilState(&dsDesc, &pDSState));
    m_pContext->OMSetDepthStencilState(pDSState.Get(), 0u);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = 800;
    depthDesc.Height = 600;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
    GFX_RETURN_FAILED(m_pDevice->CreateTexture2D(&depthDesc, nullptr, &pDepthStencil));
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    GFX_RETURN_FAILED(m_pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &m_pDSV));
    m_pContext->OMSetRenderTargets(1, m_pTarget.GetAddressOf(), m_pDSV.Get());
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
    m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::Present()
{
    m_pSwapChain->Present(1, 0);
}

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
    namespace wrl = Microsoft::WRL;
    struct Vertex
    {
        struct
        {
            float x;
            float y;
            float z;
        } pos;
    };
    const Vertex vertices[] = {
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {-1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
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

    const unsigned short indices[] = {0,
        2,
        1,
        2,
        3,
        1,
        1,
        3,
        5,
        3,
        7,
        5,
        2,
        6,
        3,
        3,
        6,
        7,
        4,
        5,
        7,
        4,
        7,
        6,
        0,
        4,
        2,
        2,
        4,
        6,
        0,
        1,
        4,
        1,
        5,
        4};
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

    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };
    struct ConstantBuffer2
    {
        struct
        {
            float r;
            float g;
            float b;
            float a;
        } face_colors[6];
    };
    const ConstantBuffer cb
        = {dx::XMMatrixTranspose(dx::XMMatrixRotationX(angle) * dx::XMMatrixTranslation(x, y, 4.0f)
                                 * dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f))};

    const ConstantBuffer2 cb2 = {{
        {1.0f, 0.0f, 1.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 1.0f},
    }};
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(cb);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA cbData = {};
    cbData.pSysMem = &cb;
    GFX_RETURN_FAILED(m_pDevice->CreateBuffer(&cbDesc, &cbData, &pConstantBuffer));
    m_pContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());

    wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
    D3D11_BUFFER_DESC cbd2;
    cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd2.Usage = D3D11_USAGE_DEFAULT;
    cbd2.CPUAccessFlags = 0u;
    cbd2.MiscFlags = 0u;
    cbd2.ByteWidth = sizeof(cb2);
    cbd2.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd2 = {};
    csd2.pSysMem = &cb2;
    GFX_RETURN_FAILED(m_pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));
    m_pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    D3D11_INPUT_ELEMENT_DESC layout[]
        = {{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};
    GFX_RETURN_FAILED(m_pDevice->CreateInputLayout(layout,
        (UINT)std::size(layout),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout));

    m_pContext->IASetInputLayout(pInputLayout.Get());

    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT vp = {};
    vp.Width = 800;
    vp.Height = 600;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &vp);
    m_pContext->OMSetRenderTargets(1, m_pTarget.GetAddressOf(), m_pDSV.Get());

    m_pContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u);
}

} // namespace PBRRenderer
