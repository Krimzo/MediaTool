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
    std::optional<VideoCodec> video_codec;
    kl::Float2 size_limits_mb = { 9.0f, 10.0f };
    int max_repeat_count = 10;
    std::wstring custom_commands;

    OptimizerSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {
        video_codec.emplace();
    }

    std::wstring produce( float bitrate_m ) const;
    void display() override;
    std::string optimize() const;

private:
    std::string m_last_error;

    float start_bitrate() const;
};
}
