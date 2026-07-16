#include "WallpaperFrame.h"

WallpaperFrame::WallpaperFrame(std::string imagePath, int duration)
    : imagePath(imagePath), duration(duration)
{
}

std::string WallpaperFrame::getImagePath()
{
    return imagePath;
}

int WallpaperFrame::getDuration()
{
    return duration;
}
