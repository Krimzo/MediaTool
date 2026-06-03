#include "progress_window.h"
#include "../resource.h"

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

mt::ProgressWindow::ProgressWindow( int count )
    : m_count( count )
{}

void mt::ProgressWindow::run( std::string_view const& title )
{
    kl::Window window{ title };
    kl::GPU gpu{ window.ptr() };
    progress_hwnd = window.ptr();

    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
        } );
    window.resize( { 250, 100 } );

    if ( HICON icon = LoadIcon( GetModuleHandleA( nullptr ), MAKEINTRESOURCE( IDI_ICON1 ) ) )
    {
        SendMessage( window.ptr(), WM_SETICON, ICON_BIG, (LPARAM) icon );
        SendMessage( window.ptr(), WM_SETICON, ICON_SMALL, (LPARAM) icon );
    }

    auto& progress_text = gpu.text_batch.emplace_back();
    progress_text.format = gpu.create_text_format( L"roboto", DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, 25.0f );
    progress_text.position = { -1.0f, 1.0f };
    progress_text.rect_size = { 2.0f, 2.0f };
    progress_text.hor_center = true;
    progress_text.ver_center = true;

    kl::dx::Buffer screen_mesh = gpu.create_screen_mesh();
    kl::Shaders shaders = gpu.create_shaders( SHADERS_SOURCE );
    gpu.bind_shaders( shaders );

    while ( window.process() )
    {
        const int progress = m_counter.load();

        struct CB
        {
            float X_OFFSET{};
            kl::Float3 PROGRESS_COLOR{};
        };

        CB cb{};
        cb.X_OFFSET = ( -1.0f + float( progress ) / m_count ) * 2.0f;
        cb.PROGRESS_COLOR = kl::RGB{ 21, 191, 103 };
        shaders.upload( cb );
        progress_text.data = kl::wformat( progress, "/", m_count );

        gpu.clear_internal( kl::colors::GRAY );
        gpu.draw( screen_mesh );
        gpu.draw_text_batch();
        gpu.swap_buffers( true );
    }
    progress_hwnd = nullptr;
}

void mt::ProgressWindow::increment()
{
    ++m_counter;
}

void mt::ProgressWindow::close()
{
    PostMessageA( progress_hwnd, WM_CLOSE, NULL, NULL );
}

bool mt::ProgressWindow::is_open() const
{
    return (bool) progress_hwnd;
}
