#include "ytdlp_section.h"

const kl::Float4 mt::YTDLPSection::COLOR = kl::RGB{ 219, 202, 118 };

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
                output_file = std::filesystem::absolute( *opt_path ).wstring();
        }
        im::SameLine();
        im::Text( "Output File: %s", kl::convert_string( *output_file ).data() );
    }

    im::Checkbox( QNAME( "Audio Only" ), &audio_only );

    const std::wstring full_command = produce();
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    if ( im::Button( QNAME( "Download" ) ) )
    {
        if ( url.empty() )
            kl::print( kl::colors::RED, "YTDLP: Video link must be specified.", kl::colors::CONSOLE );
        else
            execute( full_command );
    }
}
