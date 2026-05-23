#include "optimizer_section.h"
#include "preview_tools.h"

const kl::Float4 mt::OptimizerSection::COLOR = kl::RGB{ 255, 209, 149 };

void mt::OptimizerSection::optimize() const
{
    static constexpr float BIAS = 0.98f;

    FFMPEGSection encode_ffmpeg_data{ window, imgui_context };
    encode_ffmpeg_data.input_file = input_file;
    encode_ffmpeg_data.output_file = output_file;
    auto& codec = encode_ffmpeg_data.codec.emplace<DefaultCodec>();
    codec.video_bitrate_m = 10.0f;
    codec.gpu_encoder = gpu_encoder;

    while ( true )
    {
        if ( !execute( window.ptr(), encode_ffmpeg_data.produce() ) )
            return;
        const float file_size_mb = float( fs::file_size( output_file ) / ( 1024.0 * 1024.0 ) );
        if ( file_size_mb <= max_size_mb )
            break;
        *codec.video_bitrate_m *= ( max_size_mb / file_size_mb ) * BIAS;
    }
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

    bool has_gpu_encoder = gpu_encoder.has_value();
    if ( im::Checkbox( QNAME( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")" ), &has_gpu_encoder ) )
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

    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );
    im::SetCursorPosY( im::GetCursorPosY() + imgui_context->Style.FramePadding.y );
    im::Text( "Max Size [MB]: " );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::SetCursorPosY( im::GetCursorPosY() - imgui_context->Style.FramePadding.y );
    im::DragFloat( QNAME( "##MaxSize" ), &max_size_mb, 0.01f, 0.0f, 1e6f );
    im::PopStyleVar( 1 );

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };
    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    if ( im::Button( QNAME( "Optimize" ), main_button_size ) )
    {
        if ( !input_file.empty() && !output_file.empty() )
            optimize();
    }

    im::PopStyleVar( 2 );
}
