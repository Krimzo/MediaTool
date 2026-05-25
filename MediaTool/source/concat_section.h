#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct BoundingBox
{
    ImVec2 top_left{};
    ImVec2 bottom_right{};
};

struct ConcatFileInfo
{
    kl::Int2 video_resolution;
    int frame_rate = 0;
    int audio_rate = 0;

    bool load( fs::path const& path );
    bool match( ConcatFileInfo const& other ) const;

    void display( ConcatFileInfo const* to_compare, std::vector<BoundingBox>& out_boxes );
};

struct ConcatInput
{
    std::wstring path;
    kl::Float4 color;
    ConcatFileInfo info;

    ConcatInput();

    bool set_path( std::wstring path );
    void reload();

private:
    fs::file_time_type m_last_write_time{};
};

struct ConcatSection : Displayable
{
    static const kl::Float4 COLOR;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    const std::wstring vids_list_file = kl::wformat( "concat_vids_list__", kl::convert_string( kl::random::gen_string( 10 ) ), ".txt" );

    std::vector<ConcatInput> inputs;
    std::wstring output_file;
    bool use_copy_codec = true;
    std::wstring custom_commands;

    ConcatSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {}

    std::wstring produce() const;
    void display() override;

    void concat() const;
};
}
