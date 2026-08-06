#include "general.h"


std::wstring mt::clean_string( std::string const& source )
{
    std::wstring data = kl::convert_string( source );
    kl::replace_all( data, L"\n", L" " );
    while ( data.contains( L"  " ) )
        kl::replace_all( data, L"  ", L" " );
    if ( data.empty() )
        return data;
    for ( int i = 0; i < (int) data.size(); i++ )
    {
        if ( !std::iswspace( data[i] ) )
        {
            data = data.substr( size_t( i ) );
            break;
        }
        else if ( i == (int) data.size() - 1 )
        {
            data = {};
            return data;
        }
    }
    for ( int i = (int) data.size() - 1; i >= 0; i-- )
    {
        if ( !std::iswspace( data[i] ) )
        {
            data = data.substr( 0, size_t( i + 1 ) );
            break;
        }
    }
    return data;
}

bool mt::provide_clean_string( std::wstringstream& stream, std::string const& source )
{
    const std::wstring clean_string = mt::clean_string( source );
    if ( clean_string.empty() )
        return false;
    stream << ' ' << clean_string;
    return true;
}

bool mt::execute( HWND window, std::wstring_view const& command, bool pause )
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
    if ( pause )
        ::_wsystem( L"pause" );

    ShowWindow( GetConsoleWindow(), SW_HIDE );
    ShowWindow( window, SW_SHOW );
    SetForegroundWindow( window );

    return ( result == 0 );
}

void mt::auto_adjust_window_height( kl::Window const& window )
{
    static constexpr float MAX_SCREEN_HEIGHT_PERC = .8f;

    if ( window.maximized() )
        return;

    const int final_cursor_y = kl::min( (int) ImGui::GetCursorScreenPos().y, int( kl::SCREEN_SIZE.y * MAX_SCREEN_HEIGHT_PERC ) );
    if ( final_cursor_y != window.height() )
        QUEUED_WINDOW_HEIGHT.emplace( final_cursor_y );
}
