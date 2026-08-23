#include "CinnamonBackend.h"

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

bool CinnamonBackend::setWallpaper(const WallpaperFrame &frame, std::string &error)
{
    const std::string uri = "file://" + frame.getImagePath();
    const std::string command =
        "gsettings set org.cinnamon.desktop.background picture-uri " + escapeShellArg(uri);

    const int result = std::system(command.c_str());
    if (result != 0)
    {
        error = "gsettings failed with exit code " + std::to_string(result);
        return false;
    }

    return true;
}

std::string CinnamonBackend::getBackendName() const
{
    return "Cinnamon (gsettings)";
}

bool CinnamonBackend::isAvailable(std::string &error) const
{
    const int result = std::system("command -v gsettings >/dev/null 2>&1");
    if (result != 0)
    {
        error = "Cinnamon backend requires 'gsettings', but it was not found in PATH.";
        return false;
    }
    return true;
}
