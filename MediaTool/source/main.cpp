#include "media_tool.h"


int wmain( int argc, wchar_t** argv )
{
    mt::MediaTool media_tool{};
    while ( media_tool.update() );
    return 0;
}
