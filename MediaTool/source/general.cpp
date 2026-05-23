#include "general.h"


void mt::clean_string( std::wstring& data )
{
    kl::replace_all( data, L"\n", L" " );
    while ( data.contains( L"  " ) )
        kl::replace_all( data, L"  ", L" " );
}

int mt::execute( std::wstring_view const& command )
{
    static const auto SPECIAL_COLOR = kl::RGB{ 199, 117, 157 };
    static const auto DEFAULT_COLOR = kl::colors::CONSOLE;
    kl::console::set_title( kl::wformat( "Executing: ", command ) );
    const int result = ::_wsystem( command.data() );
    kl::wprint( "Command \"", SPECIAL_COLOR, command, DEFAULT_COLOR, "\" exited with ", SPECIAL_COLOR, result, DEFAULT_COLOR );
    kl::console::set_title( "Waiting" );
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
