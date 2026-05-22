#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct ConcatInput
{
    std::wstring path;
    std::optional<Timestamp> start_time;
    std::optional<Timestamp> end_time;
};

struct ConcatSection
{
    static const kl::Float4 COLOR;

    ImGuiContext* const& imgui_context;

    std::vector<ConcatInput> inputs;
    std::wstring output_file;
    kl::Int2 video_scale = { 1920, 1080 };
    int video_rate = 30;
    int audio_rate = 48000;
    std::optional<float> video_bitrate_m;
    std::optional<GPUEncoder> gpu_encoder;

    ConcatSection( ImGuiContext* const& imgui_context )
        : imgui_context( imgui_context )
    {}

    void concat() const;
    void display();
};
}
