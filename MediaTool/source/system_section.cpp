#include "system_section.h"
#include "ytdlp_section.h"
#include "ffmpeg_section.h"

const kl::Float4 mt::SystemSection::COLOR = kl::RGB{ 160, 236, 255 };

void mt::SystemSection::display()
{
    static const kl::Float4 PYTHON_COLOR = kl::RGB{ 178, 255, 178 };
    static const kl::Float4 DENO_COLOR = kl::RGB{ 125, 240, 255 };

    static constexpr std::string_view PYTHON_INSTALL_TEXT = "Install/Upgrade PYTHON (yt-dlp requirement)";
    static constexpr std::string_view DENO_INSTALL_TEXT = "Install/Upgrade DENO (yt-dlp requirement)";
    static constexpr std::string_view YTDLP_INSTALL_TEXT = "Install/Upgrade YT-DLP";
    static constexpr std::string_view FFMPEG_INSTALL_TEXT = "Install/Upgrade FFMPEG";

    static constexpr std::string_view PYTHON_INSTALL_COMMAND = R"(winget install python)";
    static constexpr std::string_view DENO_INSTALL_COMMAND = R"(winget install deno)";
    static constexpr std::string_view YTDLP_INSTALL_COMMAND = R"(pip install yt-dlp)";
    static constexpr std::string_view FFMPEG_INSTALL_COMMAND = R"(winget install ffmpeg)";

    const ImVec2 DEFAULT_BUTTON_SIZE = { im::GetContentRegionAvail().x * .6f, 25.0f };

    im::PushStyleVar( ImGuiStyleVar_FramePadding, { 5, 5 } );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( PYTHON_COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( PYTHON_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );
    im::Text( PYTHON_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - DEFAULT_BUTTON_SIZE.x * .5f );
    if ( im::Button( QNAME( PYTHON_INSTALL_COMMAND.data() ), DEFAULT_BUTTON_SIZE ) )
        execute( window.ptr(), kl::convert_string( PYTHON_INSTALL_COMMAND ) );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( DENO_COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( DENO_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );
    im::Text( DENO_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - DEFAULT_BUTTON_SIZE.x * .5f );
    if ( im::Button( QNAME( DENO_INSTALL_COMMAND ), DEFAULT_BUTTON_SIZE ) )
        execute( window.ptr(), kl::convert_string( DENO_INSTALL_COMMAND ) );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( YTDLPSection::COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( YTDLP_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );
    im::Text( YTDLP_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - DEFAULT_BUTTON_SIZE.x * .5f );
    if ( im::Button( QNAME( YTDLP_INSTALL_COMMAND ), DEFAULT_BUTTON_SIZE ) )
        execute( window.ptr(), kl::convert_string( YTDLP_INSTALL_COMMAND ) );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( FFMPEGSection::COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( FFMPEG_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );
    im::Text( FFMPEG_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - DEFAULT_BUTTON_SIZE.x * .5f );
    if ( im::Button( QNAME( FFMPEG_INSTALL_COMMAND ), DEFAULT_BUTTON_SIZE ) )
        execute( window.ptr(), kl::convert_string( FFMPEG_INSTALL_COMMAND ) );

    im::PopStyleColor( 4 );
    im::PopStyleVar( 1 );
}
