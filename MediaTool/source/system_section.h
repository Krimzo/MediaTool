#pragma once

#include "general.h"


namespace mt
{
struct SystemSection : Displayable
{
    static const kl::Float4 COLOR;
    static constexpr std::string_view DESCRIPTION = "Dependency installation/media inspection.";

    kl::Window const& window;
    ImGuiContext* const& imgui_context;

    SystemSection( kl::Window const& window, ImGuiContext* const& imgui_context )
        : window( window )
        , imgui_context( imgui_context )
    {}

    void display() override;
};
}
