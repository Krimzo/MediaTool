#include "ffmpeg_section.h"
#include "preview_tools.h"

const kl::Float4 mt::FFMPEGSection::COLOR = kl::RGB{ 151, 255, 227 };

void mt::Timestamp::load( float seconds_value )
{
    this->minutes = int( seconds_value / 60 );
    this->seconds = seconds_value - ( this->minutes * 60 );
}

float mt::Timestamp::total_seconds() const
{
    return minutes * 60 + seconds;
}

std::wstring mt::VideoScale::produce() const
{
    return kl::wformat( " -vf \"scale=", scale.x, ":", scale.y, "\"" );
}

std::wstring mt::VideoCrop::produce() const
{
    const kl::Int2 size = bottom_right - top_left;
    return kl::wformat( " -vf \"crop=", size.x, ":", size.y, ":", top_left.x, ":", top_left.y, "\"" );
}

std::wstring mt::DefaultCodec::produce() const
{
    std::wstringstream stream;
    if ( VideoScale const* video_scale = std::get_if<VideoScale>( &video_viewport ) )
        stream << video_scale->produce();
    else if ( VideoCrop const* video_crop = std::get_if<VideoCrop>( &video_viewport ) )
        stream << video_crop->produce();
    if ( frame_rate )
        stream << " -r " << frame_rate.value();
    if ( video_bitrate_m )
        stream << " -vb " << video_bitrate_m.value() << "M";
    if ( video_codec )
        stream << " -c:v " << video_codec.value().produce();
    if ( audio_rate )
        stream << " -ar " << audio_rate.value();
    if ( audio_bitrate_k )
        stream << " -ab " << audio_bitrate_k.value() << "K";
    if ( audio_codec )
        stream << " -c:a " << audio_codec.value().produce();
    return stream.str();
}

std::wstring mt::CopyCodec::produce() const
{
    return L" -c copy";
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
        stream << default_codec->produce();
    else if ( auto* copy_codec = std::get_if<CopyCodec>( &codec ) )
        stream << copy_codec->produce();
    if ( !custom_commands.empty() )
        stream << " " << custom_commands;
    stream << " \"" << output_file << "\"";
    return stream.str();
}

void mt::FFMPEGSection::display()
{
    im::SetCursorPosY( im::GetCursorPosY() + TAB_BOTTOM_SPACING );

    im::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 6, 10 } );
    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    if ( im::Button( QNAME( "Input File: ", kl::convert_string( input_file ), "##Input" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
            input_file = fs::absolute( *opt_path ).wstring();
    }

    if ( im::Button( QNAME( "Output File: ", kl::convert_string( output_file ), "##OutputFile" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = fs::absolute( *opt_path ).wstring();
    }

    im::PopStyleVar( 1 );

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
        if ( !input_file.empty() && kl::probe_content_type( input_file ).value_or( {} ).starts_with( "video" ) )
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
        if ( !input_file.empty() && kl::probe_content_type( input_file ).value_or( {} ).starts_with( "video" ) )
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
            im::DragInt2( QNAME( "Top Left##VideoCrop" ), &video_crop->top_left.x, 1.0f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            im::Text( "\t" );
            im::SameLine();
            im::SetNextItemWidth( 200.0f );
            im::DragInt2( QNAME( "Bottom Right##VideoCrop" ), &video_crop->bottom_right.x, 1.0f, 1, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            if ( !input_file.empty() && kl::probe_content_type( input_file ).value_or( {} ).starts_with( "video" ) )
            {
                im::SameLine();
                if ( im::Button( QNAME( "Preview##VideoCrop" ) ) )
                {
                    const auto timestamp = start_time ? *start_time : Timestamp{};
                    preview_crop( input_file, timestamp, *video_crop );
                    im::SetCurrentContext( imgui_context );
                }
            }
        }

        bool has_frame_rate = codec.frame_rate.has_value();
        if ( im::Checkbox( QNAME( "Frame Rate" ), &has_frame_rate ) )
        {
            if ( has_frame_rate )
                codec.frame_rate = DEFAULT_FRAME_RATE;
            else
                codec.frame_rate.reset();
        }
        if ( codec.frame_rate )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragFloat( QNAME( "##FrameRate" ), &*codec.frame_rate, 0.01f, 0.0f, 1e6f );
        }

        bool has_video_bitrate_m = codec.video_bitrate_m.has_value();
        if ( im::Checkbox( QNAME( "Video Bitrate [Mb]" ), &has_video_bitrate_m ) )
        {
            if ( has_video_bitrate_m )
                codec.video_bitrate_m = DEFAULT_VIDEO_BITRATE_M;
            else if ( !codec.video_codec || !codec.video_codec->uses_gpu() )
                codec.video_bitrate_m.reset();
        }
        if ( codec.video_bitrate_m )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragFloat( QNAME( "##VideoBitrate" ), &*codec.video_bitrate_m, 0.01f, 0.0f, 1e6f );
        }

        bool has_video_codec = codec.video_codec.has_value();
        if ( im::Checkbox( QNAME( "Video Codec (", kl::convert_string( GPU_ADAPTER_NAME ), ")" ), &has_video_codec ) )
        {
            if ( has_video_codec )
                codec.video_codec.emplace();
            else
                codec.video_codec.reset();
        }
        if ( has_video_codec )
        {
            if ( codec.video_codec->uses_gpu() && !codec.video_bitrate_m )
                codec.video_bitrate_m = DEFAULT_VIDEO_BITRATE_M;
            im::Text( "\t" );
            im::SameLine();
            codec.video_codec->edit();
        }

        bool has_audio_rate = codec.audio_rate.has_value();
        if ( im::Checkbox( QNAME( "Audio Rate" ), &has_audio_rate ) )
        {
            if ( has_audio_rate )
                codec.audio_rate = DEFAULT_AUDIO_RATE;
            else
                codec.audio_rate.reset();
        }
        if ( codec.audio_rate )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragInt( QNAME( "##AudioRate" ), &*codec.audio_rate, 0.1f, 0, 10'000'000 );
        }

        bool has_audio_bitrate_k = codec.audio_bitrate_k.has_value();
        if ( im::Checkbox( QNAME( "Audio Bitrate [Kb]" ), &has_audio_bitrate_k ) )
        {
            if ( has_audio_bitrate_k )
                codec.audio_bitrate_k = DEFAULT_AUDIO_BITRATE_K;
            else
                codec.audio_bitrate_k.reset();
        }
        if ( codec.audio_bitrate_k )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragFloat( QNAME( "##AudioBitrate" ), &*codec.audio_bitrate_k, 0.01f, 0.0f, 1e6f );
        }

        bool has_audio_codec = codec.audio_codec.has_value();
        if ( im::Checkbox( QNAME( "Audio Codec" ), &has_audio_codec ) )
        {
            if ( has_audio_codec )
                codec.audio_codec.emplace();
            else
                codec.audio_codec.reset();
        }
        if ( has_audio_codec )
        {
            im::Text( "\t" );
            im::SameLine();
            codec.audio_codec->edit();
        }
    }

    std::string custom_input = kl::convert_string( custom_commands );
    if ( im::InputTextMultiline( QNAME( "##Custom" ), &custom_input, { -1.0f, 0.0f } ) )
    {
        mt::clean_string( custom_input );
        custom_commands = kl::convert_string( custom_input );
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
    im::BeginDisabled( input_file.empty() || output_file.empty() || ( start_time && end_time && start_time->total_seconds() >= end_time->total_seconds() ) );
    if ( im::Button( QNAME( "Produce" ), main_button_size ) )
    {
        if ( !input_file.empty() && !output_file.empty() )
            execute( window.ptr(), full_command );
    }
    im::EndDisabled();
    im::PopStyleVar( 2 );
}
