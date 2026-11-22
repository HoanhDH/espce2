#pragma once

#include <windows.h>
#include <cmath>
#include <limits>
#include <optional>
#include <vector>

namespace click_practice {

class MouseMeasurement {
public:
    MouseMeasurement() = default;
    MouseMeasurement(const MouseMeasurement&) = delete;
    MouseMeasurement& operator=(const MouseMeasurement&) = delete;
    ~MouseMeasurement() { Cancel(); }

    bool Active() const { return dialog_ != nullptr; }

    bool Start(HWND dialog) {
        if (!dialog || Active() || GetForegroundWindow() != dialog || !MouseRegistrationFree())
            return false;

        const RAWINPUTDEVICE mouse{genericDesktop, mouseUsage, 0, dialog};
        if (!RegisterRawInputDevices(&mouse, 1, sizeof(mouse)))
            return false;

        dialog_ = dialog;
        device_ = nullptr;
        mixedDevices_ = false;
        horizontal_ = vertical_ = 0;
        return true;
    }

    void Add(HRAWINPUT input) {
        if (!Active() || GetForegroundWindow() != dialog_)
            return;

        RAWINPUT raw{};
        UINT size = sizeof(raw);
        if (GetRawInputData(input, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER)) != sizeof(raw) ||
            raw.header.dwType != RIM_TYPEMOUSE || !raw.header.hDevice ||
            (raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
            return;

        const auto& mouse = raw.data.mouse;
        if (!mouse.lLastX && !mouse.lLastY)
            return;
        if (device_ && device_ != raw.header.hDevice) {
            mixedDevices_ = true;
            return;
        }
        device_ = raw.header.hDevice;
        horizontal_ += mouse.lLastX;
        vertical_ += mouse.lLastY;
    }

    std::optional<unsigned> Finish() {
        const bool valid = Active() && GetForegroundWindow() == dialog_ && device_ && !mixedDevices_;
        Cancel();
        const double travel = std::abs(horizontal_);
        // The user moves 10 cm horizontally. Reject predominantly diagonal strokes.
        if (!valid || std::abs(vertical_) > travel * 0.25)
            return std::nullopt;
        const double dpi = std::round(travel * 2.54 / 10.0);
        if (dpi < 1 || dpi > (std::numeric_limits<unsigned>::max)())
            return std::nullopt;
        return static_cast<unsigned>(dpi);
    }

    void Cancel() {
        if (Active()) {
            const RAWINPUTDEVICE remove{genericDesktop, mouseUsage, RIDEV_REMOVE, nullptr};
            RegisterRawInputDevices(&remove, 1, sizeof(remove));
            dialog_ = nullptr;
        }
    }

private:
    static constexpr USHORT genericDesktop = 1;
    static constexpr USHORT mouseUsage = 2;

    static bool MouseRegistrationFree() {
        UINT count = 0;
        if (GetRegisteredRawInputDevices(nullptr, &count, sizeof(RAWINPUTDEVICE)) != 0)
            return false;
        if (!count)
            return true;
        std::vector<RAWINPUTDEVICE> devices(count);
        const UINT result = GetRegisteredRawInputDevices(devices.data(), &count, sizeof(RAWINPUTDEVICE));
        if (result == static_cast<UINT>(-1))
            return false;
        for (UINT i = 0; i < result; ++i) {
            const auto& device = devices[i];
            // A page-only registration can also own the mouse.
            if (device.usUsagePage == genericDesktop && (device.usUsage == mouseUsage || device.usUsage == 0))
                return false;
        }
        return true;
    }

    HWND dialog_ = nullptr;
    HANDLE device_ = nullptr;
    bool mixedDevices_ = false;
    double horizontal_ = 0;
    double vertical_ = 0;
};
} // namespace click_practice
