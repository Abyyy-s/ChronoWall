#include "DynamicWallpaper.h"

DynamicWallpaper::DynamicWallpaper()
    : startTimeSeconds(0)
{
}

void DynamicWallpaper::setFrames(
    const std::vector<WallpaperFrame> &frames)
{
    this->frames = frames;
}

const std::vector<WallpaperFrame> &
DynamicWallpaper::getFrames() const
{
    return frames;
}

void DynamicWallpaper::setStartTime(int seconds)
{
    startTimeSeconds = seconds;
}

int DynamicWallpaper::getStartTime() const
{
    return startTimeSeconds;
}
