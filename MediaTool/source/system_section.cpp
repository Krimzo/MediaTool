#include "system_section.h"
#include "ytdlp_section.h"
#include "ffmpeg_section.h"

const kl::Float4 mt::SystemSection::COLOR = kl::RGB{ 160, 236, 255 };

void mt::SystemSection::display()
{
    static constexpr float VERTICAL_SPACING = 25.0f;
    static constexpr float FFPROBE_ADDITIONAL_SPACING = 10.0f;

    static const kl::Float4 PYTHON_COLOR = kl::RGB{ 178, 255, 178 };
    static const kl::Float4 DENO_COLOR = kl::RGB{ 125, 240, 255 };

    static constexpr std::string_view PYTHON_INSTALL_TEXT = "Install/Upgrade PYTHON (yt-dlp requirement)";
    static constexpr std::string_view DENO_INSTALL_TEXT = "Install/Upgrade DENO (yt-dlp requirement)";
    static constexpr std::string_view YTDLP_INSTALL_TEXT = "Install/Upgrade YT-DLP";
    static constexpr std::string_view FFMPEG_INSTALL_TEXT = "Install/Upgrade FFMPEG";
    static constexpr std::string_view FFPROBE_TEXT = "Probe Media Info With FFPROBE";

    static constexpr std::string_view PYTHON_INSTALL_COMMAND = R"(winget install Python.Python.3.14)";
    static constexpr std::string_view DENO_INSTALL_COMMAND = R"(winget install DenoLand.Deno)";
    static constexpr std::string_view YTDLP_INSTALL_COMMAND = R"(pip install yt-dlp)";
    static constexpr std::string_view FFMPEG_INSTALL_COMMAND = R"(winget install ffmpeg)";
    static constexpr std::string_view FFPROBE_COMMAND = R"(ffprobe -hide_banner "...")";

    const ImVec2 default_button_size = { im::GetContentRegionAvail().x * .6f, 25.0f };

    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    static const std::string WORKING_DIR = fs::current_path().string();
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( WORKING_DIR.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + TAB_BOTTOM_SPACING );
    im::Text( WORKING_DIR.data() );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( PYTHON_COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( PYTHON_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );
    im::Text( PYTHON_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - default_button_size.x * .5f );
    if ( im::Button( QNAME( PYTHON_INSTALL_COMMAND.data() ), default_button_size ) )
        execute( window.ptr(), kl::convert_string( PYTHON_INSTALL_COMMAND ) );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( DENO_COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( DENO_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );
    im::Text( DENO_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - default_button_size.x * .5f );
    if ( im::Button( QNAME( DENO_INSTALL_COMMAND ), default_button_size ) )
        execute( window.ptr(), kl::convert_string( DENO_INSTALL_COMMAND ) );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( YTDLPSection::COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( YTDLP_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );
    im::Text( YTDLP_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - default_button_size.x * .5f );
    if ( im::Button( QNAME( YTDLP_INSTALL_COMMAND ), default_button_size ) )
        execute( window.ptr(), kl::convert_string( YTDLP_INSTALL_COMMAND ) );

    im::PushStyleColor( ImGuiCol_Text, (ImU32) ( ImColor ) reinterpret_cast<ImVec4 const&>( FFMPEGSection::COLOR ) );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( FFMPEG_INSTALL_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );
    im::Text( FFMPEG_INSTALL_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - default_button_size.x * .5f );
    if ( im::Button( QNAME( FFMPEG_INSTALL_COMMAND ), default_button_size ) )
        execute( window.ptr(), kl::convert_string( FFMPEG_INSTALL_COMMAND ) );

    im::PopStyleColor( 4 );

    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - im::CalcTextSize( FFPROBE_TEXT.data() ).x * .5f );
    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING + FFPROBE_ADDITIONAL_SPACING );
    im::Text( FFPROBE_TEXT.data() );
    im::SetCursorPosX( im::GetContentRegionAvail().x * .5f - default_button_size.x * .5f );
    if ( im::Button( QNAME( FFPROBE_COMMAND ), default_button_size ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
        {
            std::wstring full_command = kl::convert_string( FFPROBE_COMMAND );
            kl::replace_all( full_command, L"...", fs::absolute( *opt_path ).wstring() );
            execute( window.ptr(), full_command );
        }
    }

    im::PopStyleVar( 1 );
}
