#pragma once

#include "klibrary.h"

namespace mt
{
namespace fs = std::filesystem;
namespace im = ImGui;

struct None
{};

struct Displayable
{
    virtual ~Displayable() = default;
    virtual void display() = 0;
};

template<typename... Args>
std::string qname( void const* ptr, Args&&... args )
{
    return kl::format( args..., "##", size_t( ptr ) );
}

#define QNAME(...) (qname(this, __VA_ARGS__).c_str())

void clean_string( std::string& data );
bool execute( HWND window, std::wstring_view const& command, bool pause = true );
}
