#pragma once

#include "general.h"


namespace mt
{
enum struct GPUVendor
{
    NVIDIA,
    AMD,
    INTEL,
};

enum struct CodecType
{
    H264,
    HEVC,
    AV1,
};

struct GPUEncoder
{
    static inline std::wstring ADAPTER_NAME = {};
    static inline GPUVendor DEFAULT_VENDOR = {};

    GPUVendor vendor = DEFAULT_VENDOR;
    CodecType codec_type = {};

    void edit();
    std::wstring produce() const;

private:
    static inline const byte _init = []() -> byte
        {
            kl::GPU gpu{ nullptr };
            kl::ComRef<IDXGIDevice> dxgi_device;
            gpu.device()->QueryInterface( IID_PPV_ARGS( &dxgi_device ) ) >> kl::verify_result;
            kl::ComRef<IDXGIAdapter> dxgi_adapter;
            dxgi_device->GetAdapter( &dxgi_adapter ) >> kl::verify_result;
            DXGI_ADAPTER_DESC adapter_desc{};
            dxgi_adapter->GetDesc( &adapter_desc ) >> kl::verify_result;
            ADAPTER_NAME = adapter_desc.Description;
            switch ( adapter_desc.VendorId )
            {
            case 0x10DE: DEFAULT_VENDOR = GPUVendor::NVIDIA; break;
            case 0x1002: DEFAULT_VENDOR = GPUVendor::AMD; break;
            case 0x8086: DEFAULT_VENDOR = GPUVendor::INTEL; break;
            }
            return 0;
        }( );
};
}
