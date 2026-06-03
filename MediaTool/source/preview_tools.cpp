#include "preview_tools.h"
#include "media_tool.h"
#include "../resource.h"

void mt::preview_timestamp( fs::path const& path, Timestamp& timestamp )
{
    kl::VideoReader reader{ path.wstring() };
    kl::Window window{ kl::format( "Timestamp Preview ", path.filename().string(), " [", timestamp.minutes, ":", timestamp.seconds, "]" ) };
    kl::GPU gpu{ window.ptr() };

    auto* imgui_context = im::CreateContext();
    im::SetCurrentContext( imgui_context );

    auto& imio = im::GetIO();
    imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imio.IniFilename = nullptr;

    ImGui_ImplWin32_Init( window.ptr() );
    ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    im::StyleColorsDark();
    mt::MediaTool::load_theme();

    kl::dx::Texture preview_texture;
    kl::dx::ShaderView preview_view;
    const auto reload_texture = [&]
        {
            kl::Image frame;
            reader.seek( timestamp.total_seconds() );
            float frame_time = -std::numeric_limits<float>::infinity();
            do {
                float current_frame_time;
                reader.read_frame( frame, nullptr, &current_frame_time );
                if ( current_frame_time == frame_time )
                    break;
                frame_time = current_frame_time;
            }
            while ( frame_time < timestamp.total_seconds() );
            preview_texture = gpu.create_texture( frame );
            preview_view = gpu.create_shader_view( preview_texture, nullptr );
        };
    reload_texture();

    window.set_dark_mode( true );
    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
        } );
    window.resize( gpu.texture_size( preview_texture ) );
    window.set_position( kl::SCREEN_SIZE / 2 - window.size() / 2 );
    window.maximize();

    if ( HICON icon = LoadIcon( GetModuleHandleA( nullptr ), MAKEINTRESOURCE( IDI_ICON1 ) ) )
    {
        SendMessage( window.ptr(), WM_SETICON, ICON_BIG, (LPARAM) icon );
        SendMessage( window.ptr(), WM_SETICON, ICON_SMALL, (LPARAM) icon );
    }

    while ( window.process() )
    {
        gpu.clear_internal( kl::colors::GRAY );
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        im::NewFrame();
        ImGuizmo::BeginFrame();
        im::DockSpaceOverViewport( 0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode );

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        if ( ImGui::Begin( "Main Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking ) )
        {
            float time_value = timestamp.total_seconds();
            im::SetNextItemWidth( -1.0f );
            if ( im::SliderFloat( "##Timestamp", &time_value, 0.0f, reader.duration_seconds() ) )
            {
                timestamp.load( time_value );
                reload_texture();
            }
            const kl::Int2 texture_size = gpu.texture_size( preview_texture );
            const ImVec2 view_size = im::GetContentRegionAvail();
            ImVec2 image_size{};
            if ( view_size.x / view_size.y > texture_size.x / (float) texture_size.y )
            {
                image_size = { view_size.y * ( texture_size.x / (float) texture_size.y ), view_size.y };
                im::SetCursorPosX( im::GetCursorPosX() + ( view_size.x - image_size.x ) * .5f );
            }
            else
            {
                image_size = { view_size.x, view_size.x * ( texture_size.y / (float) texture_size.x ) };
                im::SetCursorPosY( im::GetCursorPosY() + ( view_size.y - image_size.y ) * .5f );
            }
            im::Image( preview_view.get(), image_size );
        }
        ImGui::End();
        ImGui::PopStyleVar( 1 );

        im::Render();
        ImGui_ImplDX11_RenderDrawData( im::GetDrawData() );
        gpu.swap_buffers( true );
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    im::DestroyContext( imgui_context );
}

void mt::preview_crop( fs::path const& path, Timestamp timestamp, VideoCrop& crop )
{
    kl::VideoReader reader{ path.wstring() };
    kl::Window window{ kl::format( "Video Crop Preview ", path.filename().string(), " [", timestamp.minutes, ":", timestamp.seconds, "]" ) };
    kl::GPU gpu{ window.ptr() };

    auto* imgui_context = im::CreateContext();
    im::SetCurrentContext( imgui_context );

    auto& imio = im::GetIO();
    imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imio.IniFilename = nullptr;

    ImGui_ImplWin32_Init( window.ptr() );
    ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    im::StyleColorsDark();
    mt::MediaTool::load_theme();

    kl::dx::Texture preview_texture;
    kl::dx::ShaderView preview_view;
    const auto reload_texture = [&]
        {
            kl::Image frame;
            reader.seek( timestamp.total_seconds() );
            float frame_time = -std::numeric_limits<float>::infinity();
            do {
                float current_frame_time;
                reader.read_frame( frame, nullptr, &current_frame_time );
                if ( current_frame_time == frame_time )
                    break;
                frame_time = current_frame_time;
            }
            while ( frame_time < timestamp.total_seconds() );
            preview_texture = gpu.create_texture( frame );
            preview_view = gpu.create_shader_view( preview_texture, nullptr );
        };
    reload_texture();

    window.set_dark_mode( true );
    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
        } );
    const kl::Int2 texture_size = gpu.texture_size( preview_texture );
    window.resize( texture_size );
    window.set_position( kl::SCREEN_SIZE / 2 - window.size() / 2 );
    window.maximize();

    if ( HICON icon = LoadIcon( GetModuleHandleA( nullptr ), MAKEINTRESOURCE( IDI_ICON1 ) ) )
    {
        SendMessage( window.ptr(), WM_SETICON, ICON_BIG, (LPARAM) icon );
        SendMessage( window.ptr(), WM_SETICON, ICON_SMALL, (LPARAM) icon );
    }

    while ( window.process() )
    {
        gpu.clear_internal( kl::colors::GRAY );
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        im::NewFrame();
        ImGuizmo::BeginFrame();
        im::DockSpaceOverViewport( 0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode );

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        if ( ImGui::Begin( "Main Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking ) )
        {
            static constexpr auto convert_coords = []( kl::Int2 pos ) -> ImVec2
                {
                    return ImVec2{ (float) pos.x, (float) pos.y };
                };

            float time_value = timestamp.total_seconds();
            im::SetNextItemWidth( -1.0f );
            if ( im::SliderFloat( "##Timestamp", &time_value, 0.0f, reader.duration_seconds() ) )
            {
                timestamp.load( time_value );
                reload_texture();
            }
            const bool is_slider_active = im::IsItemActive();

            const kl::Int2 texture_size = gpu.texture_size( preview_texture );
            const ImVec2 view_size = im::GetContentRegionAvail();
            ImVec2 image_size{};
            if ( view_size.x / view_size.y > texture_size.x / (float) texture_size.y )
            {
                image_size = { view_size.y * ( texture_size.x / (float) texture_size.y ), view_size.y };
                im::SetCursorPosX( im::GetCursorPosX() + ( view_size.x - image_size.x ) * .5f );
            }
            else
            {
                image_size = { view_size.x, view_size.x * ( texture_size.y / (float) texture_size.x ) };
                im::SetCursorPosY( im::GetCursorPosY() + ( view_size.y - image_size.y ) * .5f );
            }
            const ImVec2 min_content = im::GetCursorPos();
            const ImVec2 content_ratio = image_size / convert_coords( texture_size );

            im::Image( preview_view.get(), image_size );

            const ImVec2 top_left = min_content + convert_coords( crop.top_left ) * content_ratio;
            const ImVec2 bottom_right = min_content + convert_coords( crop.bottom_right ) * content_ratio;
            static const ImU32 rect_color = ImColor( 255, 255, 255 );

            auto& draw_list = *im::GetWindowDrawList();
            draw_list.AddRect( top_left, bottom_right, rect_color );

            if ( im::IsWindowFocused() && !is_slider_active )
            {
                const ImVec2 mouse_pos = imgui_context->Style.WindowPadding - min_content + im::WindowPosAbsToRel( im::GetCurrentWindow(), im::GetMousePos() );
                const ImVec2 video_pos = mouse_pos / content_ratio;
                const kl::Int2 crop_pos = { int( video_pos.x ), int( video_pos.y ) };

                if ( im::IsMouseDown( ImGuiPopupFlags_MouseButtonLeft ) )
                    crop.top_left = crop_pos + kl::Int2{ 1 };
                else if ( im::IsMouseDown( ImGuiPopupFlags_MouseButtonRight ) )
                    crop.bottom_right = crop_pos + kl::Int2{ 1 };

                crop.top_left = kl::clamp( crop.top_left, kl::Int2{ 0 }, texture_size - kl::Int2{ 1 } );
                crop.bottom_right = kl::clamp( crop.bottom_right, crop.top_left + kl::Int2{ 1 }, texture_size );
            }
        }
        ImGui::End();
        ImGui::PopStyleVar( 1 );

        im::Render();
        ImGui_ImplDX11_RenderDrawData( im::GetDrawData() );
        gpu.swap_buffers( true );
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    im::DestroyContext( imgui_context );
}
