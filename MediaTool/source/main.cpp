#include "system_section.h"
#include "ytdlp_section.h"
#include "ffmpeg_section.h"
//#include "process_section.h"
//#include "concat_section.h"


//struct OptimizerData
//{
//    std::wstring input_file;
//    std::wstring output_file;
//    std::optional<Timestamp> start_time;
//    std::optional<Timestamp> end_time;
//    std::optional<kl::Int2> video_scale;
//    std::optional<GPUEncoder> gpu_encoder;
//    float video_max_size_mb = 10.0f;
//
//    void optimize() const;
//};
//
//struct ConcatData
//{
//    struct InputFile
//    {
//        std::wstring path;
//        std::optional<Timestamp> start_time;
//        std::optional<Timestamp> end_time;
//    };
//
//    std::vector<InputFile> inputs;
//    std::wstring output_file;
//    kl::Int2 video_scale = { 1920, 1080 };
//    int video_rate = 30;
//    int audio_rate = 48000;
//    std::optional<float> video_bitrate_m;
//    std::optional<GPUEncoder> gpu_encoder;
//
//    void concat() const;
//};

namespace mt
{
struct MediaTool
{
    kl::Window window{ "Media Tool" };
    kl::GPU gpu{ window.ptr() };
    ImGuiContext* imgui_context = nullptr;

    SystemSection system_section{};
    YTDLPSection ytdlp_section{};
    FFMPEGSection ffmpeg_section{ imgui_context };
    //OptimizerData optimizer_data;
    //ConcatData concat_data;

    MediaTool()
    {
        kl::console::set_title( "Media Tool" );

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
        window.resize( { 1200, int( kl::SCREEN_SIZE.y * 0.9 ) } );

        RECT console_rect{};
        GetWindowRect( GetConsoleWindow(), &console_rect );
        const kl::Int2 screen_work_size = kl::SCREEN_SIZE - kl::Int2{ 0, get_taskbar_height() };
        const kl::Int2 total_app_size = window.size();
        window.set_position( ( screen_work_size - total_app_size ) / 2 );
    }

    virtual ~MediaTool() noexcept
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        im::DestroyContext( imgui_context );
    }

    bool update()
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
            //static const kl::Float4 OPTIMIZER_COLOR = kl::RGB{ 227, 151, 93 };
            //static const kl::Float4 CONCAT_COLOR = kl::RGB{ 184, 108, 235 };

            system_section.display();
            im::Separator();
            ytdlp_section.display();
            im::Separator();
            ffmpeg_section.display();

            //im::Separator();
            //
            //{ // optimizer
            //    im::TextColored( (ImVec4 const&) OPTIMIZER_COLOR, "OPTIMIZER" );
            //
            //    im::Text( "Input File: %s", kl::convert_string( optimizer_data.input_file ).c_str() );
            //    im::SameLine();
            //    if ( im::Button( "Browse##OptimizerInputFileButton" ) )
            //    {
            //        if ( auto opt_path = kl::wchoose_file( false ) )
            //            optimizer_data.input_file = std::filesystem::absolute( *opt_path ).wstring();
            //    }
            //
            //    im::Text( "Output File: %s", kl::convert_string( optimizer_data.output_file ).c_str() );
            //    im::SameLine();
            //    if ( im::Button( "Browse##OptimizerOutputFileButton" ) )
            //    {
            //        if ( auto opt_path = kl::wchoose_file( true ) )
            //            optimizer_data.output_file = std::filesystem::absolute( *opt_path ).wstring();
            //    }
            //
            //    bool has_start_time = optimizer_data.start_time.has_value();
            //    if ( im::Checkbox( "Start Time##Optimizer", &has_start_time ) )
            //    {
            //        if ( has_start_time )
            //            optimizer_data.start_time.emplace();
            //        else
            //            optimizer_data.start_time.reset();
            //    }
            //    if ( optimizer_data.start_time )
            //    {
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragInt( "##OptimizerStartTimeMinutes", &optimizer_data.start_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragFloat( "##OptimizerStartTimeSeconds", &optimizer_data.start_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            //        if ( !optimizer_data.input_file.empty() )
            //        {
            //            im::SameLine();
            //            if ( im::Button( "Preview##OptimizerStartTime" ) )
            //            {
            //                preview_timestamp( optimizer_data.input_file, *optimizer_data.start_time );
            //                im::SetCurrentContext( imgui_context );
            //            }
            //        }
            //    }
            //
            //    bool has_end_time = optimizer_data.end_time.has_value();
            //    if ( im::Checkbox( "End Time##Optimizer", &has_end_time ) )
            //    {
            //        if ( has_end_time )
            //            optimizer_data.end_time.emplace();
            //        else
            //            optimizer_data.end_time.reset();
            //    }
            //    if ( optimizer_data.end_time )
            //    {
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragInt( "##OptimizerEndTimeMinutes", &optimizer_data.end_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragFloat( "##OptimizerEndTimeSeconds", &optimizer_data.end_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            //        if ( !optimizer_data.input_file.empty() )
            //        {
            //            im::SameLine();
            //            if ( im::Button( "Preview##OptimizerEndTime" ) )
            //            {
            //                preview_timestamp( optimizer_data.input_file, *optimizer_data.end_time );
            //                im::SetCurrentContext( imgui_context );
            //            }
            //        }
            //    }
            //
            //    bool has_video_scale = optimizer_data.video_scale.has_value();
            //    if ( im::Checkbox( "Video Scale##Optimizer", &has_video_scale ) )
            //    {
            //        if ( has_video_scale )
            //            optimizer_data.video_scale = DEFAULT_OPTIMIZER_SCALE;
            //        else
            //            optimizer_data.video_scale.reset();
            //    }
            //    if ( optimizer_data.video_scale )
            //    {
            //        im::SameLine();
            //        im::SetNextItemWidth( 200.0f );
            //        im::DragInt2( "##OptimizerVideoScale", &optimizer_data.video_scale->x, 1.0f, 1, 1'000'000 );
            //    }
            //
            //    bool has_gpu_encoder = optimizer_data.gpu_encoder.has_value();
            //    if ( im::Checkbox( kl::format( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")##Optimizer" ).c_str(), &has_gpu_encoder ) )
            //    {
            //        if ( has_gpu_encoder )
            //            optimizer_data.gpu_encoder.emplace();
            //        else
            //            optimizer_data.gpu_encoder.reset();
            //    }
            //    if ( has_gpu_encoder )
            //    {
            //        im::Text( "\t" );
            //        im::SameLine();
            //        optimizer_data.gpu_encoder->edit();
            //    }
            //
            //    im::SetNextItemWidth( 100.0f );
            //    im::DragFloat( "Max Video Size [MB]##Optimizer", &optimizer_data.video_max_size_mb, 0.01f, 0.0f, 1e6f );
            //
            //    if ( im::Button( "Optimize##Optimizer" ) )
            //    {
            //        if ( optimizer_data.input_file.empty() || optimizer_data.output_file.empty() )
            //            kl::print( kl::colors::RED, "OPTIMIZER: Input and output files must be specified.", kl::colors::CONSOLE );
            //        else
            //            optimizer_data.optimize();
            //    }
            //}
            //
            //im::Separator();
            //
            //{ // concat
            //    im::TextColored( (ImVec4 const&) CONCAT_COLOR, "CONCAT" );
            //
            //    static constexpr size_t min_size = 2;
            //
            //    if ( concat_data.inputs.size() < min_size )
            //        concat_data.inputs.resize( min_size );
            //
            //    std::optional<int> to_remove;
            //    const bool is_min_size = ( concat_data.inputs.size() == min_size );
            //    for ( int i = 0; i < (int) concat_data.inputs.size(); i++ )
            //    {
            //        auto& input = concat_data.inputs[i];
            //
            //        im::Text( "Input File %d: %s", i + 1, kl::convert_string( input.path ).c_str() );
            //        im::SameLine();
            //        if ( im::Button( kl::format( "Browse##ConcatInput", i ).c_str() ) )
            //        {
            //            if ( auto opt_path = kl::wchoose_file( false ) )
            //                input.path = std::filesystem::absolute( *opt_path ).wstring();
            //        }
            //        im::SameLine();
            //        if ( im::Button( kl::format( is_min_size ? "Erase##ConcatInput" : "Remove##ConcatInput", i ).c_str() ) )
            //        {
            //            to_remove = i;
            //        }
            //        if ( i > 0 )
            //        {
            //            im::SameLine();
            //            if ( im::Button( kl::format( "Move Up##ConcatInput", i ).c_str() ) )
            //                std::swap( concat_data.inputs[i], concat_data.inputs[(size_t) i - 1] );
            //        }
            //        if ( i < (int) concat_data.inputs.size() - 1 )
            //        {
            //            im::SameLine();
            //            if ( im::Button( kl::format( "Move Down##ConcatInput", i ).c_str() ) )
            //                std::swap( concat_data.inputs[i], concat_data.inputs[(size_t) i + 1] );
            //        }
            //
            //        bool has_start_time = input.start_time.has_value();
            //        if ( im::Checkbox( kl::format( "Start Time##Concat", i ).c_str(), &has_start_time ) )
            //        {
            //            if ( has_start_time )
            //                input.start_time.emplace();
            //            else
            //                input.start_time.reset();
            //        }
            //        if ( input.start_time )
            //        {
            //            im::SameLine();
            //            im::SetNextItemWidth( 100.0f );
            //            im::DragInt( kl::format( "##ConcatStartTimeMinutes", i ).c_str(), &input.start_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //            im::SameLine();
            //            im::SetNextItemWidth( 100.0f );
            //            im::DragFloat( kl::format( "##ConcatStartTimeSeconds", i ).c_str(), &input.start_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            //            if ( !input.path.empty() )
            //            {
            //                im::SameLine();
            //                if ( im::Button( kl::format( "Preview##ConcatStartTime", i ).c_str() ) )
            //                {
            //                    preview_timestamp( input.path, *input.start_time );
            //                    im::SetCurrentContext( imgui_context );
            //                }
            //            }
            //        }
            //
            //        bool has_end_time = input.end_time.has_value();
            //        if ( im::Checkbox( kl::format( "End Time##Concat", i ).c_str(), &has_end_time ) )
            //        {
            //            if ( has_end_time )
            //                input.end_time.emplace();
            //            else
            //                input.end_time.reset();
            //        }
            //        if ( input.end_time )
            //        {
            //            im::SameLine();
            //            im::SetNextItemWidth( 100.0f );
            //            im::DragInt( kl::format( "##ConcatEndTimeMinutes", i ).c_str(), &input.end_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //            im::SameLine();
            //            im::SetNextItemWidth( 100.0f );
            //            im::DragFloat( kl::format( "##ConcatEndTimeSeconds", i ).c_str(), &input.end_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            //            if ( !input.path.empty() )
            //            {
            //                im::SameLine();
            //                if ( im::Button( kl::format( "Preview##ConcatEndTime", i ).c_str() ) )
            //                {
            //                    preview_timestamp( input.path, *input.end_time );
            //                    im::SetCurrentContext( imgui_context );
            //                }
            //            }
            //        }
            //    }
            //    if ( im::Button( "Add Input##Concat" ) )
            //        concat_data.inputs.emplace_back();
            //    if ( to_remove )
            //    {
            //        if ( is_min_size )
            //            concat_data.inputs[*to_remove] = {};
            //        else
            //            concat_data.inputs.erase( concat_data.inputs.begin() + *to_remove );
            //    }
            //
            //    im::Text( "Output File: %s", kl::convert_string( concat_data.output_file ).c_str() );
            //    im::SameLine();
            //    if ( im::Button( "Browse##ConcatOutput" ) )
            //    {
            //        if ( auto opt_path = kl::wchoose_file( true ) )
            //            concat_data.output_file = std::filesystem::absolute( *opt_path ).wstring();
            //    }
            //
            //    im::SetNextItemWidth( 125.0f );
            //    im::DragInt2( "Video Scale##Concat", &concat_data.video_scale.x, 1.0f, 1, 1'000'000 );
            //
            //    im::SetNextItemWidth( 100.0f );
            //    im::DragInt( "Video Rate##Concat", &concat_data.video_rate, 1.0f, 1, 1'000'000 );
            //
            //    im::SetNextItemWidth( 100.0f );
            //    im::DragInt( "Audio Rate##Concat", &concat_data.audio_rate, 1.0f, 1, 1'000'000 );
            //
            //    bool has_video_bitrate_m = concat_data.video_bitrate_m.has_value();
            //    if ( im::Checkbox( "Video Bitrate [Mb]##Concat", &has_video_bitrate_m ) )
            //    {
            //        if ( has_video_bitrate_m )
            //            concat_data.video_bitrate_m = DEFAULT_BITRATE_M;
            //        else if ( !concat_data.gpu_encoder.has_value() )
            //            concat_data.video_bitrate_m.reset();
            //    }
            //    if ( concat_data.video_bitrate_m )
            //    {
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragFloat( "##ConcatVideoBitrate", &*concat_data.video_bitrate_m, 0.01f, 0.0f, 1e6f );
            //    }
            //
            //    bool has_gpu_encoder = concat_data.gpu_encoder.has_value();
            //    if ( im::Checkbox( kl::format( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")##Concat" ).c_str(), &has_gpu_encoder ) )
            //    {
            //        if ( has_gpu_encoder )
            //            concat_data.gpu_encoder.emplace();
            //        else
            //            concat_data.gpu_encoder.reset();
            //    }
            //    if ( has_gpu_encoder )
            //    {
            //        if ( !concat_data.video_bitrate_m )
            //            concat_data.video_bitrate_m = DEFAULT_BITRATE_M;
            //        im::Text( "\t" );
            //        im::SameLine();
            //        concat_data.gpu_encoder->edit();
            //    }
            //
            //    if ( im::Button( "Concat##Concat" ) )
            //        concat_data.concat();
            //}
        }
        ImGui::End();
        ImGui::PopStyleVar( 1 );

        im::Render();
        ImGui_ImplDX11_RenderDrawData( im::GetDrawData() );
        gpu.swap_buffers( true );

        return window.process();
    }

private:
    void load_theme()
    {
        kl::Float4 special_color = kl::colors::WHITE;
        kl::Float4 alternate_color = kl::colors::BLACK;
        ImGuiStyle& style = im::GetStyle();

        style.Colors[ImGuiCol_Text] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
        style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );

        style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
        style.Colors[ImGuiCol_ChildBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );
        style.Colors[ImGuiCol_PopupBg] = ImVec4( 0.14f, 0.14f, 0.14f, 1.00f );

        style.Colors[ImGuiCol_Border] = ImVec4( 0.45f, 0.45f, 0.45f, 0.50f );
        style.Colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );

        style.Colors[ImGuiCol_FrameBg] = ImVec4( 0.15f, 0.15f, 0.15f, 1.00f );
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

    int get_taskbar_height()
    {
        RECT screen_rect{};
        RECT work_area_rect{};
        SystemParametersInfo( SPI_GETWORKAREA, 0, &work_area_rect, 0 );
        GetWindowRect( GetDesktopWindow(), &screen_rect );
        const int screen_height = screen_rect.bottom - screen_rect.top;
        const int work_height = work_area_rect.bottom - work_area_rect.top;
        return ( work_height < screen_height ) ? screen_height - work_height : 0;
    }
};
}

int wmain( int argc, wchar_t** argv )
{
    mt::MediaTool media_tool{};
    while ( media_tool.update() );
    return 0;
}

/*
void OptimizerData::optimize() const
{
    static constexpr float COPY_TIME_PRECUT = 15.0f;
    static constexpr float MIN_FILESIZE_PERC = 0.95f;
    static constexpr float AGGRESSIVE_BIAS = 0.98f;
    static constexpr float DEFAULT_BITRATE_M = 5.0f;

    const float start_time = this->start_time.value_or( Timestamp{} ).total_seconds();
    const std::wstring output_dir = std::filesystem::path( this->output_file ).parent_path().wstring();

    const float copy_start_time = kl::max( start_time - COPY_TIME_PRECUT, 0.0f );
    const std::wstring output_copy_file = kl::wformat( output_dir, "/__", kl::convert_string( kl::random::gen_string( 10 ) ), "_.mp4" );

    FFMPEGData copy_ffmpeg_data;
    copy_ffmpeg_data.input_file = input_file;
    copy_ffmpeg_data.output_file = output_copy_file;
    copy_ffmpeg_data.start_time.emplace().seconds = copy_start_time;
    copy_ffmpeg_data.end_time = end_time;
    copy_ffmpeg_data.codec.emplace<CopyCodec>();
    if ( !execute( copy_ffmpeg_data.produce() ) )
        return;

    FFMPEGData encode_ffmpeg_data;
    encode_ffmpeg_data.input_file = output_copy_file;
    encode_ffmpeg_data.output_file = output_file;
    encode_ffmpeg_data.start_time.emplace().seconds = start_time - copy_start_time;
    auto& codec = encode_ffmpeg_data.codec.emplace<CustomCodec>();
    if ( video_scale )
        codec.video_viewport.emplace<VideoScale>().scale = *video_scale;
    codec.video_bitrate_m = DEFAULT_BITRATE_M;
    codec.gpu_encoder = gpu_encoder;
    while ( true )
    {
        if ( !execute( encode_ffmpeg_data.produce() ) )
            return;

        const float file_size_mb = ( std::filesystem::file_size( output_file ) / 1024.0f ) / 1024.0f;
        kl::print( std::fixed, kl::Float2{ video_max_size_mb * MIN_FILESIZE_PERC, video_max_size_mb }, " -> ", file_size_mb );
        if ( file_size_mb > video_max_size_mb || file_size_mb < ( video_max_size_mb * MIN_FILESIZE_PERC ) )
        {
            *codec.video_bitrate_m *= ( video_max_size_mb / file_size_mb ) * AGGRESSIVE_BIAS;
            continue;
        }
        else
            break;
    }

    std::filesystem::remove( output_copy_file );
}

void ConcatData::concat() const
{
    if ( inputs.empty() )
    {
        kl::print( kl::colors::RED, "Concat inputs can't be empty.", kl::colors::CONSOLE );
        return;
    }
    for ( int i = 0; i < (int) inputs.size(); i++ )
    {
        auto const& input = inputs[i];
        if ( input.path.empty() )
        {
            kl::print( kl::colors::RED, "Concat input ", i + 1, " can't be empty.", kl::colors::CONSOLE );
            return;
        }
    }
    if ( output_file.empty() )
    {
        kl::print( kl::colors::RED, "Concat output file can't be empty.", kl::colors::CONSOLE );
        return;
    }

    const std::wstring rand_str = kl::convert_string( kl::random::gen_string( 10 ) );
    const std::wstring list_file_name = kl::wformat( "concat_vids_list__", rand_str, ".txt" );
    std::vector<std::wstring> new_input_files;
    {
        FFMPEGData ffmpeg;
        ffmpeg.other_commands = kl::wformat(
            "-r ", video_rate, "\n",
            "-ar ", audio_rate, "\n"
        );

        auto& codec = ffmpeg.codec.emplace<CustomCodec>();
        codec.video_viewport.emplace<VideoScale>().scale = video_scale;
        codec.video_bitrate_m = video_bitrate_m;
        codec.gpu_encoder = gpu_encoder;

        std::wofstream list_file{ list_file_name };
        const std::wstring output_file_extension = std::filesystem::path( this->output_file ).extension().wstring();
        for ( int i = 0; i < (int) inputs.size(); i++ )
        {
            auto const& input = inputs[i];
            ffmpeg.input_file = input.path;
            ffmpeg.start_time = input.start_time;
            ffmpeg.end_time = input.end_time;
            ffmpeg.output_file = kl::wformat( "concat_input_", i + 1, "__", rand_str, output_file_extension );
            if ( !execute( ffmpeg.produce() ) )
                return;
            new_input_files.emplace_back( ffmpeg.output_file );
            list_file << "file '" << ffmpeg.output_file << "'" << std::endl;
        }
    }

    const std::wstring command = kl::wformat( "ffmpeg -y -f concat -i \"", list_file_name, "\" -c copy \"", output_file, "\"" );
    execute( command );

    for ( auto& new_input_file : new_input_files )
        std::filesystem::remove( new_input_file );
    std::filesystem::remove( list_file_name );
}
*/
