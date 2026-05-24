#include "codec.h"


std::wstring mt::VideoCodec::produce() const
{
    std::wstring gpu_vendor_str;
    switch ( gpu_vendor )
    {
    case GPUVendor::NVIDIA: gpu_vendor_str = L"nvenc"; break;
    case GPUVendor::AMD: gpu_vendor_str = L"amf"; break;
    case GPUVendor::INTEL: gpu_vendor_str = L"qsv"; break;
    }
    std::wstring type_str;
    switch ( codec_type )
    {
    case VideoCodecType::H264: type_str = L"h264"; break;
    case VideoCodecType::HEVC: type_str = L"hevc"; break;
    case VideoCodecType::AV1: type_str = L"av1"; break;
    }
    if ( gpu_vendor_str.empty() )
        return type_str;
    else
        return kl::wformat( type_str, "_", gpu_vendor_str );
}

void mt::VideoCodec::edit()
{
    bool none = gpu_vendor == GPUVendor::NONE;
    if ( im::Checkbox( QNAME( "None" ), &none ) )
        gpu_vendor = GPUVendor::NONE;
    im::SameLine();
    im::PushStyleColor( ImGuiCol_CheckMark, (ImU32) ImColor( 56, 245, 85 ) );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 56, 245, 85 ) );
    bool nvidia = gpu_vendor == GPUVendor::NVIDIA;
    if ( im::Checkbox( QNAME( "Nvidia" ), &nvidia ) )
        gpu_vendor = GPUVendor::NVIDIA;
    im::SameLine();
    im::PushStyleColor( ImGuiCol_CheckMark, (ImU32) ImColor( 245, 56, 56 ) );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 245, 56, 56 ) );
    bool amd = gpu_vendor == GPUVendor::AMD;
    if ( im::Checkbox( QNAME( "AMD" ), &amd ) )
        gpu_vendor = GPUVendor::AMD;
    im::SameLine();
    im::PushStyleColor( ImGuiCol_CheckMark, (ImU32) ImColor( 56, 195, 245 ) );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 56, 195, 245 ) );
    bool intel = gpu_vendor == GPUVendor::INTEL;
    if ( im::Checkbox( QNAME( "Intel" ), &intel ) )
        gpu_vendor = GPUVendor::INTEL;
    im::PopStyleColor( 6 );

    im::Text( "\t" );
    im::SameLine();
    bool h264 = ( codec_type == VideoCodecType::H264 );
    if ( im::Checkbox( QNAME( "H264" ), &h264 ) )
        codec_type = VideoCodecType::H264;
    im::SameLine();
    bool hevc = ( codec_type == VideoCodecType::HEVC );
    if ( im::Checkbox( QNAME( "HEVC" ), &hevc ) )
        codec_type = VideoCodecType::HEVC;
    im::SameLine();
    bool av1 = ( codec_type == VideoCodecType::AV1 );
    if ( im::Checkbox( QNAME( "AV1" ), &av1 ) )
        codec_type = VideoCodecType::AV1;
}

bool mt::VideoCodec::uses_gpu() const
{
    return gpu_vendor != GPUVendor::NONE;
}

std::wstring mt::AudioCodec::produce() const
{
    switch ( codec_type )
    {
    default: return {};
    case AudioCodecType::AAC: return L"aac";
    case AudioCodecType::MP3: return L"libmp3lame";
    case AudioCodecType::OPUS: return L"libopus";
    case AudioCodecType::FLAC: return L"flac";
    }
}

void mt::AudioCodec::edit()
{
    bool aac = ( codec_type == AudioCodecType::AAC );
    if ( im::Checkbox( QNAME( "AAC" ), &aac ) )
        codec_type = AudioCodecType::AAC;
    im::SameLine();
    bool mp3 = ( codec_type == AudioCodecType::MP3 );
    if ( im::Checkbox( QNAME( "MP3" ), &mp3 ) )
        codec_type = AudioCodecType::MP3;
    im::SameLine();
    bool opus = ( codec_type == AudioCodecType::OPUS );
    if ( im::Checkbox( QNAME( "OPUS" ), &opus ) )
        codec_type = AudioCodecType::OPUS;
    im::SameLine();
    bool flac = ( codec_type == AudioCodecType::FLAC );
    if ( im::Checkbox( QNAME( "FLAC" ), &flac ) )
        codec_type = AudioCodecType::FLAC;
}
