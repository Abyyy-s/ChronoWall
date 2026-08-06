#ifndef WALLPAPERFRAME_H
#define WALLPAPERFRAME_H

#include <string>

class WallpaperFrame
{
private:
    std::string imagePath;
    int duration;

public:
    WallpaperFrame(std::string imagePath, int duration);

    std::string getImagePath() const;
    int getDuration() const;
};

#endif
