#include "WallpaperChanger.h"

#include <cstdlib>
#include <string>

void WallpaperChanger::setWallpaper(const WallpaperFrame &frame)
{
    std::string command =
        "gsettings set org.cinnamon.desktop.background picture-uri \"file://" + frame.getImagePath() +
        "\"";

    std::system(command.c_str());
}
