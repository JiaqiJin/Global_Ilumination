#pragma once

// https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-textures-from-file

#include <wincodec.h>

#include "../RHI/DX12/DX12_CommandContext.h"

class Texture {
public:

    Texture();
    Texture(const std::wstring& filename);
    Texture(const std::wstring& filename, UINT64 _textureID);

    Texture(const Texture& rhs) = delete;
    Texture& operator=(const Texture& rhs) = delete;

    virtual bool InitializeTextureBuffer(ID3D12Device* device, DX12_CommandContext* cmdObj);

    ID3D12Resource* getTextureBuffer();
    D3D12_SHADER_RESOURCE_VIEW_DESC getSRVDESC();

    virtual ~Texture();

private:

    virtual bool createTextureBuffer(ID3D12Device* device);
    virtual bool uploadTexture(ID3D12Device* device, DX12_CommandContext* cmdObj);

    int LoadImageDataFromFile(BYTE** imageData,
        D3D12_RESOURCE_DESC& resourceDescription,
        LPCWSTR filename,
        int& bytesPerRow);

    int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat);
    DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID);
    WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID);

    void cleanUpImageByte();

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
