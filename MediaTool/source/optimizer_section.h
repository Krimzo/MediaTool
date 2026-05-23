#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct OptimizerSection : Displayable
{
    static const kl::Float4 COLOR;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    std::wstring input_file;
    std::wstring output_file;
    std::optional<Timestamp> start_time;
    std::optional<Timestamp> end_time;
    std::optional<kl::Int2> video_scale;
    std::optional<GPUEncoder> gpu_encoder;
    float video_max_size_mb = 10.0f;

    OptimizerSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {}

    void optimize() const;
    void display() override;;
};
}
