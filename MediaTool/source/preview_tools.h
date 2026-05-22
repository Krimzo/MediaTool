#pragma once

#include "ffmpeg_section.h"


namespace mt
{
void preview_timestamp( std::wstring_view const& path, Timestamp& timestamp );
void preview_crop( std::wstring_view const& path, Timestamp timestamp, VideoCrop& crop );
}
