#pragma once

#include "klibrary.h"

namespace mt
{
namespace im = ImGui;

struct None
{};

template<typename... Args>
std::string qname( void const* ptr, Args&&... args )
{
    return kl::format( args..., "##", size_t( ptr ) );
}

#define QNAME(...) (qname(this, __VA_ARGS__).c_str())

void clean_string( std::wstring& data );
int execute( std::wstring_view const& command );
}
