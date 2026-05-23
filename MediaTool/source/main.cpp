#include "media_tool.h"


int wmain( int argc, wchar_t** argv )
{
    mt::MediaTool media_tool{};
    if ( argc > 1 )
        media_tool.ffmpeg_section->input_file = argv[1];
    while ( media_tool.update() );
    return 0;
}
