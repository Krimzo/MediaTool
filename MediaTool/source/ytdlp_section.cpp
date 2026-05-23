#include "ytdlp_section.h"

const kl::Float4 mt::YTDLPSection::COLOR = kl::RGB{ 255, 242, 152 };

std::wstring mt::YTDLPSection::produce() const
{
    std::wstringstream stream;
    stream << "yt-dlp";
    if ( output_file )
        stream << " --force-overwrite -o \"" << output_file.value() << "\"";
    if ( audio_only )
        stream << " -f bestaudio";
    stream << " \"" << url << "\"";
    std::wstring result = stream.str();
    clean_string( result );
    return result;
}

void mt::YTDLPSection::display()
{
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );

    im::Text( "To Download" );
    im::SameLine();
    {
        std::string temp = kl::convert_string( url );
        if ( im::InputText( QNAME(), &temp ) )
            url = kl::convert_string( temp );
    }

    bool has_output = output_file.has_value();
    if ( im::Checkbox( QNAME( "Output" ), &has_output ) )
    {
        if ( has_output )
            output_file.emplace();
        else
            output_file.reset();
    }
    if ( output_file )
    {
        im::SameLine();
        if ( im::Button( QNAME( "Browse" ) ) )
        {
            if ( auto opt_path = kl::wchoose_file( true ) )
                output_file = fs::absolute( *opt_path ).wstring();
        }
        im::SameLine();
        im::Text( "Output File: %s", kl::convert_string( *output_file ).data() );
    }

    im::Checkbox( QNAME( "Audio Only" ), &audio_only );

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
    if ( im::Button( QNAME( "Download" ), main_button_size ) )
    {
        if ( !url.empty() )
            execute( window.ptr(), full_command );
    }
    im::PopStyleVar( 1 );
}
