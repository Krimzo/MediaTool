#include "process_section.h"
#include "progress_window.h"

const kl::Float4 mt::ProcessSection::COLOR = kl::RGB{ 255, 149, 170 };
static constexpr float EXTENSION_INPUT_WIDTH = 50.0f;
static constexpr int SLEEP_ITERATIONS = 100;
static constexpr int SLEEP_ITERATION_TIME = 100;

std::wstring mt::ProcessSection::produce( fs::path const& input_file, fs::path* outout_file ) const
{
    const auto opt_content_type = kl::probe_content_type( input_file );
    if ( !opt_content_type )
        return {};

    const auto get_output_file = [&]( std::string const& new_extension ) -> std::wstring
        {
            if ( retain_folder_structure )
            {
                fs::path relative_file = fs::relative( input_file, fs::exists( input_file ) ? input_dir : std::wstring{} );
                relative_file.replace_extension( new_extension );
                return output_dir / relative_file;
            }
            else
                return output_dir / fs::path{ input_file.stem().wstring() + kl::convert_string( new_extension ) };
        };

    if ( image_output_ext && opt_content_type->starts_with( "image" ) )
    {
        FFMPEGSection ffmpeg{ window, imgui_context };
        ffmpeg.input_file = input_file;
        ffmpeg.output_file = get_output_file( *image_output_ext );
        ffmpeg.custom_commands = kl::wformat( "-vf \"scale='min(", max_image_dimension, ",iw)':min'(", max_image_dimension, ",ih)':force_original_aspect_ratio=decrease:force_divisible_by=2\"" );
        ffmpeg.codec.emplace<DefaultCodec>();
        if ( outout_file )
            *outout_file = ffmpeg.output_file;
        return ffmpeg.produce( false );
    }
    if ( audio_output_ext && opt_content_type->starts_with( "audio" ) )
    {
        FFMPEGSection ffmpeg{ window, imgui_context };
        ffmpeg.input_file = input_file;
        ffmpeg.output_file = get_output_file( *audio_output_ext );
        ffmpeg.codec.emplace<DefaultCodec>();
        if ( outout_file )
            *outout_file = ffmpeg.output_file;
        return ffmpeg.produce( false );
    }
    if ( video_output_ext && opt_content_type->starts_with( "video" ) )
    {
        int target_framerate = 0;
        if ( fs::exists( input_file ) )
        {
            kl::VideoReader reader{ input_file.generic_wstring(), {}, false };
            if ( reader.fps() > float( max_video_framerate ) )
                target_framerate = max_video_framerate;
        }
        FFMPEGSection ffmpeg{ window, imgui_context };
        ffmpeg.input_file = input_file;
        ffmpeg.output_file = get_output_file( *video_output_ext );
        ffmpeg.custom_commands = kl::wformat( "-vf \"scale='min(", max_video_dimension, ",iw)':min'(", max_video_dimension, ",ih)':force_original_aspect_ratio=decrease:force_divisible_by=2\"" );
        auto& codec = ffmpeg.codec.emplace<DefaultCodec>();
        if ( target_framerate > 0 )
            codec.frame_rate = float( target_framerate );
        codec.video_codec = video_codec;
        if ( outout_file )
            *outout_file = ffmpeg.output_file;
        return ffmpeg.produce( false );
    }
    return {};
}

void mt::ProcessSection::display()
{
    const float starting_cursor_pos_y = im::GetCursorPosY();
    const ImVec2 desc_text_size = im::CalcTextSize( DESCRIPTION.data() );
    im::SetCursorPos( ImVec2{ im::GetContentRegionAvail().x, TAB_BOTTOM_SPACING } * .5f - desc_text_size * .5f );
    im::Text( DESCRIPTION.data() );
    im::SetCursorPosY( starting_cursor_pos_y + TAB_BOTTOM_SPACING );

    im::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 6, 10 } );
    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    if ( im::Button( QNAME( "Input Directory: ", kl::convert_string( input_dir ), "##Input" ) ) )
    {
        if ( auto opt_dir = kl::wchoose_dir() )
            input_dir = fs::absolute( *opt_dir ).wstring();
    }

    if ( im::Button( QNAME( "Output Directory: ", kl::convert_string( output_dir ), "##Output" ) ) )
    {
        if ( auto opt_dir = kl::wchoose_dir() )
            output_dir = fs::absolute( *opt_dir ).wstring();
    }

    im::PopStyleVar( 1 );

    im::Checkbox( QNAME( "Retain Folder Structure" ), &retain_folder_structure );

    bool image_output = image_output_ext.has_value();
    if ( im::Checkbox( QNAME( "Image Output Extension" ), &image_output ) )
    {
        if ( image_output )
            image_output_ext = DEFAULT_IMAGE_OUTPUT_EXTENSION;
        else
            image_output_ext.reset();
    }
    if ( image_output )
    {
        im::SameLine();
        im::SetNextItemWidth( EXTENSION_INPUT_WIDTH );
        im::InputText( QNAME( "##ImageOutputExt" ), &*image_output_ext );
    }

    bool audio_output = audio_output_ext.has_value();
    if ( im::Checkbox( QNAME( "Audio Output Extension" ), &audio_output ) )
    {
        if ( audio_output )
            audio_output_ext = DEFAULT_AUDIO_OUTPUT_EXTENSION;
        else
            audio_output_ext.reset();
    }
    if ( audio_output )
    {
        im::SameLine();
        im::SetNextItemWidth( EXTENSION_INPUT_WIDTH );
        im::InputText( QNAME( "##AudioOutputExt" ), &*audio_output_ext );
    }

    bool video_output = video_output_ext.has_value();
    if ( im::Checkbox( QNAME( "Video Output Extension" ), &video_output ) )
    {
        if ( video_output )
            video_output_ext = DEFAULT_VIDEO_OUTPUT_EXTENSION;
        else
            video_output_ext.reset();
    }
    if ( video_output )
    {
        im::SameLine();
        im::SetNextItemWidth( EXTENSION_INPUT_WIDTH );
        im::InputText( QNAME( "##VideoOutputExt" ), &*video_output_ext );
    }

    im::Text( "Max Image Dimension" );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::DragInt( QNAME( "##MaxImageDimension" ), &max_image_dimension, 0.1f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );

    im::Text( "Max Video Dimension" );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::DragInt( QNAME( "##MaxVideoDimension" ), &max_video_dimension, 0.1f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );

    im::Text( "Max Video Framerate" );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::DragInt( QNAME( "##MaxVideoFramerate" ), &max_video_framerate, 0.1f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );

    im::Text( "Video Codec" );
    im::SameLine();
    bool h264 = ( video_codec.codec_type == VideoCodecType::H264 );
    if ( im::Checkbox( QNAME( "H264" ), &h264 ) )
        video_codec.codec_type = VideoCodecType::H264;
    im::SameLine();
    bool hevc = ( video_codec.codec_type == VideoCodecType::HEVC );
    if ( im::Checkbox( QNAME( "HEVC" ), &hevc ) )
        video_codec.codec_type = VideoCodecType::HEVC;

    const ImVec2 error_box_tl = { imgui_context->Style.WindowPadding.x, im::GetItemRectMax().y };
    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };

    const std::wstring full_command =
        L"Image: " + produce( "./some_image_dir/some_file.png" ) + L"\n\n" +
        L"Audio: " + produce( "./some_audio_dir/some_file.wav" ) + L"\n\n" +
        L"Video: " + produce( "./some_video_dir/some_file.mkv" );
    const ImVec2 text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPos( ImVec2{
        im::GetWindowWidth() * .5f - text_size.x * .5f,
        im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y - imgui_context->Style.ItemSpacing.y - text_size.y,
        } );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );
    const ImVec2 error_box_br = { im::GetWindowWidth() - imgui_context->Style.WindowPadding.x, im::GetItemRectMin().y };

    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    im::BeginDisabled( input_dir.empty() || output_dir.empty() || fs::equivalent( input_dir, output_dir ) || ( !image_output_ext && !audio_output_ext && !video_output_ext ) );
    if ( im::Button( QNAME( "Process" ), main_button_size ) )
        m_last_error = this->process();
    im::EndDisabled();
    im::PopStyleVar( 2 );

    if ( !m_last_error.empty() )
    {
        const ImVec2 text_size = im::CalcTextSize( m_last_error.c_str() );
        im::SetCursorPos( error_box_tl + ( error_box_br - error_box_tl ) * .5f - text_size * .5f - ImVec2{ 0.0f, TAB_BOTTOM_SPACING } );
        im::TextColored( ImColor( 255, 0, 0 ), m_last_error.c_str() );
    }
}

std::string mt::ProcessSection::process() const
{
    struct Input
    {
        std::string input_file;
        std::wstring command;
    };

    if ( !fs::exists( input_dir ) )
        return "Input directory does not exist.";
    std::vector<Input> inputs;
    for ( auto& entry : fs::recursive_directory_iterator( input_dir ) )
    {
        if ( entry.is_directory() )
            continue;
        fs::path output_file;
        const std::wstring command = produce( entry, &output_file );
        if ( command.empty() )
            continue;
        if ( output_file.has_parent_path() )
            fs::create_directories( output_file.parent_path() );
        inputs.emplace_back( fs::path{ entry }.generic_string(), command );
    }
    if ( inputs.empty() )
        return "No files to process.";

    std::mutex mutex;
    std::stringstream stream;
    int counter = 0;
    ProgressWindow progress_window{ (int) inputs.size() };
    std::jthread progress_thread{ [&]() {
        progress_window.run( "Process Progress" );
        } };
    for ( int i = 0; i < SLEEP_ITERATIONS && !progress_window.is_open(); i++ )
        Sleep( SLEEP_ITERATION_TIME );
    std::for_each( std::execution::par, inputs.begin(), inputs.end(), [&]( Input const& input )
        {
            if ( !progress_window.is_open() )
                return;
            if ( ::_wsystem( input.command.data() ) != 0 )
            {
                std::lock_guard lock{ mutex };
                stream << "Failed: " << ( ++counter ) << ". " << input.input_file << "\n";
            }
            progress_window.increment();
        } );
    progress_window.close();
    return stream.str();
}
