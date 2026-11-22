#pragma once

#include <windows.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>
#include <optional>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#pragma comment(lib, "advapi32.lib")
#endif

namespace click_practice {

inline std::optional<double> ReadSensitivityFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("sensitivity") == std::string::npos) continue;
        std::istringstream fields(line);
        std::string key, value, trailing;
        if (!(fields >> std::quoted(key)) || key != "sensitivity") continue;
        if (!(fields >> std::quoted(value)) || ((fields >> trailing) && !trailing.starts_with("//")))
            return std::nullopt;

        std::istringstream number(value);
        number.imbue(std::locale::classic());
        double result;
        if (!(number >> std::noskipws >> result) || !number.eof() || !std::isfinite(result) || result <= 0)
            return std::nullopt;
        return result;
    }
    return std::nullopt;
}

inline std::optional<double> ReadSavedSensitivity() {
    constexpr auto steamKey = L"Software\\Valve\\Steam";
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, steamKey, L"SteamPath", RRF_RT_REG_SZ,
                     nullptr, nullptr, &bytes) != ERROR_SUCCESS || bytes < sizeof(wchar_t))
        return std::nullopt;
    std::wstring steamPath(bytes / sizeof(wchar_t), L'\0');
    if (RegGetValueW(HKEY_CURRENT_USER, steamKey, L"SteamPath", RRF_RT_REG_SZ,
                     nullptr, steamPath.data(), &bytes) != ERROR_SUCCESS || steamPath.front() == L'\0')
        return std::nullopt;
    const auto userdata = std::filesystem::path(steamPath.c_str()) / L"userdata";

    DWORD activeUser = 0;
    bytes = sizeof(activeUser);
    std::filesystem::path profile;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", L"ActiveUser",
                     RRF_RT_REG_DWORD, nullptr, &activeUser, &bytes) == ERROR_SUCCESS && activeUser) {
        profile = userdata / std::to_wstring(activeUser);
    } else {
        // With Steam signed out, use a profile only if exactly one account is present.
        std::error_code error;
        for (std::filesystem::directory_iterator it(userdata, error), end; !error && it != end; it.increment(error)) {
            const auto name = it->path().filename().wstring();
            if (name == L"0" || name.find_first_not_of(L"0123456789") != std::wstring::npos) continue;
            const bool directory = it->is_directory(error);
            if (error) return std::nullopt;
            if (!directory) continue;
            if (!profile.empty()) return std::nullopt;
            profile = it->path();
        }
        if (error || profile.empty()) return std::nullopt;
    }
    return ReadSensitivityFile(profile / L"730/local/cfg/cs2_user_convars_0_slot0.vcfg");
}

} // namespace click_practice
