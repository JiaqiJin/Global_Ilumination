#pragma once

// https://www.braynzarsoft.net/viewtutorial/q16390-directx-12-textures-from-file

#include <wincodec.h>

class Texture
{
public:
    Texture();
    Texture(const std::wstring& filename);
    Texture(const std::wstring& filename, UINT64 _textureID);
    ~Texture();
    Texture(const Texture& rhs) = delete;
    Texture& operator=(const Texture& rhs) = delete;


    bool InitializeTextureBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    ID3D12Resource* getTextureBuffer() { return mTextureBuffer.Get(); }
    D3D12_SHADER_RESOURCE_VIEW_DESC getSRVDESC() { return SRVDESC; }

private:
    bool CreateTextureBuffer(ID3D12Device* device);
    bool UploadTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdObj);

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

    Microsoft::WRL::ComPtr<ID3D12Resource> mTextureBuffer = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadHeap = nullptr;
    D3D12_RESOURCE_DESC textureDESC;
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDESC;
    int imageBytesPerRow;
    BYTE* imageData;
};