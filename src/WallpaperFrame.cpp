#include "WallpaperFrame.h"

WallpaperFrame::WallpaperFrame(std::string imagePath, int duration)
    : imagePath(imagePath), duration(duration)
{
}

std::string WallpaperFrame::getImagePath() const
{
    return imagePath;
}

int WallpaperFrame::getDuration() const
{
    return duration;
}
