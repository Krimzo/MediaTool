#include "preview_tools.h"


void mt::preview_timestamp( std::wstring_view const& path, Timestamp& timestamp )
{
    //const std::filesystem::path p{ path };
    //const std::wstring preview_name = kl::wformat( "__vidhandle_preview_", kl::convert_string( kl::random::gen_string( 10 ) ), "_.png" );
    //std::wstring preview_path = kl::wformat( p.parent_path().wstring(), "/", preview_name );
    //preview_path = std::filesystem::absolute( preview_path ).wstring();
    //
    //kl::Window window{ kl::format( "Video Handler Timestamp Preview ", p.filename(), " [", timestamp.minutes, ":", timestamp.seconds, "]" ) };
    //kl::GPU gpu{ window.ptr() };
    //
    //FFMPEGData ffmpeg_data;
    //ffmpeg_data.input_file = path;
    //ffmpeg_data.output_file = preview_path;
    //ffmpeg_data.start_time.emplace( timestamp );
    //ffmpeg_data.codec.emplace<CustomCodec>();
    //ffmpeg_data.other_commands = L"-frames:v 1";
    //execute( ffmpeg_data.produce() );
    //
    //kl::Image preview_image{ preview_path };
    //if ( preview_image.width() == 0 || preview_image.height() == 0 )
    //    return;
    //
    //kl::dx::Texture preview_texture = gpu.create_texture( preview_image );
    //kl::dx::ShaderView preview_view = gpu.create_shader_view( preview_texture, nullptr );
    //std::filesystem::remove( preview_path );
    //
    //auto* imgui_context = im::CreateContext();
    //im::SetCurrentContext( imgui_context );
    //
    //auto& imio = im::GetIO();
    //imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //imio.IniFilename = nullptr;
    //
    //ImGui_ImplWin32_Init( window.ptr() );
    //ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    //im::StyleColorsDark();
    //load_theme( 1.35f );
    //
    //window.set_dark_mode( true );
    //window.on_resize.emplace_back( [&]( kl::Int2 size )
    //    {
    //        gpu.resize_internal( size );
    //        gpu.set_viewport_size( size );
    //    } );
    //window.resize( gpu.texture_size( preview_texture ) );
    //window.set_position( kl::SCREEN_SIZE / 2 - window.size() / 2 );
    //
    //while ( window.process() )
    //{
    //    gpu.clear_internal( kl::colors::GRAY );
    //    ImGui_ImplWin32_NewFrame();
    //    ImGui_ImplDX11_NewFrame();
    //    im::NewFrame();
    //    ImGuizmo::BeginFrame();
    //    im::DockSpaceOverViewport( 0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode );
    //
    //    ImGuiViewport* viewport = ImGui::GetMainViewport();
    //    ImGui::SetNextWindowPos( viewport->WorkPos );
    //    ImGui::SetNextWindowSize( viewport->WorkSize );
    //    ImGui::SetNextWindowViewport( viewport->ID );
    //
    //    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    //    if ( ImGui::Begin( "Main Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking ) )
    //    {
    //        ImVec2 view_size = im::GetContentRegionAvail();
    //        im::Image( preview_view.get(), view_size );
    //    }
    //    ImGui::End();
    //    ImGui::PopStyleVar( 1 );
    //
    //    im::Render();
    //    ImGui_ImplDX11_RenderDrawData( im::GetDrawData() );
    //    gpu.swap_buffers( true );
    //}
    //
    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //im::DestroyContext( imgui_context );
}

void mt::preview_crop( std::wstring_view const& path, Timestamp timestamp, VideoCrop& crop )
{
    //const std::filesystem::path p{ path };
    //const std::wstring preview_name = kl::wformat( "__vidhandle_preview_", kl::convert_string( kl::random::gen_string( 10 ) ), "_.png" );
    //std::wstring preview_path = kl::wformat( p.parent_path().wstring(), "/", preview_name );
    //preview_path = std::filesystem::absolute( preview_path ).wstring();
    //
    //kl::Window window{ kl::format( "Video Handler Crop Preview ", p.filename(), " [", timestamp.minutes, ":", timestamp.seconds, "]" ) };
    //kl::GPU gpu{ window.ptr() };
    //
    //FFMPEGData ffmpeg_data;
    //ffmpeg_data.input_file = path;
    //ffmpeg_data.output_file = preview_path;
    //ffmpeg_data.start_time.emplace( timestamp );
    //ffmpeg_data.codec.emplace<CustomCodec>();
    //ffmpeg_data.other_commands = L"-frames:v 1";
    //execute( ffmpeg_data.produce() );
    //
    //kl::Image preview_image{ preview_path };
    //if ( preview_image.width() == 0 || preview_image.height() == 0 )
    //    return;
    //
    //kl::dx::Texture preview_texture = gpu.create_texture( preview_image );
    //kl::dx::ShaderView preview_view = gpu.create_shader_view( preview_texture, nullptr );
    //std::filesystem::remove( preview_path );
    //
    //auto* imgui_context = im::CreateContext();
    //im::SetCurrentContext( imgui_context );
    //
    //auto& imio = im::GetIO();
    //imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //imio.IniFilename = nullptr;
    //
    //ImGui_ImplWin32_Init( window.ptr() );
    //ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    //im::StyleColorsDark();
    //load_theme( 1.35f );
    //
    //window.set_dark_mode( true );
    //window.on_resize.emplace_back( [&]( kl::Int2 size )
    //    {
    //        gpu.resize_internal( size );
    //        gpu.set_viewport_size( size );
    //    } );
    //
    //const kl::Int2 texture_size = gpu.texture_size( preview_texture );
    //window.resize( texture_size );
    //window.set_position( kl::SCREEN_SIZE / 2 - window.size() / 2 );
    //
    //while ( window.process() )
    //{
    //    gpu.clear_internal( kl::colors::GRAY );
    //    ImGui_ImplWin32_NewFrame();
    //    ImGui_ImplDX11_NewFrame();
    //    im::NewFrame();
    //    ImGuizmo::BeginFrame();
    //    im::DockSpaceOverViewport( 0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode );
    //
    //    ImGuiViewport* viewport = ImGui::GetMainViewport();
    //    ImGui::SetNextWindowPos( viewport->WorkPos );
    //    ImGui::SetNextWindowSize( viewport->WorkSize );
    //    ImGui::SetNextWindowViewport( viewport->ID );
    //
    //    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    //    if ( ImGui::Begin( "Main Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking ) )
    //    {
    //        static constexpr auto convert_coords = []( kl::Int2 pos )
    //            {
    //                return ImVec2{ (float) pos.x, (float) pos.y };
    //            };
    //
    //        const ImVec2 min_content = im::GetWindowContentRegionMin();
    //        const ImVec2 max_content = im::GetWindowContentRegionMax();
    //        const ImVec2 content_region = max_content - min_content;
    //        const ImVec2 content_ratio = content_region / convert_coords( texture_size );
    //
    //        im::Image( preview_view.get(), content_region );
    //
    //        const ImVec2 top_left = min_content + convert_coords( crop.position ) * content_ratio;
    //        const ImVec2 bottom_right = min_content + convert_coords( crop.position + crop.size ) * content_ratio;
    //        static const ImU32 rect_color = ImColor( 255, 255, 255 );
    //
    //        auto& draw_list = *im::GetWindowDrawList();
    //        draw_list.AddRect( top_left, bottom_right, rect_color );
    //
    //        if ( im::IsWindowFocused() )
    //        {
    //            const ImVec2 mouse_pos = im::WindowPosAbsToRel( im::GetCurrentWindow(), im::GetMousePos() );
    //            const ImVec2 video_pos = mouse_pos / content_ratio;
    //            const kl::Int2 crop_pos = { int( video_pos.x ), int( video_pos.y ) };
    //
    //            if ( im::IsMouseDown( ImGuiPopupFlags_MouseButtonLeft ) )
    //                crop.position = crop_pos;
    //            else if ( im::IsMouseDown( ImGuiPopupFlags_MouseButtonRight ) )
    //                crop.size = crop_pos - crop.position;
    //
    //            crop.position = kl::clamp( crop.position, kl::Int2{ 0 }, texture_size - kl::Int2{ 1 } );
    //            crop.size = kl::clamp( crop.size, kl::Int2{ 1 }, texture_size - crop.position );
    //        }
    //    }
    //    ImGui::End();
    //    ImGui::PopStyleVar( 1 );
    //
    //    im::Render();
    //    ImGui_ImplDX11_RenderDrawData( im::GetDrawData() );
    //    gpu.swap_buffers( true );
    //}
    //
    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //im::DestroyContext( imgui_context );
}
