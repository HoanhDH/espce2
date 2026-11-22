#pragma once

#include <windows.h>
#include <commdlg.h>
#include <iomanip>
#include <sstream>
#include <string>
#include "MouseMeasurement.hpp"
#include "Resource.h"
#include "SavedSensitivity.hpp"

#ifdef _MSC_VER
#pragma comment(lib, "comdlg32.lib")
#endif

namespace click_practice {
inline void ShowSensitivity(HWND dialog, std::optional<double> value) {
    if (!value) {
        SetDlgItemTextW(dialog, IDC_MOUSE_SENSITIVITY, L"Saved CS2 sensitivity: unavailable");
        return;
    }
    std::wostringstream text;
    text << L"Saved CS2 sensitivity: " << std::setprecision(8) << *value;
    SetDlgItemTextW(dialog, IDC_MOUSE_SENSITIVITY, text.str().c_str());
}

inline void ChooseSensitivityFile(HWND dialog) {
    wchar_t path[32768]{};
    OPENFILENAMEW file{};
    file.lStructSize = sizeof(file);
    file.hwndOwner = dialog;
    file.lpstrTitle = L"Open saved CS2 user convars";
    file.lpstrFilter = L"CS2 user convars (*.vcfg)\0*.vcfg\0";
    file.lpstrFile = path;
    file.nMaxFile = sizeof(path) / sizeof(path[0]);
    file.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_DONTADDTORECENT;
    if (GetOpenFileNameW(&file))
        ShowSensitivity(dialog, ReadSensitivityFile(path));
}

struct Settings {
    int key = VK_F6;
    const char* key_name = "F6";
    int delay_ms = 150;
    int zone = 2; // 0 = head, 1 = body, 2 = either
    bool toggle = false;
};

struct Session {
    Settings settings;
    bool key_down = false;
    bool active = false;
    bool fired = false;
    HWND pending = nullptr;
    ULONGLONG due = 0;
    HWND receiving = nullptr;
    unsigned clicks = 0;
    MouseMeasurement measurement;

    void Disarm() {
        active = false;
        fired = false;
        pending = nullptr;
    }

    void ToggleMeasurement(HWND dialog) {
        if (measurement.Active()) {
            const auto dpi = measurement.Finish();
            SetDlgItemTextW(dialog, IDC_MEASURE_DPI, L"Measure DPI");
            if (dpi) {
                const auto label = L"Estimated mouse DPI: " + std::to_wstring(*dpi);
                SetDlgItemTextW(dialog, IDC_MOUSE_DPI, label.c_str());
                SetDlgItemTextW(dialog, IDC_DPI_GUIDE, L"Repeat the 10 cm measurement if you change mouse or DPI setting.");
            } else {
                SetDlgItemTextW(dialog, IDC_DPI_GUIDE, L"No usable measurement. Retry with one mouse on a straight 10 cm path.");
            }
            key_down = (GetKeyState(settings.key) & 0x8000) != 0;
            return;
        }
        if (!measurement.Start(dialog)) {
            SetDlgItemTextW(dialog, IDC_DPI_GUIDE, L"Mouse input is unavailable for measurement in this window.");
            return;
        }
        Disarm();
        SetFocus(GetDlgItem(dialog, IDC_MEASURE_DPI));
        SetDlgItemTextW(dialog, IDC_MEASURE_DPI, L"Finish (Space)");
        SetDlgItemTextW(dialog, IDC_DPI_GUIDE, L"Move the mouse exactly 10 cm left or right. Press Space to finish.");
    }

    void Tick(HWND dialog) {
        const bool down = (GetKeyState(settings.key) & 0x8000) != 0;
        if (GetForegroundWindow() != dialog) {
            Disarm();
        } else if (down && !key_down) {
            if (active && settings.toggle)
                Disarm();
            else
                active = true;
        } else if (!down && !settings.toggle) {
            Disarm();
        }
        key_down = down;
        if (!active || fired) return;

        POINT cursor{};
        const HWND hit = GetCursorPos(&cursor) ? WindowFromPoint(cursor) : nullptr;
        const HWND head = GetDlgItem(dialog, IDC_PRACTICE_HEAD);
        const HWND body = GetDlgItem(dialog, IDC_PRACTICE_BODY);
        if (!hit || !((hit == head && settings.zone != 1) || (hit == body && settings.zone != 0))) {
            pending = nullptr;
            return;
        }

        const ULONGLONG now = GetTickCount64();
        if (hit != pending) {
            pending = hit;
            due = now + settings.delay_ms;
        }
        if (now < due) return;

        fired = true;
        // BM_CLICK delivers a left-button down/up pair to this dialog's own control.
        receiving = hit;
        SendMessageW(receiving, BM_CLICK, 0, 0);
        receiving = nullptr;
    }
};

inline INT_PTR CALLBACK Procedure(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam) {
    auto* session = reinterpret_cast<Session*>(GetWindowLongPtrW(dialog, DWLP_USER));
    switch (message) {
    case WM_INITDIALOG: {
        session = reinterpret_cast<Session*>(lparam);
        SetWindowLongPtrW(dialog, DWLP_USER, lparam);
        const auto& settings = session->settings;
        std::string help = std::string(settings.toggle ? "Toggle " : "Hold ") + settings.key_name +
            " over an enabled target for " + std::to_string(settings.delay_ms) + " ms.";
        SetDlgItemTextA(dialog, IDC_PRACTICE_HELP, help.c_str());
        ShowSensitivity(dialog, ReadSavedSensitivity());
        EnableWindow(GetDlgItem(dialog, IDC_PRACTICE_HEAD), settings.zone != 1);
        EnableWindow(GetDlgItem(dialog, IDC_PRACTICE_BODY), settings.zone != 0);
        if (!SetTimer(dialog, 1, 10, nullptr)) EndDialog(dialog, -1);
        return TRUE;
    }
    case WM_ACTIVATE:
        if (session && LOWORD(wparam) == WA_INACTIVE) {
            session->Disarm();
            if (session->measurement.Active()) {
                session->measurement.Cancel();
                SetDlgItemTextW(dialog, IDC_MEASURE_DPI, L"Measure DPI");
                SetDlgItemTextW(dialog, IDC_DPI_GUIDE, L"Measurement canceled when the window lost focus.");
            }
        }
        if (session) session->key_down = (GetKeyState(session->settings.key) & 0x8000) != 0;
        break;
    case WM_TIMER:
        if (session && wparam == 1 && !session->measurement.Active()) session->Tick(dialog);
        return TRUE;
    case WM_INPUT:
        if (session && session->measurement.Active() && GET_RAWINPUT_CODE_WPARAM(wparam) == RIM_INPUT)
            session->measurement.Add(reinterpret_cast<HRAWINPUT>(lparam));
        return FALSE;
    case WM_COMMAND:
        if (LOWORD(wparam) == IDCANCEL) {
            EndDialog(dialog, 0);
            return TRUE;
        }
        if (LOWORD(wparam) == IDC_SENSITIVITY_FILE && HIWORD(wparam) == BN_CLICKED) {
            ChooseSensitivityFile(dialog);
            return TRUE;
        }
        if (session && LOWORD(wparam) == IDC_MEASURE_DPI && HIWORD(wparam) == BN_CLICKED) {
            session->ToggleMeasurement(dialog);
            return TRUE;
        }
        if (session && session->receiving && reinterpret_cast<HWND>(lparam) == session->receiving &&
            HIWORD(wparam) == BN_CLICKED) {
            auto text = std::wstring(L"Clicks received: ") + std::to_wstring(++session->clicks);
            SetDlgItemTextW(dialog, IDC_PRACTICE_STATUS, text.c_str());
            return TRUE;
        }
        break;
    case WM_CLOSE:
        EndDialog(dialog, 0);
        return TRUE;
    case WM_DESTROY:
        if (session) session->measurement.Cancel();
        break;
    }
    return FALSE;
}

inline bool Run(HWND owner, Settings settings = {}) {
    Session session{settings};
    return DialogBoxParamW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDD_CLICK_PRACTICE),
                           owner, Procedure, reinterpret_cast<LPARAM>(&session)) != -1;
}
} // namespace click_practice
