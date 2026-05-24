#pragma once

#include "codec.h"


namespace mt
{
struct Timestamp
{
    int minutes = 0;
    float seconds = 0.0f;

    void load( float seconds );
    float total_seconds() const;
};

struct VideoScale
{
    kl::Int2 scale;

    std::wstring produce() const;
};

struct VideoCrop
{
    kl::Int2 top_left{ 0 };
    kl::Int2 bottom_right{ 1 };

    std::wstring produce() const;
};

struct DefaultCodec
{
    std::variant<None, VideoScale, VideoCrop> video_viewport;
    std::optional<float> frame_rate;
    std::optional<float> video_bitrate_m;
    std::optional<VideoCodec> video_codec;
    std::optional<float> audio_bitrate_k;
    std::optional<AudioCodec> audio_codec;

    std::wstring produce() const;
};

struct CopyCodec
{
    std::wstring produce() const;
};

struct FFMPEGSection : Displayable
{
    static constexpr kl::Int2 DEFAULT_SCALE = { 1920, 1080 };
    static constexpr float DEFAULT_FRAME_RATE = 30.0f;
    static constexpr float DEFAULT_VIDEO_BITRATE_M = 5.0f;
    static constexpr float DEFAULT_AUDIO_BITRATE_K = 192.0f;

    static const kl::Float4 COLOR;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    std::wstring input_file;
    std::wstring output_file;
    std::optional<Timestamp> start_time;
    std::optional<Timestamp> end_time;
    std::variant<DefaultCodec, CopyCodec> codec;
    std::wstring custom_commands;

    FFMPEGSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {}

    std::wstring produce() const;
    void display() override;
};
}
