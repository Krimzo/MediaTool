#include "general.h"


void mt::clean_string( std::wstring& data )
{
    kl::replace_all( data, L"\n", L" " );
    while ( data.contains( L"  " ) )
        kl::replace_all( data, L"  ", L" " );
}

int mt::execute( std::wstring_view const& command )
{
    static const auto special_color = kl::RGB{ 199, 117, 157 };
    static const auto console_color = kl::colors::CONSOLE;
    kl::console::set_title( kl::wformat( "Executing: ", command ) );
    const int result = ::_wsystem( command.data() );
    kl::wprint( "Command \"", special_color, command, console_color, "\" exited with ", special_color, result, console_color );
    kl::console::set_title( "Waiting" );
    return result;
}
