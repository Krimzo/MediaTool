#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct ProcessSection : Displayable
{
    static constexpr std::string_view DEFAULT_IMAGE_OUTPUT_EXTENSION = ".jpg";
    static constexpr std::string_view DEFAULT_AUDIO_OUTPUT_EXTENSION = ".mp3";
    static constexpr std::string_view DEFAULT_VIDEO_OUTPUT_EXTENSION = ".mp4";
    static constexpr float DEFAULT_VIDEO_BITRATE_M = 3.0f;

    static const kl::Float4 COLOR;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    std::wstring input_dir;
    std::wstring output_dir;
    bool retain_folder_structure = true;
    std::optional<std::string> image_output_ext{ DEFAULT_IMAGE_OUTPUT_EXTENSION };
    std::optional<std::string> audio_output_ext{ DEFAULT_AUDIO_OUTPUT_EXTENSION };
    std::optional<std::string> video_output_ext{ DEFAULT_VIDEO_OUTPUT_EXTENSION };
    int max_image_dimension = 1920;
    int max_video_dimension = 1920;
    int max_video_framerate = 30;
    VideoCodec video_codec{};

    ProcessSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {
        video_codec.gpu_vendor = GPUVendor::NONE;
        video_codec.codec_type = VideoCodecType::HEVC;
    }

    std::wstring produce( fs::path const& input_file, fs::path* outout_file = nullptr ) const;
    void display() override;
    std::string process() const;

private:
    std::string m_last_error;
};
}
