#pragma once

#include <wincodec.h>
#include "../Utilities/Utils.h"
#include "../DX12/CommandContext.h"

class Texture
{
public:
    Texture();
    Texture(const std::wstring& filename);
    Texture(const std::wstring& filename, UINT64 _textureID);

    Texture(const Texture& rhs) = delete;
    Texture& operator=(const Texture& rhs) = delete;

    virtual ~Texture();

    virtual bool initializeTextureBuffer(ID3D12Device* device, CommandContext* cmdContext);

    ID3D12Resource* GetTextureBuffer();
    D3D12_SHADER_RESOURCE_VIEW_DESC GetSRVDesc();

protected:
    virtual bool CreateTextureBuffer(ID3D12Device* device);
    virtual bool UploadTexture(ID3D12Device* device, CommandContext* cmdContext);

    int LoadImageDataFromFile(BYTE** imageData,
        D3D12_RESOURCE_DESC& resourceDescription,
        LPCWSTR filename,
        int& bytesPerRow);

    int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
    DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
    WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);

    void CleanUpImageByte();

public:
    INT64 textureID;

    std::string Name;
    std::wstring Filename;

    Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
    D3D12_RESOURCE_DESC textureDESC;
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDESC;
    int imageBytesPerRow;
    BYTE* imageData;
};

class RenderTexture : public Texture
{
public:

    RenderTexture();
    RenderTexture(UINT _width, UINT _height, DXGI_FORMAT _format);

    ID3D12Resource* GetDepthStencilBuffer();
    D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc();
    D3D12_DEPTH_STENCIL_VIEW_DESC GetDsvDesc();

    virtual bool initializeTextureBuffer(ID3D12Device* device, CommandContext* cmdContext);

protected:

    virtual bool CreateTextureBuffer(ID3D12Device* device);
    virtual bool UploadTexture(ID3D12Device* device, CommandContext* cmdContext);

private:
    UINT width, height;

    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
    D3D12_RESOURCE_DESC depthStencilDESC;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDESC;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDESC;
};