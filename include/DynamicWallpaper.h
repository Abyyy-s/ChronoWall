#ifndef DYNAMICWALLPAPER_H
#define DYNAMICWALLPAPER_H

#include <vector>
#include "WallpaperFrame.h"

class DynamicWallpaper
{
private:
    std::vector<WallpaperFrame> frames;

    int startTimeSeconds;

public:
    DynamicWallpaper();

    void setFrames(const std::vector<WallpaperFrame> &frames);

    const std::vector<WallpaperFrame> &getFrames() const;

    void setStartTime(int seconds);

    int getStartTime() const;
};

#endif
