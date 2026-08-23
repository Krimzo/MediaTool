#include "drag_drop.h"


void mt::DragDropPackage::load( int2 pos, std::vector<std::wstring> const& paths )
{
    this->position = pos;
    this->paths = paths;
}

void mt::DragDropPackage::reset()
{
    position = int2{ -1 };
    paths.clear();
}

void mt::DragDropPackage::dragdrop_to_imgui( std::function<void( std::wstring const& )> const& callback )
{
    const ImVec2 top_left = im::GetItemRectMin();
    const ImVec2 bottom_right = im::GetItemRectMax();
    if ( !float2{ position }.in_bounds( reinterpret_cast<float2 const&>( top_left ), reinterpret_cast<float2 const&>( bottom_right ) ) )
        return;
    for ( auto& path : paths )
        callback( path );
    reset();
}

void mt::DragDropPackage::dragdrop_to_imgui_first( std::wstring& out_first_path )
{
    bool saved = false;
    dragdrop_to_imgui( [&]( std::wstring const& path )
        {
            if ( saved )
                return;
            out_first_path = path;
            saved = true;
        } );
}

void mt::DragDropPackage::dragdrop_to_imgui_file( std::wstring& out_first_path )
{
    std::wstring temp;
    dragdrop_to_imgui_first( temp );
    if ( temp.empty() )
        return;
    if ( fs::is_directory( temp ) )
        return;
    out_first_path = temp;
}

void mt::DragDropPackage::dragdrop_to_imgui_dir( std::wstring& out_first_path )
{
    std::wstring temp;
    dragdrop_to_imgui_first( temp );
    if ( temp.empty() )
        return;
    if ( !fs::is_directory( temp ) )
    {
        const fs::path path = temp;
        if ( path.has_parent_path() )
            out_first_path = path.parent_path().wstring();
        return;
    }
    out_first_path = temp;
}
