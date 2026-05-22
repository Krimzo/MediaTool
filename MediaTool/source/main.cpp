#include "system_section.h"
#include "ytdlp_section.h"
//#include "ffmpeg_section.h"
//#include "process_section.h"
//#include "concat_section.h"

//struct CopyCodec
//{
//    std::wstring produce() const;
//};
//
//struct CustomCodec
//{
//    std::variant<Default, VideoScale, VideoCrop> video_viewport;
//    std::optional<float> video_bitrate_m;
//    std::optional<GPUEncoder> gpu_encoder;
//
//    std::wstring produce() const;
//};
//
//struct FFMPEGData
//{
//    std::wstring input_file;
//    std::wstring output_file;
//    std::optional<Timestamp> start_time;
//    std::optional<Timestamp> end_time;
//    std::variant<Default, CopyCodec, CustomCodec> codec;
//    std::wstring other_commands;
//
//    std::wstring produce() const;
//};
//
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
//
//void preview_timestamp( std::wstring_view const& path, Timestamp timestamp );
//void preview_crop( std::wstring_view const& path, Timestamp timestamp, VideoCrop& crop );
//
//static constexpr float DEFAULT_BITRATE_M = 5.0f;
//static constexpr kl::Int2 DEFAULT_FFMPEG_SCALE = { 1920, 1080 };
//static constexpr kl::Int2 DEFAULT_OPTIMIZER_SCALE = { 1280, 720 };

namespace mt
{
struct MediaTool
{
    kl::Window window{ "Media Tool" };
    kl::GPU gpu{ window.ptr() };
    ImGuiContext* imgui_context = nullptr;

    SystemSection system_section{};
    YTDLPSection ytdlp_section{};
    //FFMPEGData ffmpeg_data;
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
            //static const kl::Float4 FFMPEG_COLOR = kl::RGB{ 118, 219, 192 };
            //static const kl::Float4 OPTIMIZER_COLOR = kl::RGB{ 227, 151, 93 };
            //static const kl::Float4 CONCAT_COLOR = kl::RGB{ 184, 108, 235 };

            system_section.display();
            im::Separator();
            ytdlp_section.display();
            im::Separator();

            //{ // ffmpeg
            //    im::TextColored( (ImVec4 const&) FFMPEG_COLOR, "FFMPEG" );
            //
            //    im::Text( "Input File: %s", kl::convert_string( ffmpeg_data.input_file ).c_str() );
            //    im::SameLine();
            //    if ( im::Button( "Browse##FFMPEGInputFileButton" ) )
            //    {
            //        if ( auto opt_path = kl::wchoose_file( false ) )
            //            ffmpeg_data.input_file = std::filesystem::absolute( *opt_path ).wstring();
            //    }
            //
            //    im::Text( "Output File: %s", kl::convert_string( ffmpeg_data.output_file ).c_str() );
            //    im::SameLine();
            //    if ( im::Button( "Browse##FFMPEGOutputFileButton" ) )
            //    {
            //        if ( auto opt_path = kl::wchoose_file( true ) )
            //            ffmpeg_data.output_file = std::filesystem::absolute( *opt_path ).wstring();
            //    }
            //
            //    bool has_start_time = ffmpeg_data.start_time.has_value();
            //    if ( im::Checkbox( "Start Time##FFMPEG", &has_start_time ) )
            //    {
            //        if ( has_start_time )
            //            ffmpeg_data.start_time.emplace();
            //        else
            //            ffmpeg_data.start_time.reset();
            //    }
            //    if ( ffmpeg_data.start_time )
            //    {
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragInt( "##FFMPEGStartTimeMinutes", &ffmpeg_data.start_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragFloat( "##FFMPEGStartTimeSeconds", &ffmpeg_data.start_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            //        if ( !ffmpeg_data.input_file.empty() )
            //        {
            //            im::SameLine();
            //            if ( im::Button( "Preview##FFMPEGStartTime" ) )
            //            {
            //                preview_timestamp( ffmpeg_data.input_file, *ffmpeg_data.start_time );
            //                im::SetCurrentContext( imgui_context );
            //            }
            //        }
            //    }
            //
            //    bool has_end_time = ffmpeg_data.end_time.has_value();
            //    if ( im::Checkbox( "End Time##FFMPEG", &has_end_time ) )
            //    {
            //        if ( has_end_time )
            //            ffmpeg_data.end_time.emplace();
            //        else
            //            ffmpeg_data.end_time.reset();
            //    }
            //    if ( ffmpeg_data.end_time )
            //    {
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragInt( "##FFMPEGEndTimeMinutes", &ffmpeg_data.end_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //        im::SameLine();
            //        im::SetNextItemWidth( 100.0f );
            //        im::DragFloat( "##FFMPEGEndTimeSeconds", &ffmpeg_data.end_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            //        if ( !ffmpeg_data.input_file.empty() )
            //        {
            //            im::SameLine();
            //            if ( im::Button( "Preview##FFMPEGEndTime" ) )
            //            {
            //                preview_timestamp( ffmpeg_data.input_file, *ffmpeg_data.end_time );
            //                im::SetCurrentContext( imgui_context );
            //            }
            //        }
            //    }
            //
            //    bool has_default_codec = std::holds_alternative<Default>( ffmpeg_data.codec );
            //    if ( im::Checkbox( "Default Codec##FFMPEG", &has_default_codec ) )
            //    {
            //        if ( !std::holds_alternative<Default>( ffmpeg_data.codec ) )
            //            ffmpeg_data.codec.emplace<Default>();
            //    }
            //
            //    im::SameLine();
            //
            //    bool has_copy_codec = std::holds_alternative<CopyCodec>( ffmpeg_data.codec );
            //    if ( im::Checkbox( "Copy Codec##FFMPEG", &has_copy_codec ) )
            //    {
            //        if ( !std::holds_alternative<CopyCodec>( ffmpeg_data.codec ) )
            //            ffmpeg_data.codec.emplace<CopyCodec>();
            //    }
            //
            //    im::SameLine();
            //
            //    bool has_custom_codec = std::holds_alternative<CustomCodec>( ffmpeg_data.codec );
            //    if ( im::Checkbox( "Custom Codec##FFMPEG", &has_custom_codec ) )
            //    {
            //        if ( !std::holds_alternative<CustomCodec>( ffmpeg_data.codec ) )
            //            ffmpeg_data.codec.emplace<CustomCodec>();
            //    }
            //
            //    if ( has_custom_codec )
            //    {
            //        auto& codec = std::get<CustomCodec>( ffmpeg_data.codec );
            //
            //        bool holds_video_scale = std::holds_alternative<VideoScale>( codec.video_viewport );
            //        if ( im::Checkbox( "Video Scale##FFMPEG", &holds_video_scale ) )
            //        {
            //            if ( holds_video_scale )
            //                codec.video_viewport.emplace<VideoScale>( DEFAULT_FFMPEG_SCALE );
            //            else
            //                codec.video_viewport.emplace<Default>();
            //        }
            //
            //        im::SameLine();
            //
            //        bool holds_video_crop = std::holds_alternative<VideoCrop>( codec.video_viewport );
            //        if ( im::Checkbox( "Video Crop##FFMPEG", &holds_video_crop ) )
            //        {
            //            if ( holds_video_crop )
            //                codec.video_viewport.emplace<VideoCrop>();
            //            else
            //                codec.video_viewport.emplace<Default>();
            //        }
            //
            //        if ( VideoScale* video_scale = std::get_if<VideoScale>( &codec.video_viewport ) )
            //        {
            //            im::Text( "\t" );
            //            im::SameLine();
            //            im::SetNextItemWidth( 200.0f );
            //            im::DragInt2( "Scale##FFMPEGVideoScale", &video_scale->scale.x, 1.0f, 1, 1'000'000 );
            //        }
            //        else if ( VideoCrop* video_crop = std::get_if<VideoCrop>( &codec.video_viewport ) )
            //        {
            //            im::Text( "\t" );
            //            im::SameLine();
            //            im::SetNextItemWidth( 200.0f );
            //            im::DragInt2( "Position##FFMPEG", &video_crop->position.x, 1.0f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //            im::Text( "\t" );
            //            im::SameLine();
            //            im::SetNextItemWidth( 200.0f );
            //            im::DragInt2( "Size##FFMPEG", &video_crop->size.x, 1.0f, 1, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            //            if ( !ffmpeg_data.input_file.empty() )
            //            {
            //                im::SameLine();
            //                if ( im::Button( "Preview##FFMPEGCrop" ) )
            //                {
            //                    auto timestamp = ffmpeg_data.start_time ? *ffmpeg_data.start_time : Timestamp{};
            //                    preview_crop( ffmpeg_data.input_file, timestamp, *video_crop );
            //                    im::SetCurrentContext( imgui_context );
            //                }
            //            }
            //        }
            //
            //        bool has_video_bitrate_m = codec.video_bitrate_m.has_value();
            //        if ( im::Checkbox( "Video Bitrate [Mb]##FFMPEG", &has_video_bitrate_m ) )
            //        {
            //            if ( has_video_bitrate_m )
            //                codec.video_bitrate_m = DEFAULT_BITRATE_M;
            //            else if ( !codec.gpu_encoder.has_value() )
            //                codec.video_bitrate_m.reset();
            //        }
            //        if ( codec.video_bitrate_m )
            //        {
            //            im::SameLine();
            //            im::SetNextItemWidth( 100.0f );
            //            im::DragFloat( "##FFMPEGVideoBitrate", &*codec.video_bitrate_m, 0.01f, 0.0f, 1e6f );
            //        }
            //
            //        bool has_gpu_encoder = codec.gpu_encoder.has_value();
            //        if ( im::Checkbox( kl::format( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")##FFMPEG" ).c_str(), &has_gpu_encoder ) )
            //        {
            //            if ( has_gpu_encoder )
            //                codec.gpu_encoder.emplace();
            //            else
            //                codec.gpu_encoder.reset();
            //        }
            //        if ( has_gpu_encoder )
            //        {
            //            if ( !codec.video_bitrate_m )
            //                codec.video_bitrate_m = DEFAULT_BITRATE_M;
            //            im::Text( "\t" );
            //            im::SameLine();
            //            codec.gpu_encoder->edit();
            //        }
            //    }
            //
            //    {
            //        std::string temp = kl::convert_string( ffmpeg_data.other_commands );
            //        if ( im::InputTextMultiline( "Custom##FFMPEG", &temp ) )
            //            ffmpeg_data.other_commands = kl::convert_string( temp );
            //    }
            //
            //    const std::wstring ffmpeg_full_command = ffmpeg_data.produce();
            //    im::TextWrapped( "%s", kl::convert_string( ffmpeg_full_command ).c_str() );
            //
            //    if ( im::Button( "Produce##FFMPEG" ) )
            //    {
            //        if ( ffmpeg_data.input_file.empty() || ffmpeg_data.output_file.empty() )
            //            kl::print( kl::colors::RED, "FFMPEG: Input and output files must be specified.", kl::colors::CONSOLE );
            //        else
            //            execute( ffmpeg_full_command );
            //    }
            //}
            //
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
std::wstring VideoScale::produce() const
{
    return kl::wformat( "-vf \"scale=", scale.x, ":", scale.y, "\"" );
}

std::wstring VideoCrop::produce() const
{
    return kl::wformat( "-vf \"crop=", size.x, ":", size.y, ":", position.x, ":", position.y, "\"" );
}

std::wstring CopyCodec::produce() const
{
    return L"-c copy";
}

std::wstring CustomCodec::produce() const
{
    std::wstringstream stream;
    if ( VideoScale const* video_scale = std::get_if<VideoScale>( &video_viewport ) )
        stream << video_scale->produce();
    else if ( VideoCrop const* video_crop = std::get_if<VideoCrop>( &video_viewport ) )
        stream << video_crop->produce();
    if ( video_bitrate_m )
        stream << " -vb " << video_bitrate_m.value() << "M";
    if ( gpu_encoder )
        stream << " -c:v " << gpu_encoder.value().produce();
    return stream.str();
}

std::wstring FFMPEGData::produce() const
{
    std::wstringstream stream;
    stream << "ffmpeg -y";
    stream << " -i \"" << input_file << "\"";
    if ( start_time )
        stream << " -ss " << start_time->total_seconds();
    if ( end_time )
        stream << " -to " << end_time->total_seconds();
    if ( auto* custom_codec = std::get_if<CustomCodec>( &codec ) )
        stream << " " << custom_codec->produce();
    else if ( auto* copy_codec = std::get_if<CopyCodec>( &codec ) )
        stream << " " << copy_codec->produce();
    stream << " " << other_commands;
    stream << " \"" << output_file << "\"";
    std::wstring result = stream.str();
    clean_string( result );
    return result;
}

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

void preview_timestamp( std::wstring_view const& path, Timestamp timestamp )
{
    const std::filesystem::path p{ path };
    const std::wstring preview_name = kl::wformat( "__vidhandle_preview_", kl::convert_string( kl::random::gen_string( 10 ) ), "_.png" );
    std::wstring preview_path = kl::wformat( p.parent_path().wstring(), "/", preview_name );
    preview_path = std::filesystem::absolute( preview_path ).wstring();

    kl::Window window{ kl::format( "Video Handler Timestamp Preview ", p.filename(), " [", timestamp.minutes, ":", timestamp.seconds, "]" ) };
    kl::GPU gpu{ window.ptr() };

    FFMPEGData ffmpeg_data;
    ffmpeg_data.input_file = path;
    ffmpeg_data.output_file = preview_path;
    ffmpeg_data.start_time.emplace( timestamp );
    ffmpeg_data.codec.emplace<CustomCodec>();
    ffmpeg_data.other_commands = L"-frames:v 1";
    execute( ffmpeg_data.produce() );

    kl::Image preview_image{ preview_path };
    if ( preview_image.width() == 0 || preview_image.height() == 0 )
        return;

    kl::dx::Texture preview_texture = gpu.create_texture( preview_image );
    kl::dx::ShaderView preview_view = gpu.create_shader_view( preview_texture, nullptr );
    std::filesystem::remove( preview_path );

    auto* imgui_context = im::CreateContext();
    im::SetCurrentContext( imgui_context );

    auto& imio = im::GetIO();
    imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imio.IniFilename = nullptr;

    ImGui_ImplWin32_Init( window.ptr() );
    ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    im::StyleColorsDark();
    load_theme( 1.35f );

    window.set_dark_mode( true );
    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
        } );
    window.resize( gpu.texture_size( preview_texture ) );
    window.set_position( kl::SCREEN_SIZE / 2 - window.size() / 2 );

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
            ImVec2 view_size = im::GetContentRegionAvail();
            im::Image( preview_view.get(), view_size );
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

void preview_crop( std::wstring_view const& path, Timestamp timestamp, VideoCrop& crop )
{
    const std::filesystem::path p{ path };
    const std::wstring preview_name = kl::wformat( "__vidhandle_preview_", kl::convert_string( kl::random::gen_string( 10 ) ), "_.png" );
    std::wstring preview_path = kl::wformat( p.parent_path().wstring(), "/", preview_name );
    preview_path = std::filesystem::absolute( preview_path ).wstring();

    kl::Window window{ kl::format( "Video Handler Crop Preview ", p.filename(), " [", timestamp.minutes, ":", timestamp.seconds, "]" ) };
    kl::GPU gpu{ window.ptr() };

    FFMPEGData ffmpeg_data;
    ffmpeg_data.input_file = path;
    ffmpeg_data.output_file = preview_path;
    ffmpeg_data.start_time.emplace( timestamp );
    ffmpeg_data.codec.emplace<CustomCodec>();
    ffmpeg_data.other_commands = L"-frames:v 1";
    execute( ffmpeg_data.produce() );

    kl::Image preview_image{ preview_path };
    if ( preview_image.width() == 0 || preview_image.height() == 0 )
        return;

    kl::dx::Texture preview_texture = gpu.create_texture( preview_image );
    kl::dx::ShaderView preview_view = gpu.create_shader_view( preview_texture, nullptr );
    std::filesystem::remove( preview_path );

    auto* imgui_context = im::CreateContext();
    im::SetCurrentContext( imgui_context );

    auto& imio = im::GetIO();
    imio.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    imio.IniFilename = nullptr;

    ImGui_ImplWin32_Init( window.ptr() );
    ImGui_ImplDX11_Init( gpu.device().get(), gpu.context().get() );
    im::StyleColorsDark();
    load_theme( 1.35f );

    window.set_dark_mode( true );
    window.on_resize.emplace_back( [&]( kl::Int2 size )
        {
            gpu.resize_internal( size );
            gpu.set_viewport_size( size );
        } );

    const kl::Int2 texture_size = gpu.texture_size( preview_texture );
    window.resize( texture_size );
    window.set_position( kl::SCREEN_SIZE / 2 - window.size() / 2 );

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
            static constexpr auto convert_coords = []( kl::Int2 pos )
                {
                    return ImVec2{ (float) pos.x, (float) pos.y };
                };

            const ImVec2 min_content = im::GetWindowContentRegionMin();
            const ImVec2 max_content = im::GetWindowContentRegionMax();
            const ImVec2 content_region = max_content - min_content;
            const ImVec2 content_ratio = content_region / convert_coords( texture_size );

            im::Image( preview_view.get(), content_region );

            const ImVec2 top_left = min_content + convert_coords( crop.position ) * content_ratio;
            const ImVec2 bottom_right = min_content + convert_coords( crop.position + crop.size ) * content_ratio;
            static const ImU32 rect_color = ImColor( 255, 255, 255 );

            auto& draw_list = *im::GetWindowDrawList();
            draw_list.AddRect( top_left, bottom_right, rect_color );

            if ( im::IsWindowFocused() )
            {
                const ImVec2 mouse_pos = im::WindowPosAbsToRel( im::GetCurrentWindow(), im::GetMousePos() );
                const ImVec2 video_pos = mouse_pos / content_ratio;
                const kl::Int2 crop_pos = { int( video_pos.x ), int( video_pos.y ) };

                if ( im::IsMouseDown( ImGuiPopupFlags_MouseButtonLeft ) )
                    crop.position = crop_pos;
                else if ( im::IsMouseDown( ImGuiPopupFlags_MouseButtonRight ) )
                    crop.size = crop_pos - crop.position;

                crop.position = kl::clamp( crop.position, kl::Int2{ 0 }, texture_size - kl::Int2{ 1 } );
                crop.size = kl::clamp( crop.size, kl::Int2{ 1 }, texture_size - crop.position );
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
*/
