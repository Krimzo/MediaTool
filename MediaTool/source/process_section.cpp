#include "process_section.h"
#include "progress_window.h"

const kl::Float4 mt::ProcessSection::COLOR = kl::RGB{ 255, 149, 170 };
static constexpr float EXTENSION_INPUT_WIDTH = 50.0f;
static constexpr int SLEEP_ITERATIONS = 100;
static constexpr int SLEEP_ITERATION_TIME = 100;

std::wstring mt::ProcessSection::produce( fs::path const& input_file, MediaType& out_media_type, fs::path* outout_file ) const
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
        if ( !image_custom_commands.empty() )
            ffmpeg.custom_commands += L" " + image_custom_commands;
        ffmpeg.codec.emplace<DefaultCodec>();
        out_media_type = MediaType::IMAGE;
        if ( outout_file )
            *outout_file = ffmpeg.output_file;
        return ffmpeg.produce( false );
    }
    else if ( audio_output_ext && opt_content_type->starts_with( "audio" ) )
    {
        FFMPEGSection ffmpeg{ window, imgui_context };
        ffmpeg.input_file = input_file;
        ffmpeg.output_file = get_output_file( *audio_output_ext );
        ffmpeg.custom_commands = audio_custom_commands;
        ffmpeg.codec.emplace<DefaultCodec>();
        out_media_type = MediaType::AUDIO;
        if ( outout_file )
            *outout_file = ffmpeg.output_file;
        return ffmpeg.produce( false );
    }
    else if ( video_output_ext && opt_content_type->starts_with( "video" ) )
    {
        FFMPEGSection ffmpeg{ window, imgui_context };
        ffmpeg.use_hardware_decoding = use_hardware_decoding;
        ffmpeg.input_file = input_file;
        ffmpeg.output_file = get_output_file( *video_output_ext );
        ffmpeg.custom_commands = kl::wformat( "-vf \"scale='min(", max_video_dimension, ",iw)':min'(", max_video_dimension, ",ih)':force_original_aspect_ratio=decrease:force_divisible_by=2,fps=fps='min(", max_video_framerate, ",source_fps)'\"" );
        if ( !video_custom_commands.empty() )
            ffmpeg.custom_commands += L" " + video_custom_commands;
        auto& codec = ffmpeg.codec.emplace<DefaultCodec>();
        codec.video_bitrate_m = video_bitrate_m;
        codec.video_codec = video_codec;
        out_media_type = MediaType::VIDEO;
        if ( outout_file )
            *outout_file = ffmpeg.output_file;
        return ffmpeg.produce( false );
    }
    else
    {
        out_media_type = MediaType::IGNORED;
        return {};
    }
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

    im::Checkbox( QNAME( "Recursive Search" ), &recursive_search );
    if ( recursive_search )
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

    im::Checkbox( QNAME( "Hardware Decoding" ), &use_hardware_decoding );

    bool has_video_bitrate_m = video_bitrate_m.has_value();
    if ( im::Checkbox( QNAME( "Video Bitrate [Mb]" ), &has_video_bitrate_m ) )
    {
        if ( has_video_bitrate_m )
            video_bitrate_m = DEFAULT_VIDEO_BITRATE_M;
        else
            video_bitrate_m.reset();
    }
    if ( video_bitrate_m )
    {
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragFloat( QNAME( "##VideoBitrate" ), &*video_bitrate_m, 0.01f, 0.0f, 1e6f );
    }

    im::Text( kl::format( "Video Codec (", kl::convert_string( GPU_ADAPTER_NAME ), ")" ).c_str() );
    im::Text( "\t" );
    im::SameLine();
    video_codec.edit();

    std::string image_custom_input = kl::convert_string( image_custom_commands );
    if ( im::InputTextMultilineHint( QNAME( "##CustomImage" ), "Image Custom Commands", &image_custom_input, { -1.0f, 0.0f } ) )
    {
        mt::clean_string( image_custom_input );
        image_custom_commands = kl::convert_string( image_custom_input );
    }

    std::string audio_custom_input = kl::convert_string( audio_custom_commands );
    if ( im::InputTextMultilineHint( QNAME( "##CustomAudio" ), "Audio Custom Commands", &audio_custom_input, { -1.0f, 0.0f } ) )
    {
        mt::clean_string( audio_custom_input );
        audio_custom_commands = kl::convert_string( audio_custom_input );
    }

    std::string video_custom_input = kl::convert_string( video_custom_commands );
    if ( im::InputTextMultilineHint( QNAME( "##CustomVideo" ), "Video Custom Commands", &video_custom_input, { -1.0f, 0.0f } ) )
    {
        mt::clean_string( video_custom_input );
        video_custom_commands = kl::convert_string( video_custom_input );
    }

    MediaType _med_typ{};
    std::wstring full_command = produce( "*.png", _med_typ );
    ImVec2 text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPosX( im::GetWindowWidth() * .5f - text_size.x * .5f );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    full_command = produce( "*.wav", _med_typ );
    text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPosX( im::GetWindowWidth() * .5f - text_size.x * .5f );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    full_command = produce( "*.mkv", _med_typ );
    text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPosX( im::GetWindowWidth() * .5f - text_size.x * .5f );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    std::error_code _ignored_error{};
    im::BeginDisabled( input_dir.empty() || output_dir.empty() || fs::equivalent( input_dir, output_dir, _ignored_error ) || ( !image_output_ext && !audio_output_ext && !video_output_ext ) );
    if ( im::Button( QNAME( "Process" ), { im::GetContentRegionAvail().x, 30.0f } ) )
        this->process();
    im::EndDisabled();

    im::PopStyleVar( 2 );

    auto_adjust_window_height( window );
}

void mt::ProcessSection::process() const
{
    struct Input
    {
        std::wstring input_file;
        MediaType media_type{};
        std::wstring command;
    };

    if ( !fs::exists( input_dir ) )
    {
        Logger::log( COLOR, "Input directory does not exist." );
        return;
    }

    std::vector<Input> video_inputs;
    std::vector<Input> image_audio_inputs;
    const auto search_func = [&]( fs::directory_entry const& entry )
        {
            if ( entry.is_directory() )
                return;
            MediaType media_type{};
            fs::path output_file;
            const std::wstring command = produce( entry, media_type, &output_file );
            if ( command.empty() )
                return;
            if ( output_file.has_parent_path() )
                fs::create_directories( output_file.parent_path() );
            ( ( media_type == MediaType::VIDEO ) ? video_inputs : image_audio_inputs )
                .emplace_back( fs::path{ entry }.generic_wstring(), media_type, command );
        };
    if ( recursive_search )
        for ( auto& entry : fs::recursive_directory_iterator( input_dir ) )
            search_func( entry );
    else
        for ( auto& entry : fs::directory_iterator( input_dir ) )
            search_func( entry );

    if ( video_inputs.empty() && image_audio_inputs.empty() )
    {
        Logger::log( COLOR, "No files to process." );
        return;
    }

    std::sort( video_inputs.begin(), video_inputs.end(), []( Input const& left, Input const& right )
        {
            return fs::file_size( left.input_file ) > fs::file_size( right.input_file );
        } );

    ProgressWindow progress_window{ (int) video_inputs.size() + (int) image_audio_inputs.size() };
    std::jthread progress_thread{ [&]() {
        progress_window.run( "Process Progress" );
        } };
    for ( int i = 0; i < SLEEP_ITERATIONS && !progress_window.is_open(); i++ )
        Sleep( SLEEP_ITERATION_TIME );

    const auto process_func = [&]( Input const& input )
        {
            if ( !progress_window.is_open() )
                return;
            if ( ::_wsystem( input.command.data() ) != 0 )
                Logger::log( COLOR, "Process failed at file: ", kl::convert_string( input.input_file ) );
            progress_window.increment();
        };

    progress_window.progress_color = VIDEO_PROGRESS_COLOR;
    std::for_each( video_inputs.begin(), video_inputs.end(), process_func );

    progress_window.progress_color = IMAGE_AUDIO_PROGRESS_COLOR;
    std::for_each( std::execution::par, image_audio_inputs.begin(), image_audio_inputs.end(), process_func );

    progress_window.close();
}
