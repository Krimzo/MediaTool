#include "system_section.h"

const kl::Float4 mt::SystemSection::COLOR = kl::RGB{ 117, 203, 224 };

void mt::SystemSection::display()
{
    im::TextColored( (ImVec4 const&) COLOR, "SYSTEM" );
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 235, 99, 99 ) );
    if ( im::Button( QNAME( "Reinstall YT-DLP" ) ) )
        execute( LR"(winget install "yt-dlp" --skip-dependencies)" );
    im::SameLine();
    im::PushStyleColor( ImGuiCol_Text, (ImU32) ImColor( 155, 227, 104 ) );
    if ( im::Button( QNAME( "Reinstall FFMPEG" ) ) )
        execute( LR"(winget install "ffmpeg" --skip-dependencies)" );
    im::PopStyleColor( 2 );
}
