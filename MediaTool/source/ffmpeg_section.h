#pragma once

#include "gpu_encoder.h"


namespace mt
{
struct Timestamp
{
    int minutes = 0;
    float seconds = 0.0f;

    constexpr float total_seconds() const
    {
        return minutes * 60 + seconds;
    }
};

struct VideoScale
{
    kl::Int2 scale;

    std::wstring produce() const;
};

struct VideoCrop
{
    kl::Int2 position;
    kl::Int2 size{ 1 };

    std::wstring produce() const;
};

struct DefaultCodec
{
    std::variant<None, VideoScale, VideoCrop> video_viewport;
    std::optional<float> video_bitrate_m;
    std::optional<GPUEncoder> gpu_encoder;

    std::wstring produce() const;
};

struct CopyCodec
{
    std::wstring produce() const;
};

struct FFMPEGSection : Displayable
{
    static constexpr float DEFAULT_BITRATE_M = 5.0f;
    static constexpr kl::Int2 DEFAULT_SCALE = { 1920, 1080 };

    static const kl::Float4 COLOR;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    std::wstring input_file;
    std::wstring output_file;
    std::optional<Timestamp> start_time;
    std::optional<Timestamp> end_time;
    std::variant<DefaultCodec, CopyCodec> codec;
    std::wstring other_commands;

    FFMPEGSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {}

    std::wstring produce() const;
    void display() override;
};
}
