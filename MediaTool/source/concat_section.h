#pragma once

#include "ffmpeg_section.h"


namespace mt
{
struct ConcatInput
{
    std::wstring path;
    kl::Float4 color;

    ConcatInput();
};

struct ConcatSection : Displayable
{
    static const kl::Float4 COLOR;

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    const std::wstring vids_list_file = kl::wformat( "concat_vids_list__", kl::convert_string( kl::random::gen_string( 10 ) ), ".txt" );

    std::vector<ConcatInput> inputs;
    std::wstring output_file;
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
