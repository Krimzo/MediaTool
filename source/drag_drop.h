#pragma once

#include "klibrary.h"

namespace mt
{
namespace fs = std::filesystem;
namespace im = ImGui;

struct DragDropPackage
{
    int2 position{-1};
    std::vector<std::wstring> paths;

    void load(int2 pos, std::vector<std::wstring> const& paths);
    void reset();

    void dragdrop_to_imgui(std::function<void(std::wstring const&)> const& callback);
    void dragdrop_to_imgui_first(std::wstring& out_first_path);
    void dragdrop_to_imgui_file(std::wstring& out_first_path);
    void dragdrop_to_imgui_dir(std::wstring& out_first_path);
};

inline DragDropPackage DRAG_DROP_PACKAGE{};
} // namespace mt
