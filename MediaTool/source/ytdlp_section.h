#pragma once

#include "general.h"


namespace mt
{
struct YTDLPSection : Displayable
{
    static const kl::Float4 COLOR;

    std::wstring url;
    std::optional<std::wstring> output_file;
    bool audio_only = false;

    std::wstring produce() const;
    void display() override;;
};
}
