#ifndef WALLPAPERFRAME_H
#define WALLPAPERFRAME_H

#include <string>

class WallpaperFrame
{
private:
    std::string imagePath;
    double duration;

public:
    WallpaperFrame(std::string imagePath, double duration);

    std::string getImagePath();
    double getDuration();
};

#endif
