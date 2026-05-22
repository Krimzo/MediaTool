#include "gpu_encoder.h"


void mt::GPUEncoder::edit()
{
    im::PushStyleColor( ImGuiCol_CheckMark, (ImU32) ImColor( 56, 245, 85 ) );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 56, 245, 85 ) );
    bool nvidia = vendor == GPUVendor::NVIDIA;
    if ( im::Checkbox( QNAME( "Nvidia" ), &nvidia ) )
        vendor = GPUVendor::NVIDIA;
    im::SameLine();
    im::PushStyleColor( ImGuiCol_CheckMark, (ImU32) ImColor( 245, 56, 56 ) );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 245, 56, 56 ) );
    bool amd = vendor == GPUVendor::AMD;
    if ( im::Checkbox( QNAME( "AMD" ), &amd ) )
        vendor = GPUVendor::AMD;
    im::SameLine();
    im::PushStyleColor( ImGuiCol_CheckMark, (ImU32) ImColor( 56, 195, 245 ) );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 56, 195, 245 ) );
    bool intel = vendor == GPUVendor::INTEL;
    if ( im::Checkbox( QNAME( "Intel" ), &intel ) )
        vendor = GPUVendor::INTEL;
    im::PopStyleColor( 6 );

    im::Text( "\t" );
    im::SameLine();
    bool h264 = codec_type == CodecType::H264;
    if ( im::Checkbox( QNAME( "H264" ), &h264 ) )
        codec_type = CodecType::H264;
    im::SameLine();
    bool hevc = codec_type == CodecType::HEVC;
    if ( im::Checkbox( QNAME( "HEVC" ), &hevc ) )
        codec_type = CodecType::HEVC;
    im::SameLine();
    bool av1 = codec_type == CodecType::AV1;
    if ( im::Checkbox( QNAME( "AV1" ), &av1 ) )
        codec_type = CodecType::AV1;
}

std::wstring mt::GPUEncoder::produce() const
{
    std::wstring vendor_str;
    switch ( vendor )
    {
    case GPUVendor::NVIDIA: vendor_str = L"nvenc"; break;
    case GPUVendor::AMD: vendor_str = L"amf"; break;
    case GPUVendor::INTEL: vendor_str = L"qsv"; break;
    }
    std::wstring type_str;
    switch ( codec_type )
    {
    case CodecType::H264: type_str = L"h264"; break;
    case CodecType::HEVC: type_str = L"hevc"; break;
    case CodecType::AV1: type_str = L"av1"; break;
    }
    return kl::wformat( type_str, "_", vendor_str );
}
