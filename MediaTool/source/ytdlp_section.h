#pragma once

#include "general.h"


namespace mt
{
struct YTDLPSection : Displayable
{
    static const kl::Float4 COLOR;

    ImGuiContext* const& imgui_context;

    std::wstring url;
    std::optional<std::wstring> output_file;
    bool audio_only = false;

    YTDLPSection( ImGuiContext* const& imgui_context )
        : imgui_context( imgui_context )
    {}

    std::wstring produce() const;
    void display() override;;
};
}
