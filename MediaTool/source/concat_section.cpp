#include "concat_section.h"
#include "preview_tools.h"

const kl::Float4 mt::ConcatSection::COLOR = kl::RGB{ 218, 162, 255 };

mt::ConcatInput::ConcatInput()
{
    kl::RGB color;
    while ( kl::YUV{ color }.y < .5f )
        color = kl::random::gen_rgb();
    this->color = color;
}

void mt::ConcatSection::concat() const
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
        FFMPEGSection ffmpeg{ window, imgui_context };
        ffmpeg.other_commands = kl::wformat(
            "-r ", video_rate, "\n",
            "-ar ", audio_rate, "\n" );

        auto& codec = ffmpeg.codec.emplace<DefaultCodec>();
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
            if ( !execute( window.ptr(), ffmpeg.produce() ) )
                return;
            new_input_files.emplace_back( ffmpeg.output_file );
            list_file << "file '" << ffmpeg.output_file << "'" << std::endl;
        }
    }

    const std::wstring command = kl::wformat( "ffmpeg -hide_banner -y -f concat -i \"", list_file_name, "\" -c copy \"", output_file, "\"" );
    execute( window.ptr(), command );

    for ( auto& new_input_file : new_input_files )
        std::filesystem::remove( new_input_file );
    std::filesystem::remove( list_file_name );
}

void mt::ConcatSection::display()
{
    static constexpr size_t MIN_INPUTS_COUNT = 2;

    if ( inputs.size() < MIN_INPUTS_COUNT )
        inputs.resize( MIN_INPUTS_COUNT );

    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );

    std::optional<int> to_remove;
    const bool is_min_size = ( inputs.size() == MIN_INPUTS_COUNT );
    for ( int i = 0; i < (int) inputs.size(); i++ )
    {
        auto& input = inputs[i];

        im::PushStyleColor( ImGuiCol_Text, reinterpret_cast<ImVec4 const&>( input.color ) );

        im::Text( "Input File %d: %s", i + 1, kl::convert_string( input.path ).c_str() );
        im::SameLine();
        if ( im::Button( kl::format( "Browse##ConcatInput", i ).c_str() ) )
        {
            if ( auto opt_path = kl::wchoose_file( false ) )
                input.path = std::filesystem::absolute( *opt_path ).wstring();
        }
        im::SameLine();
        if ( im::Button( kl::format( is_min_size ? "Erase##ConcatInput" : "Remove##ConcatInput", i ).c_str() ) )
            to_remove = i;
        im::SameLine();
        im::BeginDisabled( i <= 0 );
        if ( im::Button( kl::format( "Move Up##ConcatInput", i ).c_str() ) )
            std::swap( inputs[i], inputs[(size_t) i - 1] );
        im::EndDisabled();
        im::SameLine();
        im::BeginDisabled( i >= (int) inputs.size() - 1 );
        if ( im::Button( kl::format( "Move Down##ConcatInput", i ).c_str() ) )
            std::swap( inputs[i], inputs[(size_t) i + 1] );
        im::EndDisabled();

        bool has_start_time = input.start_time.has_value();
        if ( im::Checkbox( kl::format( "Start Time##Concat", i ).c_str(), &has_start_time ) )
        {
            if ( has_start_time )
                input.start_time.emplace();
            else
                input.start_time.reset();
        }
        if ( input.start_time )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragInt( kl::format( "##ConcatStartTimeMinutes", i ).c_str(), &input.start_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragFloat( kl::format( "##ConcatStartTimeSeconds", i ).c_str(), &input.start_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            if ( !input.path.empty() )
            {
                im::SameLine();
                if ( im::Button( kl::format( "Preview##ConcatStartTime", i ).c_str() ) )
                {
                    preview_timestamp( input.path, *input.start_time );
                    im::SetCurrentContext( imgui_context );
                }
            }
        }

        bool has_end_time = input.end_time.has_value();
        if ( im::Checkbox( kl::format( "End Time##Concat", i ).c_str(), &has_end_time ) )
        {
            if ( has_end_time )
                input.end_time.emplace();
            else
                input.end_time.reset();
        }
        if ( input.end_time )
        {
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragInt( kl::format( "##ConcatEndTimeMinutes", i ).c_str(), &input.end_time->minutes, 0.05f, 0, 1'000'000, "%d", ImGuiSliderFlags_AlwaysClamp );
            im::SameLine();
            im::SetNextItemWidth( 100.0f );
            im::DragFloat( kl::format( "##ConcatEndTimeSeconds", i ).c_str(), &input.end_time->seconds, 0.01f, 0.0f, 59.999f, "%.3f", ImGuiSliderFlags_AlwaysClamp );
            if ( !input.path.empty() )
            {
                im::SameLine();
                if ( im::Button( kl::format( "Preview##ConcatEndTime", i ).c_str() ) )
                {
                    preview_timestamp( input.path, *input.end_time );
                    im::SetCurrentContext( imgui_context );
                }
            }
        }

        im::PopStyleColor( 1 );
    }
    if ( im::Button( "Add Input##Concat" ) )
        inputs.emplace_back();
    if ( to_remove )
    {
        if ( is_min_size )
            inputs[*to_remove] = {};
        else
            inputs.erase( inputs.begin() + *to_remove );
    }

    im::Text( "Output File: %s", kl::convert_string( output_file ).c_str() );
    im::SameLine();
    if ( im::Button( "Browse##ConcatOutput" ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = std::filesystem::absolute( *opt_path ).wstring();
    }

    im::SetNextItemWidth( 125.0f );
    im::DragInt2( "Video Scale##Concat", &video_scale.x, 1.0f, 1, 1'000'000 );

    im::SetNextItemWidth( 100.0f );
    im::DragInt( "Video Rate##Concat", &video_rate, 1.0f, 1, 1'000'000 );

    im::SetNextItemWidth( 100.0f );
    im::DragInt( "Audio Rate##Concat", &audio_rate, 1.0f, 1, 1'000'000 );

    bool has_video_bitrate_m = video_bitrate_m.has_value();
    if ( im::Checkbox( "Video Bitrate [Mb]##Concat", &has_video_bitrate_m ) )
    {
        if ( has_video_bitrate_m )
            video_bitrate_m = FFMPEGSection::DEFAULT_BITRATE_M;
        else if ( !gpu_encoder.has_value() )
            video_bitrate_m.reset();
    }
    if ( video_bitrate_m )
    {
        im::SameLine();
        im::SetNextItemWidth( 100.0f );
        im::DragFloat( "##ConcatVideoBitrate", &*video_bitrate_m, 0.01f, 0.0f, 1e6f );
    }

    bool has_gpu_encoder = gpu_encoder.has_value();
    if ( im::Checkbox( kl::format( "GPU Encoder (", kl::convert_string( GPUEncoder::ADAPTER_NAME ), ")##Concat" ).c_str(), &has_gpu_encoder ) )
    {
        if ( has_gpu_encoder )
            gpu_encoder.emplace();
        else
            gpu_encoder.reset();
    }
    if ( has_gpu_encoder )
    {
        if ( !video_bitrate_m )
            video_bitrate_m = FFMPEGSection::DEFAULT_BITRATE_M;
        im::Text( "\t" );
        im::SameLine();
        gpu_encoder->edit();
    }

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };
    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    if ( im::Button( QNAME( "Concat" ), main_button_size ) )
        concat();
    im::PopStyleVar( 1 );
}
