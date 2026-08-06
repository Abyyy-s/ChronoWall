#ifndef WALLPAPERSCHEDULER_H
#define WALLPAPERSCHEDULER_H

#include <vector>
#include <optional>
#include "WallpaperFrame.h"

class WallpaperScheduler
{
public:
    std::optional<WallpaperFrame> getCurrentWallpaper(
        const std::vector<WallpaperFrame> &frames,
        int elapsedSeconds);
};

#endif
