#include "concat_section.h"
#include "preview_tools.h"

const kl::Float4 mt::ConcatSection::COLOR = kl::RGB{ 218, 162, 255 };

bool mt::ConcatFileInfo::load( fs::path const& path )
{
    if ( !kl::probe_content_type( path ).value_or( {} ).starts_with( "video" ) )
        return false;
    kl::VideoReader reader{ path.generic_wstring(), {}, false };
    video_resolution = reader.frame_size();
    frame_rate = reader.fps();
    audio_rate = reader.audio_rate();
    return true;
}

bool mt::ConcatFileInfo::match( ConcatFileInfo const& other ) const
{
    return this->video_resolution == other.video_resolution
        && this->frame_rate == other.frame_rate
        && this->audio_rate == other.audio_rate;
}

void mt::ConcatFileInfo::display( ConcatFileInfo const* to_compare, std::vector<BoundingBox>& out_boxes )
{
    im::PushItemFlag( ImGuiItemFlags_Disabled, true );
    im::SetNextItemWidth( 200.0f );
    im::DragInt2( QNAME( "Resolution" ), &video_resolution.x );
    if ( to_compare && to_compare->video_resolution != video_resolution )
        out_boxes.emplace_back( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::DragInt( QNAME( "Framerate" ), &frame_rate );
    if ( to_compare && to_compare->frame_rate != frame_rate )
        out_boxes.emplace_back( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
    im::SameLine();
    im::SetNextItemWidth( 100.0f );
    im::DragInt( QNAME( "Audio Rate" ), &audio_rate );
    if ( to_compare && to_compare->audio_rate != audio_rate )
        out_boxes.emplace_back( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
    im::PopItemFlag();
}

mt::ConcatInput::ConcatInput()
{
    kl::RGB color;
    while ( kl::YUV{ color }.y < .5f )
        color = kl::random::gen_rgb();
    this->color = color;
}

bool mt::ConcatInput::set_path( std::wstring path )
{
    this->path = path;
    return info.load( this->path );
}

void mt::ConcatInput::reload()
{
    const auto write_time = fs::last_write_time( path );
    if ( write_time == m_last_write_time )
        return;
    m_last_write_time = write_time;
    info.load( this->path );
}

std::wstring mt::ConcatSection::produce() const
{
    std::wstringstream stream;
    stream << "ffmpeg -hide_banner -y";
    stream << " -f concat -safe 0 -i \"" << vids_list_file << "\"";
    if ( !custom_commands.empty() )
        stream << " " << custom_commands;
    else if ( use_copy_codec )
        stream << " -c copy";
    stream << " \"" << output_file << "\"";
    return stream.str();
}

void mt::ConcatSection::display()
{
    static constexpr float VERTICAL_SPACING = 20.0f;
    static const ImU32 CROSS_LINE_COLOR = ImColor( 255, 0, 0 );

    im::SetCursorPosY( im::GetCursorPosY() + TAB_BOTTOM_SPACING );
    im::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2{ 5, 5 } );

    std::optional<int> to_remove;
    std::vector<BoundingBox> boxes_to_cross;
    for ( int i = 0; i < (int) inputs.size(); i++ )
    {
        auto& input = inputs[i];

        im::PushStyleColor( ImGuiCol_Text, reinterpret_cast<ImVec4 const&>( input.color ) );

        if ( im::Button( QNAME( "Input File ", i + 1, ": ", kl::convert_string( input.path ), "##Input", i ) ) )
        {
            if ( auto opt_path = kl::wchoose_file( false ) )
                input.set_path( fs::absolute( *opt_path ).wstring() );
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

        inputs[i].reload();
        inputs[i].info.display( &inputs[0].info, boxes_to_cross );

        im::PopStyleColor( 1 );
    }
    if ( im::Button( kl::format( "Input File ", inputs.size() + 1, ": " ).c_str() ) )
    {
        if ( auto opt_path = kl::wchoose_file( false ) )
            inputs.emplace_back().set_path( fs::absolute( *opt_path ).wstring() );
    }
    if ( to_remove )
        inputs.erase( inputs.begin() + *to_remove );

    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );

    if ( im::Button( QNAME( "Output File: ", kl::convert_string( output_file ), "##Output" ) ) )
    {
        if ( auto opt_path = kl::wchoose_file( true ) )
            output_file = fs::absolute( *opt_path ).wstring();
    }

    im::PopStyleVar( 1 );

    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );

    im::BeginDisabled( !custom_commands.empty() );
    im::Checkbox( QNAME( "Use Copy Codec" ), &use_copy_codec );
    im::EndDisabled();

    im::SetCursorPosY( im::GetCursorPosY() + VERTICAL_SPACING );

    std::string custom_input = kl::convert_string( custom_commands );
    if ( im::InputTextMultiline( QNAME( "##Custom" ), &custom_input, { -1.0f, 0.0f } ) )
    {
        mt::clean_string( custom_input );
        custom_commands = kl::convert_string( custom_input );
    }

    auto& draw_list = *im::GetWindowDrawList();
    for ( auto& box : boxes_to_cross )
        draw_list.AddLine( box.top_left, box.bottom_right, CROSS_LINE_COLOR );

    const ImVec2 main_button_size = { im::GetContentRegionAvail().x, 30.0f };

    const std::wstring full_command = produce();
    const ImVec2 text_size = im::CalcTextSize( kl::convert_string( full_command ).c_str(), nullptr, false, im::GetContentRegionAvail().x );
    im::SetCursorPos( ImVec2{
        im::GetWindowWidth() * .5f - text_size.x * .5f,
        im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y - imgui_context->Style.ItemSpacing.y - text_size.y,
        } );
    im::TextWrapped( "%s", kl::convert_string( full_command ).c_str() );

    im::SetCursorPosY( im::GetWindowHeight() - imgui_context->Style.WindowPadding.y - main_button_size.y );
    im::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
    im::BeginDisabled( inputs.empty() || output_file.empty() || !boxes_to_cross.empty() );
    if ( im::Button( QNAME( "Concat" ), main_button_size ) )
        concat();
    im::EndDisabled();
    im::PopStyleVar( 1 );
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

    {
        std::wofstream list_file{ vids_list_file };
        for ( auto& input : inputs )
            list_file << "file '" << input.path << "'" << std::endl;
    }

    const std::wstring command = produce();
    execute( window.ptr(), command );

    fs::remove( vids_list_file );
}
