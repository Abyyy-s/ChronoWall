#include "WallpaperChanger.h"

#include <cstdlib>
#include <iostream>
#include <string>

void WallpaperChanger::setWallpaper(const WallpaperFrame &frame)
{
    std::string command =
        "gsettings set org.cinnamon.desktop.background picture-uri \"file://" + frame.getImagePath() +
        "\"";

    const int result = std::system(command.c_str());
    if (result != 0)
    {
        std::cerr << "Warning: failed to set wallpaper: "
                  << frame.getImagePath() << "\n";
    }
}
