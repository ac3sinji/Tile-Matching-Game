#include "MapSettingPanel.h"

#include <string>

void MapSettingPanel::Create(HWND parent) {
    parent_ = parent;
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(parent, GWLP_HINSTANCE));

    panel_ = CreateWindowEx(
        0,
        L"STATIC",
        L"Map Size Settings",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        kPanelX,
        kPanelY,
        kPanelWidth,
        kPanelHeight,
        parent,
        nullptr,
        instance,
        nullptr);

    const int firstRowY = kControlMargin + 36;

    CreateWindowEx(
        0,
        L"STATIC",
        L"Width",
        WS_CHILD | WS_VISIBLE,
        kControlMargin,
        firstRowY,
        kLabelWidth,
        kInputHeight,
        panel_,
        nullptr,
        instance,
        nullptr);

    widthInput_ = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"10",
        WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL,
        kControlMargin + kLabelWidth,
        firstRowY,
        kInputWidth,
        kInputHeight,
        panel_,
        reinterpret_cast<HMENU>(kWidthInputId),
        instance,
        nullptr);

    CreateWindowEx(
        0,
        L"STATIC",
        L"Height",
        WS_CHILD | WS_VISIBLE,
        kControlMargin,
        firstRowY + kRowGap,
        kLabelWidth,
        kInputHeight,
        panel_,
        nullptr,
        instance,
        nullptr);

    heightInput_ = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"10",
        WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL,
        kControlMargin + kLabelWidth,
        firstRowY + kRowGap,
        kInputWidth,
        kInputHeight,
        panel_,
        reinterpret_cast<HMENU>(kHeightInputId),
        instance,
        nullptr);

    CreateWindowEx(
        0,
        L"BUTTON",
        L"Create Map",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        kControlMargin,
        firstRowY + (kRowGap * 2),
        kLabelWidth + kInputWidth,
        34,
        panel_,
        reinterpret_cast<HMENU>(kCreateMapButtonId),
        instance,
        nullptr);
}

bool MapSettingPanel::HandleCommand(WPARAM wParam) const {
    if (LOWORD(wParam) != kCreateMapButtonId || HIWORD(wParam) != BN_CLICKED) {
        return false;
    }

    if (widthInput_ == nullptr || heightInput_ == nullptr || parent_ == nullptr) {
        return true;
    }

    const int mapWidth = ReadInputValue(widthInput_);
    const int mapHeight = ReadInputValue(heightInput_);

    if (mapWidth <= 0 || mapHeight <= 0) {
        MessageBox(parent_, L"Width and height must be positive integers.", L"Input Error", MB_OK | MB_ICONWARNING);
        return true;
    }

    ShowMapSizeDialog(parent_, mapWidth, mapHeight);
    return true;
}

int MapSettingPanel::ReadInputValue(HWND inputHwnd) {
    wchar_t buffer[16]{};
    GetWindowText(inputHwnd, buffer, ARRAYSIZE(buffer));
    return _wtoi(buffer);
}

void MapSettingPanel::ShowMapSizeDialog(HWND parent, int width, int height) {
    std::wstring message = L"Map Size: " + std::to_wstring(width) + L" x " + std::to_wstring(height);
    MessageBox(parent, message.c_str(), L"Create Map", MB_OK | MB_ICONINFORMATION);
}
