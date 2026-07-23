#include "WallpaperFrame.h"

WallpaperFrame::WallpaperFrame(std::string imagePath, double duration)
    : imagePath(imagePath), duration(duration)
{
}

std::string WallpaperFrame::getImagePath() const
{
    return imagePath;
}

double WallpaperFrame::getDuration() const
{
    return duration;
}
