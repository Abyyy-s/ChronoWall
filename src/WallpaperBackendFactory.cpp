#include "WallpaperBackendFactory.h"
#include "CinnamonBackend.h"
#include "KDEPlasmaBackend.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace
{
std::string trim(const std::string &value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return {};

    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

std::string toLower(const std::string &value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return result;
}

bool containsCaseInsensitive(const std::string &haystack, const std::string &needle)
{
    const std::string lowerHaystack = toLower(haystack);
    const std::string lowerNeedle = toLower(needle);
    return lowerHaystack.find(lowerNeedle) != std::string::npos;
}

const char *getEnvOrEmpty(const char *name)
{
    const char *val = std::getenv(name);
    return val ? val : "";
}
}

std::unique_ptr<WallpaperBackend> WallpaperBackendFactory::createBackendByName(
    const std::string &name,
    std::string &error)
{
    const std::string normalized = toLower(trim(name));
    std::unique_ptr<WallpaperBackend> backend;

    if (normalized == "kde" || normalized == "plasma")
    {
        backend = std::make_unique<KDEPlasmaBackend>();
    }
    else if (normalized == "cinnamon")
    {
        backend = std::make_unique<CinnamonBackend>();
    }
    else
    {
        error = "Unknown wallpaper backend: '" + name + "'. Supported backends: 'kde', 'cinnamon'.";
        return nullptr;
    }

    if (!backend->isAvailable(error))
    {
        return nullptr;
    }

    return backend;
}

std::unique_ptr<WallpaperBackend> WallpaperBackendFactory::createBackend(std::string &error)
{
    // 1. Explicit override via CHRONOWALL_BACKEND
    const char *overrideEnv = std::getenv("CHRONOWALL_BACKEND");
    if (overrideEnv && *overrideEnv)
    {
        const std::string overrideName = trim(overrideEnv);
        if (!overrideName.empty())
        {
            return createBackendByName(overrideName, error);
        }
    }

    // 2. Desktop session environment detection
    const std::string currentDesktop = getEnvOrEmpty("XDG_CURRENT_DESKTOP");
    const std::string sessionDesktop = getEnvOrEmpty("XDG_SESSION_DESKTOP");
    const std::string desktopSession = getEnvOrEmpty("DESKTOP_SESSION");

    if (containsCaseInsensitive(currentDesktop, "kde") ||
        containsCaseInsensitive(currentDesktop, "plasma") ||
        containsCaseInsensitive(sessionDesktop, "kde") ||
        containsCaseInsensitive(sessionDesktop, "plasma") ||
        containsCaseInsensitive(desktopSession, "plasma"))
    {
        return createBackendByName("kde", error);
    }

    if (containsCaseInsensitive(currentDesktop, "cinnamon") ||
        containsCaseInsensitive(sessionDesktop, "cinnamon") ||
        containsCaseInsensitive(desktopSession, "cinnamon"))
    {
        return createBackendByName("cinnamon", error);
    }

    // 3. Deterministic explicit failure (no heuristic guessing)
    error = "Unsupported or undetected desktop environment";
    if (!currentDesktop.empty())
    {
        error += " (XDG_CURRENT_DESKTOP='" + currentDesktop + "')";
    }
    error += ". Supported backends: 'kde', 'cinnamon'. Set CHRONOWALL_BACKEND=<kde|cinnamon> to override.";
    return nullptr;
}
