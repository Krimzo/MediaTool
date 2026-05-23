#pragma once

#include "general.h"


namespace mt
{
struct SystemSection : Displayable
{
    static const kl::Float4 COLOR;

    kl::Window const& window;

    SystemSection( kl::Window const& window )
        : window( window )
    {}

    void display() override;
};
}
