#pragma once

#include "logger.h"

namespace mt
{
struct ProgressSector
{
    kl::RGB color;
    int total_count = 0;
    int counter = 0;

    bool increment();
};

struct ProgressWindow
{
    std::vector<ProgressSector> sectors;
    float progress_color_dim = 0.5f;

    void run(std::string_view const& title);
    void increment();
    void close();

    bool is_open() const;

  private:
    std::mutex m_mutex;
    HWND progress_hwnd = nullptr;
};
} // namespace mt
