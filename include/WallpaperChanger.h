#ifndef WALLPAPERCHANGER_H
#define WALLPAPERCHANGER_H

#include "WallpaperBackend.h"
#include "WallpaperFrame.h"

#include <memory>
#include <string>

class WallpaperChanger
{
public:
    WallpaperChanger();
    explicit WallpaperChanger(std::unique_ptr<WallpaperBackend> backend);

    bool initialize(std::string &error);
    bool setWallpaper(const WallpaperFrame &frame);
    std::string getBackendName() const;
    bool isReady() const;

private:
    std::unique_ptr<WallpaperBackend> backend;
};

#endif
