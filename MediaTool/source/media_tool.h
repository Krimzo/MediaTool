#pragma once

#include "system_section.h"
#include "ytdlp_section.h"
#include "ffmpeg_section.h"
#include "optimizer_section.h"
#include "concat_section.h"


namespace mt
{
struct MediaTool
{
    kl::Window window{ "Media Tool" };
    kl::GPU gpu{ window.ptr() };
    ImGuiContext* imgui_context = nullptr;

    kl::Ref<SystemSection> system_section = new SystemSection( window );
    kl::Ref<YTDLPSection> ytdlp_section = new YTDLPSection( window, imgui_context );
    kl::Ref<FFMPEGSection> ffmpeg_section = new FFMPEGSection( window, imgui_context );
    kl::Ref<OptimizerSection> optimizer_section = new OptimizerSection( window, imgui_context );
    kl::Ref<ConcatSection> concat_section = new ConcatSection( window, imgui_context );
    kl::Ref<Displayable> current_section = ffmpeg_section;

    MediaTool();
    virtual ~MediaTool() noexcept;

    bool update();

private:
    void load_theme();
    int get_taskbar_height();
};
}
