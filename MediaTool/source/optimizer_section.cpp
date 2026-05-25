#include "optimizer_section.h"
#include "preview_tools.h"

const kl::Float4 mt::OptimizerSection::COLOR = kl::RGB{ 255, 209, 149 };

std::wstring mt::OptimizerSection::produce( float bitrate_m ) const
{
    FFMPEGSection ffmpeg{ window, imgui_context };
    ffmpeg.input_file = input_file;
    ffmpeg.output_file = output_file;
    auto& codec = ffmpeg.codec.emplace<DefaultCodec>();
    codec.video_bitrate_m = bitrate_m;
    codec.video_codec = video_codec;
    return ffmpeg.produce();
}

void mt::OptimizerSection::display()
{
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );

    im::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 6, 10 } );
    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    if ( im::Button( QNAME( "Input File: ", kl::convert_string( input_file ), "##Input" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
            input_file = fs::absolute( *opt_path ).wstring();
    }

    if ( im::Button( QNAME( "Output File: ", kl::convert_string( output_file ), "##Output" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = fs::absolute( *opt_path ).wstring();
    }

    im::PopStyleVar( 1 );

    bool has_video_codec = video_codec.has_value();
    if ( im::Checkbox( QNAME( "Video Codec (", kl::convert_string( GPU_ADAPTER_NAME ), ")" ), &has_video_codec ) )
    {
        if ( has_video_codec )
            video_codec.emplace();
        else
            video_codec.reset();
    }
    if ( has_video_codec )
    {
        im::Text( "\t" );
        im::SameLine();
        video_codec->edit();
    }

    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );
    im::SetCursorPosY( im::GetCursorPosY() + imgui_context->Style.FramePadding.y );
    im::Text( "Max Size [MB]: " );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::SetCursorPosY( im::GetCursorPosY() - imgui_context->Style.FramePadding.y );
    im::DragFloat( QNAME( "##MaxSize" ), &max_size_mb, 0.01f, 0.0f, 1e6f );
    im::PopStyleVar( 1 );

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };

    const std::wstring full_command = produce( STARTING_BITRATE );
    const ImVec2 text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPos( ImVec2{
        im::GetWindowWidth() * .5f - text_size.x * .5f,
        im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y - imgui_context->Style.ItemSpacing.y - text_size.y,
        } );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    if ( im::Button( QNAME( "Optimize" ), main_button_size ) )
    {
        if ( !input_file.empty() && !output_file.empty() )
            optimize();
    }

    im::PopStyleVar( 2 );
}

void mt::OptimizerSection::optimize() const
{
    static constexpr float BIAS = 0.98f;
    float bitrate_m = STARTING_BITRATE;
    if ( kl::probe_content_type( input_file ).value_or( {} ).starts_with( "video" ) )
    {
        kl::VideoReader reader{ input_file, {}, false };
        bitrate_m = ( max_size_mb * 8 ) / reader.duration_seconds();
    }
    while ( execute( window.ptr(), produce( bitrate_m ), false ) )
    {
        const float file_size_mb = float( fs::file_size( output_file ) / ( 1024.0 * 1024.0 ) );
        if ( file_size_mb <= max_size_mb )
            break;
        bitrate_m *= ( max_size_mb / file_size_mb ) * BIAS;
    }
}
