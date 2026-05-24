#pragma once

#include "ffmpeg_section.h"


namespace mt
{
void preview_timestamp( fs::path const& path, Timestamp& timestamp );
void preview_crop( fs::path const& path, Timestamp timestamp, VideoCrop& crop );
}
