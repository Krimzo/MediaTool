#include "media_tool.h"
#include "../resource.h"

mt::MediaTool::MediaTool()
{
    kl::console::set_enabled( false );
    imgui_context = im::CreateContext();
    im::SetCurrentContext( imgui_context );

    auto& imio = im::GetIO();
    imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imio.IniFilename = nullptr;

    ImGui_ImplWin32_Init( window.ptr() );
    ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    im::StyleColorsDark();
    load_theme();

    window.set_dark_mode( true );
    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
        } );
    window.resize( { 700, 700 } );

    const kl::Int2 screen_work_size = kl::SCREEN_SIZE - kl::Int2{ 0, get_taskbar_height() };
    const kl::Int2 total_app_size = window.size();
    window.set_position( ( screen_work_size - total_app_size ) / 2 );

    if ( HICON icon = LoadIcon( GetModuleHandleA( nullptr ), MAKEINTRESOURCE( IDI_ICON1 ) ) )
    {
        SendMessage( window.ptr(), WM_SETICON, ICON_BIG, (LPARAM) icon );
        SendMessage( window.ptr(), WM_SETICON, ICON_SMALL, (LPARAM) icon );
    }
}

mt::MediaTool::~MediaTool()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    im::DestroyContext( imgui_context );
    kl::console::set_enabled( true );
}

bool mt::MediaTool::update()
{
    gpu.clear_internal( kl::colors::GRAY );
    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX11_NewFrame();
    im::NewFrame();
    ImGuizmo::BeginFrame();
    im::DockSpaceOverViewport( 0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode );

    ImGuiViewport* viewport = im::GetMainViewport();
    im::SetNextWindowPos( viewport->WorkPos );
    im::SetNextWindowSize( viewport->WorkSize );
    im::SetNextWindowViewport( viewport->ID );

    im::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    im::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2{} );
    if ( im::Begin( QNAME( "Main Window" ), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking ) )
    {
        im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
        im::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{} );
        const ImVec2 button_size = { im::GetContentRegionAvail().x / 5, 25 };
        if ( tab_button( current_section.is<SystemSection>(), "System", button_size, SystemSection::COLOR ) )
            current_section = system_section;
        im::SameLine();
        if ( tab_button( current_section.is<YTDLPSection>(), "YT-DLP", button_size, YTDLPSection::COLOR ) )
            current_section = ytdlp_section;
        im::SameLine();
        if ( tab_button( current_section.is<FFMPEGSection>(), "FFMPEG", button_size, FFMPEGSection::COLOR ) )
            current_section = ffmpeg_section;
        im::SameLine();
        if ( tab_button( current_section.is<OptimizerSection>(), "OPTIMIZER", button_size, OptimizerSection::COLOR ) )
            current_section = optimizer_section;
        im::SameLine();
        if ( tab_button( current_section.is<ConcatSection>(), "CONCAT", button_size, ConcatSection::COLOR ) )
            current_section = concat_section;
        im::PopStyleVar( 2 );
        im::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2{ 8, 8 } );
        if ( im::BeginChild( "Section Child", {}, ImGuiChildFlags_AlwaysUseWindowPadding ) )
            current_section->display();
        im::EndChild();
        im::PopStyleVar( 1 );
    }
    im::End();
    im::PopStyleVar( 2 );

    im::Render();
    ImGui_ImplDX11_RenderDrawData( im::GetDrawData() );
    gpu.swap_buffers( true );

    return window.process();
}

void mt::MediaTool::load_theme()
{
    const kl::Float4 special_color = kl::colors::WHITE;
    const kl::Float4 alternate_color = kl::colors::BLACK;
    ImGuiStyle& style = im::GetStyle();

    style.Colors[ImGuiCol_Text] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
    style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );

    style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
    style.Colors[ImGuiCol_ChildBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
    style.Colors[ImGuiCol_PopupBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );

    style.Colors[ImGuiCol_Border] = ImVec4( 0.45f, 0.45f, 0.45f, 0.50f );
    style.Colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );

    style.Colors[ImGuiCol_FrameBg] = style.Colors[ImGuiCol_ChildBg];
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.30f, 0.30f, 0.30f, 1.00f );
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.60f, 0.60f, 0.60f, 0.40f );

    style.Colors[ImGuiCol_TitleBg] = ImVec4( 0.08f, 0.08f, 0.08f, 1.00f );
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.08f, 0.08f, 0.08f, 1.00f );
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f, 0.00f, 0.00f, 0.51f );

    style.Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );

    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.02f, 0.02f, 0.02f, 0.53f );
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.31f, 0.31f, 0.31f, 1.00f );
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.51f, 0.51f, 0.51f, 1.00f );

    style.Colors[ImGuiCol_CheckMark] = (ImVec4&) special_color;

    style.Colors[ImGuiCol_SliderGrab] = (ImVec4&) special_color;
    style.Colors[ImGuiCol_SliderGrabActive] = (ImVec4&) special_color;

    style.Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_FrameBg];
    style.Colors[ImGuiCol_ButtonHovered] = style.Colors[ImGuiCol_FrameBgHovered];
    style.Colors[ImGuiCol_ButtonActive] = style.Colors[ImGuiCol_FrameBgActive];

    style.Colors[ImGuiCol_Header] = ImVec4( 0.22f, 0.22f, 0.22f, 1.00f );
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.25f, 0.25f, 0.25f, 1.00f );
    style.Colors[ImGuiCol_HeaderActive] = ImVec4( 0.67f, 0.67f, 0.67f, 0.39f );

    style.Colors[ImGuiCol_Separator] = ImVec4( 0.45f, 0.45f, 0.45f, 0.50f );
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.44f, 0.44f, 0.44f, 1.00f );
    style.Colors[ImGuiCol_SeparatorActive] = (ImVec4&) special_color;

    style.Colors[ImGuiCol_ResizeGrip] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.30f, 0.30f, 0.30f, 0.67f );
    style.Colors[ImGuiCol_ResizeGripActive] = (ImVec4&) special_color;

    style.Colors[ImGuiCol_Tab] = ImVec4( 0.08f, 0.08f, 0.08f, 0.83f );
    style.Colors[ImGuiCol_TabHovered] = ImVec4( 0.35f, 0.35f, 0.35f, 0.83f );
    style.Colors[ImGuiCol_TabActive] = ImVec4( 0.23f, 0.23f, 0.23f, 1.00f );
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4( 0.08f, 0.08f, 0.08f, 1.00f );
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
    style.Colors[ImGuiCol_TabSelectedOverline] = (ImVec4&) special_color;

    style.Colors[ImGuiCol_DockingPreview] = (ImVec4&) special_color;
    style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );

    style.Colors[ImGuiCol_PlotLines] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );

    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.42f, 0.69f, 0.69f, 0.32f );

    style.Colors[ImGuiCol_DragDropTarget] = (ImVec4&) special_color;

    style.Colors[ImGuiCol_NavHighlight] = (ImVec4&) special_color;
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );

    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );

    style.WindowPadding = ImVec2( 8.00f, 8.00f );
    style.FramePadding = ImVec2( 5.00f, 2.00f );
    style.CellPadding = ImVec2( 6.00f, 6.00f );
    style.ItemSpacing = ImVec2( 6.00f, 6.00f );
    style.ItemInnerSpacing = ImVec2( 6.00f, 6.00f );
    style.TouchExtraPadding = ImVec2( 0.00f, 0.00f );
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
}

int mt::MediaTool::get_taskbar_height()
{
    RECT screen_rect{};
    RECT work_area_rect{};
    SystemParametersInfo( SPI_GETWORKAREA, 0, &work_area_rect, 0 );
    GetWindowRect( GetDesktopWindow(), &screen_rect );
    const int screen_height = screen_rect.bottom - screen_rect.top;
    const int work_height = work_area_rect.bottom - work_area_rect.top;
    return ( work_height < screen_height ) ? screen_height - work_height : 0;
}

bool mt::tab_button( bool is_pressed, char const* label, ImVec2 const& size_arg, kl::Float4 const& color )
{
    if ( is_pressed )
    {
        im::PushItemFlag( ImGuiItemFlags_Disabled, true );
        im::PushStyleColor( ImGuiCol_Text, reinterpret_cast<ImVec4 const&>( color ) );
        im::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.0f );
    }
    const bool result = im::Button( label, size_arg );
    if ( is_pressed )
    {
        im::PopStyleVar( 1 );
        im::PopStyleColor( 1 );
        im::PopItemFlag();
    }
    return result;
}
