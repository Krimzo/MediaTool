#include "concat_section.h"
#include "preview_tools.h"

const kl::Float4 mt::ConcatSection::COLOR = kl::RGB{ 218, 162, 255 };

mt::ConcatInput::ConcatInput()
{
    kl::RGB color;
    while ( kl::YUV{ color }.y < .5f )
        color = kl::random::gen_rgb();
    this->color = color;
}

void mt::ConcatSection::concat() const
{
    if ( inputs.empty() )
        return;
    for ( int i = 0; i < (int) inputs.size(); i++ )
    {
        auto const& input = inputs[i];
        if ( input.path.empty() )
            return;
    }
    if ( output_file.empty() )
        return;
    if ( !fs::exists( output_file ) )
        kl::File{ output_file, true }.close();

    const std::wstring rand_str = kl::convert_string( kl::random::gen_string( 10 ) );
    const std::wstring vids_list_file = kl::wformat( "concat_vids_list__", rand_str, ".txt" );
    {
        std::wofstream list_file{ vids_list_file };
        for ( auto& input : inputs )
            list_file << "file '" << input.path << "'" << std::endl;
    }

    const std::wstring command = kl::wformat( "ffmpeg -hide_banner -y -f concat -i \"", vids_list_file, "\" -c copy \"", output_file, "\"" );
    execute( window.ptr(), command );

    fs::remove( vids_list_file );
}

void mt::ConcatSection::display()
{
    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );

    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    std::optional<int> to_remove;
    for ( int i = 0; i < (int) inputs.size(); i++ )
    {
        auto& input = inputs[i];

        im::PushStyleColor( ImGuiCol_Text, reinterpret_cast<ImVec4 const&>( input.color ) );

        if ( im::Button( QNAME( "Input File ", i + 1, ": ", kl::convert_string( input.path ), "##Input", i ) ) )
        {
            if ( auto opt_path = kl::wchoose_file( false ) )
                input.path = fs::absolute( *opt_path ).wstring();
        }
        im::SameLine();
        if ( im::Button( QNAME( "Remove##Input", i ) ) )
            to_remove = i;
        im::SameLine();
        im::BeginDisabled( i <= 0 );
        if ( im::Button( QNAME( "Move Up##Input", i ) ) )
            std::swap( inputs[i], inputs[(size_t) i - 1] );
        im::EndDisabled();
        im::SameLine();
        im::BeginDisabled( i >= (int) inputs.size() - 1 );
        if ( im::Button( QNAME( "Move Down##Input", i ) ) )
            std::swap( inputs[i], inputs[(size_t) i + 1] );
        im::EndDisabled();

        im::PopStyleColor( 1 );
    }
    if ( im::Button( kl::format( "Input File ", inputs.size() + 1, ": " ).c_str() ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
            inputs.emplace_back().path = fs::absolute( *opt_path ).wstring();
    }
    if ( to_remove )
        inputs.erase( inputs.begin() + *to_remove );

    im::SetCursorPosY( im::GetCursorPosY() + 25.0f );

    if ( im::Button( QNAME( "Output File: ", kl::convert_string( output_file ), "##Output" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = fs::absolute( *opt_path ).wstring();
    }

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };
    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    if ( im::Button( QNAME( "Concat" ), main_button_size ) )
        concat();

    im::PopStyleVar( 2 );
}
