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

    std::vector<ConcatInput> inputs;
    std::wstring output_file;

    ConcatSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {}

    void concat() const;
    void display() override;
};
}
