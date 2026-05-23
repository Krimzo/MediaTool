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

    kl::Ref<Displayable> current_section = new FFMPEGSection( imgui_context );

    MediaTool();
    virtual ~MediaTool() noexcept;

    bool update();

private:
    void load_theme();
    int get_taskbar_height();
};
}
