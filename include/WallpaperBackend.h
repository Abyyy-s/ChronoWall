#ifndef WALLPAPERBACKEND_H
#define WALLPAPERBACKEND_H

#include "WallpaperFrame.h"
#include <string>

class WallpaperBackend
{
public:
    virtual ~WallpaperBackend() = default;

    virtual bool setWallpaper(const WallpaperFrame &frame, std::string &error) = 0;
    virtual std::string getBackendName() const = 0;
    virtual bool isAvailable(std::string &error) const = 0;
};

#endif
