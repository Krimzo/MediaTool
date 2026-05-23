#include "general.h"


void mt::clean_string( std::wstring& data )
{
    kl::replace_all( data, L"\n", L" " );
    while ( data.contains( L"  " ) )
        kl::replace_all( data, L"  ", L" " );
}

int mt::execute( HWND window, std::wstring_view const& command )
{
    RECT rect;
    GetWindowRect( window, &rect );

    MoveWindow( GetConsoleWindow(), rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top, false );
    kl::console::set_title( command );

    ShowWindow( window, SW_HIDE );
    ShowWindow( GetConsoleWindow(), SW_SHOW );
    SetForegroundWindow( GetConsoleWindow() );

    ::_wsystem( L"cls" );
    const int result = ::_wsystem( command.data() );
    ::_wsystem( L"pause" );

    ShowWindow( GetConsoleWindow(), SW_HIDE );
    ShowWindow( window, SW_SHOW );
    SetForegroundWindow( window );

    return result;
}

bool mt::custom_button( bool is_pressed, char const* label, ImVec2 const& size_arg, kl::Float4 const& color )
{
    if ( is_pressed )
    {
        im::PushItemFlag( ImGuiItemFlags_Disabled, true );
        im::PushStyleColor( ImGuiCol_Text, reinterpret_cast<ImVec4 const&>( color ) );
        im::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.0f );
    }
    const bool result = im::Button( label, size_arg );
    if ( is_pressed )
    {
        im::PopStyleVar( 1 );
        im::PopStyleColor( 1 );
        im::PopItemFlag();
    }
    return result;
}
