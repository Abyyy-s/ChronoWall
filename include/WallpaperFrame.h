#ifndef WALLPAPERFRAME_H
#define WALLPAPERFRAME_H

#include <string>

class WallpaperFrame
{
private:
    std::string imagePath;
    double duration;

public:
    WallpaperFrame(std::string imagePath, int duration);

    std::string getImagePath();
    int getDuration();
};

#endif
