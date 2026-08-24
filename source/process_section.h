#pragma once

#include "ffmpeg_section.h"

namespace mt
{
enum struct MediaType
{
    IGNORED = 0,
    IMAGE,
    AUDIO,
    VIDEO,
};

struct ProcessSection : Displayable
{
    static constexpr std::string_view DEFAULT_IMAGE_OUTPUT_EXTENSION = ".jpg";
    static constexpr std::string_view DEFAULT_AUDIO_OUTPUT_EXTENSION = ".mp3";
    static constexpr std::string_view DEFAULT_VIDEO_OUTPUT_EXTENSION = ".mp4";
    static constexpr float DEFAULT_VIDEO_BITRATE_M = 3.0f;

    static const float4 COLOR;
    static constexpr std::string_view DESCRIPTION = "Bulk media processing.";

    static constexpr kl::RGB VIDEO_PROGRESS_COLOR = {151, 245, 100};
    static constexpr kl::RGB IMAGE_AUDIO_PROGRESS_COLOR = {247, 231, 106};

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    bool use_hardware_decoding = false;
    std::wstring input_dir;
    std::wstring output_dir;
    bool recursive_search = true;
    bool retain_folder_structure = false;
    std::optional<std::string> image_output_ext{DEFAULT_IMAGE_OUTPUT_EXTENSION};
    std::optional<std::string> audio_output_ext{DEFAULT_AUDIO_OUTPUT_EXTENSION};
    std::optional<std::string> video_output_ext{DEFAULT_VIDEO_OUTPUT_EXTENSION};
    int max_image_dimension = 1920;
    int max_video_dimension = 1920;
    int max_video_framerate = 30;
    std::optional<float> video_bitrate_m;
    VideoCodec video_codec;
    std::string image_custom_commands;
    std::string audio_custom_commands;
    std::string video_custom_commands;

    ProcessSection(kl::Window const& window, ImGuiContext* const& imgui_context)
        : window(window), imgui_context(imgui_context)
    {
        video_codec.codec_type = VideoCodecType::HEVC;
    }

    std::wstring produce(fs::path const& input_file, MediaType& out_media_type, fs::path* outout_file = nullptr) const;
    void display() override;
    void process() const;
};
} // namespace mt
