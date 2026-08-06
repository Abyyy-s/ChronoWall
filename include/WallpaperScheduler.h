#ifndef WALLPAPERSCHEDULER_H
#define WALLPAPERSCHEDULER_H

#include <vector>
#include "WallpaperFrame.h"

class WallpaperScheduler
{
public:
    WallpaperFrame getCurrentWallpaper(
        const std::vector<WallpaperFrame> &frames,
        int elapsedSeconds);
};

#endif
