#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct OptimizerSection : Displayable
{
    static const float4 COLOR;
    static constexpr std::string_view DESCRIPTION = "Media file size optimization.";

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    bool use_hardware_decoding = true;
    std::wstring input_file;
    std::wstring output_file;
    std::optional<Timestamp> start_time;
    std::optional<Timestamp> end_time;
    std::optional<VideoCodec> video_codec;
    float2 size_limits_mb = { 9.0f, 10.0f };
    int max_repeat_count = 10;
    std::string custom_commands;

    OptimizerSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {
        video_codec.emplace();
    }

    std::wstring produce( float bitrate_m ) const;
    void display() override;
    void optimize() const;

private:
    float start_bitrate() const;
};
}
