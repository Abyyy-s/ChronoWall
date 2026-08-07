#ifndef WALLPAPERSCHEDULER_H
#define WALLPAPERSCHEDULER_H

#include "DynamicWallpaper.h"
#include "TimelineEvent.h"

class WallpaperScheduler
{
public:
    const TimelineEvent *getCurrentEvent(
        const DynamicWallpaper &wallpaper,
        double elapsedSeconds);
};

#endif
