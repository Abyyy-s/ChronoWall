#include "KDEPlasmaBackend.h"

#include <cstdlib>
#include <string>

namespace
{
std::string escapeShellArg(const std::string &input)
{
    std::string escaped = "'";
    for (char c : input)
    {
        if (c == '\'')
        {
            escaped += "'\\''";
        }
        else
        {
            escaped += c;
        }
    }
    escaped += "'";
    return escaped;
}
}

bool KDEPlasmaBackend::setWallpaper(const WallpaperFrame &frame, std::string &error)
{
    const std::string command =
        "plasma-apply-wallpaperimage " + escapeShellArg(frame.getImagePath());

    const int result = std::system(command.c_str());
    if (result != 0)
    {
        error = "plasma-apply-wallpaperimage failed with exit code " + std::to_string(result);
        return false;
    }

    return true;
}

std::string KDEPlasmaBackend::getBackendName() const
{
    return "KDE Plasma (plasma-apply-wallpaperimage)";
}

bool KDEPlasmaBackend::isAvailable(std::string &error) const
{
    const int result = std::system("command -v plasma-apply-wallpaperimage >/dev/null 2>&1");
    if (result != 0)
    {
        error = "KDE Plasma backend requires 'plasma-apply-wallpaperimage' (provided by plasma-workspace), but it was not found in PATH.";
        return false;
    }
    return true;
}
