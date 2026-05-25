#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct OptimizerSection : Displayable
{
    static const kl::Float4 COLOR;
    static constexpr float STARTING_BITRATE = 10.0f;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    std::wstring input_file;
    std::wstring output_file;
    std::optional<VideoCodec> video_codec;
    float max_size_mb = 10.0f;
    std::wstring custom_commands;

    OptimizerSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {
        video_codec.emplace();
    }

    std::wstring produce( float bitrate_m ) const;
    void display() override;
    void optimize() const;
};
}
