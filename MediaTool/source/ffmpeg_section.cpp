#include "ffmpeg_section.h"
#include "preview_tools.h"

const kl::Float4 mt::FFMPEGSection::COLOR = kl::RGB{ 151, 255, 227 };

std::wstring mt::VideoScale::produce() const
{
    return kl::wformat( "-vf \"scale=", scale.x, ":", scale.y, "\"" );
}

std::wstring mt::VideoCrop::produce() const
{
    return kl::wformat( "-vf \"crop=", size.x, ":", size.y, ":", position.x, ":", position.y, "\"" );
}

std::wstring mt::DefaultCodec::produce() const
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

std::wstring mt::CopyCodec::produce() const
{
    return L"-c copy";
}

std::wstring mt::FFMPEGSection::produce() const
{
    std::wstringstream stream;
    stream << "ffmpeg -hide_banner -y";
    stream << " -i \"" << input_file << "\"";
    if ( start_time )
        stream << " -ss " << start_time->total_seconds();
    if ( end_time )
        stream << " -to " << end_time->total_seconds();
    if ( auto* default_codec = std::get_if<DefaultCodec>( &codec ) )
        stream << " " << default_codec->produce();
    else if ( auto* copy_codec = std::get_if<CopyCodec>( &codec ) )
        stream << " " << copy_codec->produce();
    stream << " " << other_commands;
    stream << " \"" << output_file << "\"";
    std::wstring result = stream.str();
    clean_string( result );
    return result;
}

void mt::FFMPEGSection::display()
{
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );

    im::Text( "Input File: %s", kl::convert_string( input_file ).c_str() );
    im::SameLine();
    if ( im::Button( QNAME( "Browse##InputFile" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
            input_file = std::filesystem::absolute( *opt_path ).wstring();
    }

    im::Text( "Output File: %s", kl::convert_string( output_file ).c_str() );
    im::SameLine();
    if ( im::Button( QNAME( "Browse##OutputFile" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = std::filesystem::absolute( *opt_path ).wstring();
    }

    bool has_start_time = start_time.has_value();
    if ( im::Checkbox( QNAME( "Start Time" ), &has_start_time ) )
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
        im::DragInt( QNAME( "##StartTimeMinutes" ), &start_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragFloat( QNAME( "##StartTimeSeconds" ), &start_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
        if ( !input_file.empty() )
        {
            im::SameLine();
            if ( im::Button( QNAME( "Preview##StartTime" ) ) )
            {
                preview_timestamp( input_file, *start_time );
                im::SetCurrentContext( imgui_context );
            }
        }
    }

    bool has_end_time = end_time.has_value();
    if ( im::Checkbox( QNAME( "End Time" ), &has_end_time ) )
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
        im::DragInt( QNAME( "##EndTimeMinutes" ), &end_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragFloat( QNAME( "##EndTimeSeconds" ), &end_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
        if ( !input_file.empty() )
        {
            im::SameLine();
            if ( im::Button( QNAME( "Preview##EndTime" ) ) )
            {
                preview_timestamp( input_file, *end_time );
                im::SetCurrentContext( imgui_context );
            }
        }
    }

    bool has_default_codec = std::holds_alternative<DefaultCodec>( codec );
    if ( im::Checkbox( QNAME( "Default Codec" ), &has_default_codec ) )
        codec.emplace<DefaultCodec>();

    im::SameLine();

    bool has_copy_codec = std::holds_alternative<CopyCodec>( codec );
    if ( im::Checkbox( QNAME( "Copy Codec" ), &has_copy_codec ) )
        codec.emplace<CopyCodec>();

    if ( std::holds_alternative<DefaultCodec>( codec ) )
    {
        auto& codec = std::get<DefaultCodec>( this->codec );

        bool holds_video_scale = std::holds_alternative<VideoScale>( codec.video_viewport );
        if ( im::Checkbox( QNAME( "Video Scale" ), &holds_video_scale ) )
        {
            if ( holds_video_scale )
                codec.video_viewport.emplace<VideoScale>( DEFAULT_SCALE );
            else
                codec.video_viewport.emplace<None>();
        }

        im::SameLine();

        bool holds_video_crop = std::holds_alternative<VideoCrop>( codec.video_viewport );
        if ( im::Checkbox( QNAME( "Video Crop" ), &holds_video_crop ) )
        {
            if ( holds_video_crop )
                codec.video_viewport.emplace<VideoCrop>();
            else
                codec.video_viewport.emplace<None>();
        }

        if ( VideoScale* video_scale = std::get_if<VideoScale>( &codec.video_viewport ) )
        {
            im::Text( "\t" );
            im::SameLine();
            im::SetNextItemWidth( 200.0f );
            im::DragInt2( QNAME( "Scale##VideoScale" ), &video_scale->scale.x, 1.0f, 1, 1'000'000 );
        }
        else if ( VideoCrop* video_crop = std::get_if<VideoCrop>( &codec.video_viewport ) )
        {
            im::Text( "\t" );
            im::SameLine();
            im::SetNextItemWidth( 200.0f );
            im::DragInt2( QNAME( "Position##VideoCrop" ), &video_crop->position.x, 1.0f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            im::Text( "\t" );
            im::SameLine();
            im::SetNextItemWidth( 200.0f );
            im::DragInt2( QNAME( "Size##VideoCrop" ), &video_crop->size.x, 1.0f, 1, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            if ( !input_file.empty() )
            {
                im::SameLine();
                if ( im::Button( QNAME( "Preview##VideoCrop" ) ) )
                {
                    auto timestamp = start_time ? *start_time : Timestamp{};
                    preview_crop( input_file, timestamp, *video_crop );
                    im::SetCurrentContext( imgui_context );
                }
            }
        }

        bool has_video_bitrate_m = codec.video_bitrate_m.has_value();
        if ( im::Checkbox( QNAME( "Video Bitrate [Mb]" ), &has_video_bitrate_m ) )
        {
            if ( has_video_bitrate_m )
                codec.video_bitrate_m = DEFAULT_BITRATE_M;
            else if ( !codec.gpu_encoder.has_value() )
                codec.video_bitrate_m.reset();
        }
        if ( codec.video_bitrate_m )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragFloat( QNAME( "##VideoBitrate" ), &*codec.video_bitrate_m, 0.01f, 0.0f, 1e6f );
        }

        bool has_gpu_encoder = codec.gpu_encoder.has_value();
        if ( im::Checkbox( QNAME( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")" ), &has_gpu_encoder ) )
        {
            if ( has_gpu_encoder )
                codec.gpu_encoder.emplace();
            else
                codec.gpu_encoder.reset();
        }
        if ( has_gpu_encoder )
        {
            if ( !codec.video_bitrate_m )
                codec.video_bitrate_m = DEFAULT_BITRATE_M;
            im::Text( "\t" );
            im::SameLine();
            codec.gpu_encoder->edit();
        }
    }

    {
        std::string temp = kl::convert_string( other_commands );
        if ( im::InputTextMultiline( QNAME( "Custom" ), &temp ) )
            other_commands = kl::convert_string( temp );
    }

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };

    const std::wstring full_command = produce();
    const ImVec2 text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPos( ImVec2{
        im::GetWindowWidth() * .5f - text_size.x * .5f,
        im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y - imgui_context->Style.ItemSpacing.y - text_size.y,
        } );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    if ( im::Button( QNAME( "Produce" ), main_button_size ) )
    {
        if ( input_file.empty() || output_file.empty() )
            kl::print( kl::colors::RED, "FFMPEG: Input and output files must be specified.", kl::colors::CONSOLE );
        else
            execute( window.ptr(), full_command );
    }
    im::PopStyleVar( 1 );
}
