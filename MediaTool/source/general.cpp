#include "general.h"


void mt::clean_string( std::string& data )
{
    kl::replace_all( data, "\n", " " );
    while ( data.contains( "  " ) )
        kl::replace_all( data, "  ", " " );
    if ( data.empty() )
        return;
    for ( int i = 0; i < (int) data.size(); i++ )
    {
        if ( !std::isspace( data[i] ) )
        {
            data = data.substr( size_t( i ) );
            break;
        }
        else if ( i == (int) data.size() - 1 )
        {
            data = {};
            return;
        }
    }
    for ( int i = (int) data.size() - 1; i >= 0; i-- )
    {
        if ( !std::isspace( data[i] ) )
        {
            data = data.substr( 0, size_t( i + 1 ) );
            break;
        }
    }
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
