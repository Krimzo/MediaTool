#include "optimizer_section.h"
#include "preview_tools.h"

const kl::Float4 mt::OptimizerSection::COLOR = kl::RGB{ 255, 209, 149 };

void mt::OptimizerSection::optimize() const
{
    static constexpr float COPY_TIME_PRECUT = 15.0f;
    static constexpr float MIN_FILESIZE_PERC = 0.95f;
    static constexpr float AGGRESSIVE_BIAS = 0.98f;
    static constexpr float DEFAULT_BITRATE_M = 5.0f;

    const float start_time = this->start_time.value_or( Timestamp{} ).total_seconds();
    const std::wstring output_dir = std::filesystem::path( this->output_file ).parent_path().wstring();

    const float copy_start_time = kl::max( start_time - COPY_TIME_PRECUT, 0.0f );
    const std::wstring output_copy_file = kl::wformat( output_dir, "/__", kl::convert_string( kl::random::gen_string( 10 ) ), "_.mp4" );

    FFMPEGSection copy_ffmpeg_data{ imgui_context };
    copy_ffmpeg_data.input_file = input_file;
    copy_ffmpeg_data.output_file = output_copy_file;
    copy_ffmpeg_data.start_time.emplace().seconds = copy_start_time;
    copy_ffmpeg_data.end_time = end_time;
    copy_ffmpeg_data.codec.emplace<CopyCodec>();
    if ( !execute( copy_ffmpeg_data.produce() ) )
        return;

    FFMPEGSection encode_ffmpeg_data{ imgui_context };
    encode_ffmpeg_data.input_file = output_copy_file;
    encode_ffmpeg_data.output_file = output_file;
    encode_ffmpeg_data.start_time.emplace().seconds = start_time - copy_start_time;
    auto& codec = encode_ffmpeg_data.codec.emplace<DefaultCodec>();
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

void mt::OptimizerSection::display()
{
    im::Text( "Input File: %s", kl::convert_string( input_file ).c_str() );
    im::SameLine();
    if ( im::Button( "Browse##OptimizerInputFileButton" ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
            input_file = std::filesystem::absolute( *opt_path ).wstring();
    }

    im::Text( "Output File: %s", kl::convert_string( output_file ).c_str() );
    im::SameLine();
    if ( im::Button( "Browse##OptimizerOutputFileButton" ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = std::filesystem::absolute( *opt_path ).wstring();
    }

    bool has_start_time = start_time.has_value();
    if ( im::Checkbox( "Start Time##Optimizer", &has_start_time ) )
    {
        if ( has_start_time )
            start_time.emplace();
        else
            start_time.reset();
    }
    if ( start_time )
    {
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragInt( "##OptimizerStartTimeMinutes", &start_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragFloat( "##OptimizerStartTimeSeconds", &start_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
        if ( !input_file.empty() )
        {
            im::SameLine();
            if ( im::Button( "Preview##OptimizerStartTime" ) )
            {
                preview_timestamp( input_file, *start_time );
                im::SetCurrentContext( imgui_context );
            }
        }
    }

    bool has_end_time = end_time.has_value();
    if ( im::Checkbox( "End Time##Optimizer", &has_end_time ) )
    {
        if ( has_end_time )
            end_time.emplace();
        else
            end_time.reset();
    }
    if ( end_time )
    {
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragInt( "##OptimizerEndTimeMinutes", &end_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragFloat( "##OptimizerEndTimeSeconds", &end_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
        if ( !input_file.empty() )
        {
            im::SameLine();
            if ( im::Button( "Preview##OptimizerEndTime" ) )
            {
                preview_timestamp( input_file, *end_time );
                im::SetCurrentContext( imgui_context );
            }
        }
    }

    bool has_video_scale = video_scale.has_value();
    if ( im::Checkbox( "Video Scale##Optimizer", &has_video_scale ) )
    {
        if ( has_video_scale )
            video_scale = FFMPEGSection::DEFAULT_SCALE;
        else
            video_scale.reset();
    }
    if ( video_scale )
    {
        im::SameLine();
        im::SetNextItemWidth( 200.0f );
        im::DragInt2( "##OptimizerVideoScale", &video_scale->x, 1.0f, 1, 1'000'000 );
    }

    bool has_gpu_encoder = gpu_encoder.has_value();
    if ( im::Checkbox( kl::format( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")##Optimizer" ).c_str(), &has_gpu_encoder ) )
    {
        if ( has_gpu_encoder )
            gpu_encoder.emplace();
        else
            gpu_encoder.reset();
    }
    if ( has_gpu_encoder )
    {
        im::Text( "\t" );
        im::SameLine();
        gpu_encoder->edit();
    }

    im::SetNextItemWidth( 100.0f );
    im::DragFloat( "Max Video Size [MB]##Optimizer", &video_max_size_mb, 0.01f, 0.0f, 1e6f );

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };
    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    if ( im::Button( QNAME( "Optimize" ), main_button_size ) )
    {
        if ( input_file.empty() || output_file.empty() )
            kl::print( kl::colors::RED, "OPTIMIZER: Input and output files must be specified.", kl::colors::CONSOLE );
        else
            optimize();
    }
    im::PopStyleVar( 1 );
}
