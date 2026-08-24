#include "progress_window.h"

static constexpr auto SHADERS_SOURCE = R"(
float X_OFFSET;
float3 PROGRESS_COLOR;

float4 v_shader(float3 position : KL_Position) : SV_Position
{
    return float4(position.x + X_OFFSET, position.y, position.z, 1.0f);
}

float4 p_shader(float4 position : SV_Position) : SV_Target0
{
    return float4(PROGRESS_COLOR, 1.0f);
}
)";

mt::ProgressWindow::ProgressWindow(int count) : m_count(count)
{
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

    kl::dx::Buffer screen_mesh = gpu.create_screen_mesh();
    kl::Shaders shaders = gpu.create_shaders(SHADERS_SOURCE);
    gpu.bind_shaders(shaders);

    while (window.process())
    {
        const int progress = m_counter.load();

        struct CB
        {
            float X_OFFSET{};
            float3 PROGRESS_COLOR{};
        };

        CB cb{};
        cb.X_OFFSET = (-1.0f + float(progress) / m_count) * 2.0f;
        cb.PROGRESS_COLOR = this->progress_color;
        shaders.upload(cb);
        progress_text->data = kl::wformat(progress, "/", m_count);

        gpu.clear_internal(MAIN_BACKGROUND_COLOR);
        gpu.draw(screen_mesh);
        gpu.draw_raster_batch();
        gpu.swap_buffers(true);
    }
    progress_hwnd = nullptr;
}

void mt::ProgressWindow::increment()
{
    ++m_counter;
}

void mt::ProgressWindow::close()
{
    PostMessageA(progress_hwnd, WM_CLOSE, NULL, NULL);
}

bool mt::ProgressWindow::is_open() const
{
    return (bool)progress_hwnd;
}
