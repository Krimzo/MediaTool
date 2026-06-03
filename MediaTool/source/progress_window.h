#pragma once

#include "general.h"


namespace mt
{
struct ProgressWindow
{
    ProgressWindow( int count );

    void run( std::string_view const& title );
    void increment();
    void close();

    bool is_open() const;

private:
    const int m_count;
    std::atomic_int m_counter;
    HWND progress_hwnd = nullptr;
};
}
