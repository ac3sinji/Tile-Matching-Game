#pragma once

#include <windows.h>

class MapSettingPanel {
public:
    static constexpr int kCreateMapButtonId = 1003;

    void Create(HWND parent);
    bool HandleCommand(WPARAM wParam) const;

private:
    static constexpr int kPanelX = 24;
    static constexpr int kPanelY = 24;
    static constexpr int kPanelWidth = 360;
    static constexpr int kPanelHeight = 200;

    static constexpr int kControlMargin = 20;
    static constexpr int kLabelWidth = 120;
    static constexpr int kInputWidth = 140;
    static constexpr int kInputHeight = 28;
    static constexpr int kRowGap = 44;

    static constexpr int kWidthInputId = 1001;
    static constexpr int kHeightInputId = 1002;

    static int ReadInputValue(HWND inputHwnd);
    static void ShowMapSizeDialog(HWND parent, int width, int height);

    HWND parent_ = nullptr;
    HWND panel_ = nullptr;
    HWND widthInput_ = nullptr;
    HWND heightInput_ = nullptr;
};
