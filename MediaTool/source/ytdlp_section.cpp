#include "ytdlp_section.h"

const float4 mt::YTDLPSection::COLOR = kl::RGB{ 255, 242, 152 };

std::wstring mt::YTDLPSection::produce() const
{
    std::wstringstream stream;
    stream << "yt-dlp";
    if ( output_file )
        stream << " --force-overwrite -o \"" << output_file.value() << "\"";
    if ( audio_only )
        stream << " -x";
    provide_clean_string( stream, custom_commands );
    stream << " \"" << url << "\"";
    return stream.str();
}

void mt::YTDLPSection::display()
{
    const float starting_cursor_pos_y = im::GetCursorPosY();
    const ImVec2 desc_text_size = im::CalcTextSize( DESCRIPTION.data() );
    im::SetCursorPos( ImVec2{ im::GetContentRegionAvail().x, TAB_BOTTOM_SPACING } * .5f - desc_text_size * .5f );
    im::Text( DESCRIPTION.data() );
    im::SetCursorPosY( starting_cursor_pos_y + TAB_BOTTOM_SPACING );

    im::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 6, 10 } );
    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    im::SetCursorPosY( im::GetCursorPosY() + imgui_context->Style.FramePadding.y );
    im::Text( "To Download: " );
    im::SameLine();
    {
        std::string temp = kl::convert_string( url );
        im::SetCursorPosY( im::GetCursorPosY() - imgui_context->Style.FramePadding.y );
        im::SetNextItemWidth( -1.0f );
        if ( im::InputText( QNAME( "##UrlInput" ), &temp ) )
            url = kl::convert_string( temp );
    }

    im::PopStyleVar( 1 );

    bool has_output = output_file.has_value();
    im::SetCursorPosY( im::GetCursorPosY() + imgui_context->Style.FramePadding.y );
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
        im::SetCursorPosY( im::GetCursorPosY() - imgui_context->Style.FramePadding.y );
        im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );
        if ( im::Button( QNAME( "Output File: ", kl::convert_string( *output_file ), "##OutputFile" ) ) )
        {
            if ( auto opt_path = kl::wchoose_file( true ) )
                output_file = fs::absolute( *opt_path ).wstring();
        }
        im::PopStyleVar( 1 );
    }

    im::Checkbox( QNAME( "Audio Only" ), &audio_only );

    input_text_multiline_hint( QNAME( "##Custom" ), "Custom Commands", &custom_commands, { -1.0f, 0.0f } );

    const std::wstring full_command = produce();
    const ImVec2 text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPosX( im::GetWindowWidth() * .5f - text_size.x * .5f );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    im::BeginDisabled( url.empty() || ( output_file && output_file->empty() ) );
    if ( im::Button( QNAME( "Download" ), { im::GetContentRegionAvail().x, 30.0f } ) )
        execute( window.ptr(), full_command );
    im::EndDisabled();

    im::PopStyleVar( 2 );

    auto_adjust_window_height( window );
}
