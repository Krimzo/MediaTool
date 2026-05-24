#pragma once

#include "general.h"


namespace mt
{
enum struct GPUVendor
{
    NONE,
    NVIDIA,
    AMD,
    INTEL,
};

enum struct VideoCodecType
{
    H264,
    HEVC,
    AV1,
};

enum struct AudioCodecType
{
    AAC,
    MP3,
    OPUS,
    FLAC,
};

inline const std::wstring GPU_ADAPTER_NAME = []() -> std::wstring
    {
        kl::GPU gpu{ nullptr };
        kl::ComRef<IDXGIDevice> dxgi_device;
        gpu.device()->QueryInterface( IID_PPV_ARGS( &dxgi_device ) ) >> kl::verify_result;
        kl::ComRef<IDXGIAdapter> dxgi_adapter;
        dxgi_device->GetAdapter( &dxgi_adapter ) >> kl::verify_result;
        DXGI_ADAPTER_DESC adapter_desc{};
        dxgi_adapter->GetDesc( &adapter_desc ) >> kl::verify_result;
        return adapter_desc.Description;
    }( );

inline const GPUVendor CURRENT_GPU_VENDOR = []() -> GPUVendor
    {
        kl::GPU gpu{ nullptr };
        kl::ComRef<IDXGIDevice> dxgi_device;
        gpu.device()->QueryInterface( IID_PPV_ARGS( &dxgi_device ) ) >> kl::verify_result;
        kl::ComRef<IDXGIAdapter> dxgi_adapter;
        dxgi_device->GetAdapter( &dxgi_adapter ) >> kl::verify_result;
        DXGI_ADAPTER_DESC adapter_desc{};
        dxgi_adapter->GetDesc( &adapter_desc ) >> kl::verify_result;
        switch ( adapter_desc.VendorId )
        {
        default: return GPUVendor::NONE;
        case 0x10DE: return GPUVendor::NVIDIA;
        case 0x1002: return GPUVendor::AMD;
        case 0x8086: return GPUVendor::INTEL;
        }
    }( );

struct VideoCodec
{
    GPUVendor gpu_vendor = CURRENT_GPU_VENDOR;
    VideoCodecType codec_type = VideoCodecType::H264;

    std::wstring produce() const;
    void edit();

    bool uses_gpu() const;
};

struct AudioCodec
{
    AudioCodecType codec_type = AudioCodecType::AAC;

    std::wstring produce() const;
    void edit();
};
}
