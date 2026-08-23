#include "WallpaperChanger.h"
#include "WallpaperBackendFactory.h"

#include <iostream>

WallpaperChanger::WallpaperChanger()
    : backend(nullptr)
{
}

WallpaperChanger::WallpaperChanger(std::unique_ptr<WallpaperBackend> customBackend)
    : backend(std::move(customBackend))
{
}

bool WallpaperChanger::initialize(std::string &error)
{
    if (backend != nullptr)
    {
        return true;
    }

    backend = WallpaperBackendFactory::createBackend(error);
    return backend != nullptr;
}

bool WallpaperChanger::setWallpaper(const WallpaperFrame &frame)
{
    if (!backend)
    {
        std::string error;
        if (!initialize(error))
        {
            std::cerr << "Warning: failed to set wallpaper (no backend available): "
                      << error << "\n";
            return false;
        }
    }

    std::string error;
    if (!backend->setWallpaper(frame, error))
    {
        std::cerr << "Warning: failed to set wallpaper (" << frame.getImagePath()
                  << "): " << error << "\n";
        return false;
    }

    return true;
}

std::string WallpaperChanger::getBackendName() const
{
    return backend ? backend->getBackendName() : "None";
}

bool WallpaperChanger::isReady() const
{
    return backend != nullptr;
}
