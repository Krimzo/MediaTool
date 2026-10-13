#include "progress_window.h"

static constexpr auto SHADERS_SOURCE = R"(
float3 PROGRESS_COLOR;
float X_SCALE;
float X_OFFSET;

float4 v_shader(float3 position : KL_Position) : SV_Position
{
    return float4(position.x * X_SCALE + X_OFFSET, position.y, position.z, 1.0f);
}

float4 p_shader(float4 position : SV_Position) : SV_Target0
{
    return float4(PROGRESS_COLOR, 1.0f);
}
)";

bool mt::ProgressSector::increment()
{
    if (counter == total_count)
        return false;
    counter++;
    return true;
}

void mt::ProgressWindow::run(std::string_view const& title)
{
    kl::Window window{title};
    kl::GPU gpu{window.ptr()};
    progress_hwnd = window.ptr();

    window.on_resize.emplace_back([&](int2 size) {
        gpu.resize_internal(size);
        gpu.set_viewport_size(size);
    });
    window.resize({250, 100});
    window.set_icon("icons/icon.ico");

    kl::Ref<kl::RasterText> progress_text = new kl::RasterText();
    progress_text->format =
        gpu.create_text_format(L"roboto", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, 25.0f);
    progress_text->box_top_left = {-1.0f, 1.0f};
    progress_text->box_bottom_right = {1.0f, -1.0f};
    progress_text->h_align = kl::HAlign::DWRITE_TEXT_ALIGNMENT_CENTER;
    progress_text->v_align = kl::VAlign::DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    gpu.raster_batch.push_back(progress_text);

    kl::dx::DepthState depth_state = gpu.create_depth_state(false);
    gpu.bind_depth_state(depth_state);

    kl::dx::Buffer screen_mesh = gpu.create_screen_mesh();
    kl::Shaders shaders = gpu.create_shaders(SHADERS_SOURCE);
    gpu.bind_shaders(shaders);

    while (window.process())
    {
        gpu.clear_internal(MAIN_BACKGROUND_COLOR);

        struct alignas(16) CB
        {
            float3 PROGRESS_COLOR{};
            float X_SCALE{1};
            float X_OFFSET{};
        };

        int total_count = 0;
        for (auto& sector : sectors)
            total_count += sector.total_count;

        int offset = 0;
        int total_progress = 0;
        for (auto& sector : sectors)
        {
            CB cb{};
            cb.X_SCALE = sector.total_count / (float)total_count;
            cb.X_OFFSET = -1.0f + cb.X_SCALE + offset / (float)total_count * 2.0f;
            cb.PROGRESS_COLOR = (float3)sector.color * progress_color_dim;
            shaders.upload(cb);
            gpu.draw(screen_mesh);

            cb = {};
            cb.X_SCALE = sector.counter / (float)total_count;
            cb.X_OFFSET = -1.0f + cb.X_SCALE + offset / (float)total_count * 2.0f;
            cb.PROGRESS_COLOR = sector.color;
            shaders.upload(cb);
            gpu.draw(screen_mesh);

            offset += sector.total_count;
            total_progress += sector.counter;
        }

        progress_text->data = kl::wformat(total_progress, "/", total_count);
        gpu.draw_raster_batch();

        gpu.swap_buffers(true);
    }
    progress_hwnd = nullptr;
}

void mt::ProgressWindow::increment()
{
    std::lock_guard lock{m_mutex};
    for (auto& sector : sectors)
        if (sector.increment())
            break;
}

void mt::ProgressWindow::close()
{
    PostMessageA(progress_hwnd, WM_CLOSE, NULL, NULL);
}

bool mt::ProgressWindow::is_open() const
{
    return (bool)progress_hwnd;
}
