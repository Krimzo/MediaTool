#pragma once

#include "drag_drop.h"

namespace mt
{
struct None
{
};

struct Displayable
{
    virtual ~Displayable() = default;
    virtual void display() = 0;
};

inline constexpr kl::RGB MAIN_BACKGROUND_COLOR = {36, 36, 36};
inline constexpr float TAB_BOTTOM_SPACING = 25.0f;
inline std::optional<int> QUEUED_WINDOW_HEIGHT;

template <typename... Args> std::string qname(void const* ptr, Args&&... args)
{
    return kl::format(args..., "##", size_t(ptr));
}

#define QNAME(...) (qname(this, __VA_ARGS__).c_str())

std::wstring clean_string(std::string const& source);
bool provide_clean_string(std::wstringstream& stream, std::string const& source);
bool execute(HWND window, std::wstring_view const& command, bool pause = true);
void auto_adjust_window_height(kl::Window const& window);

// imgui addons
bool input_text_multiline_hint(const char* label, const char* hint, char* buf, size_t buf_size,
                               const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0,
                               ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
bool input_text_multiline_hint(const char* label, const char* hint, std::string* str, const ImVec2& size = ImVec2(0, 0),
                               ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr,
                               void* user_data = nullptr);
} // namespace mt
