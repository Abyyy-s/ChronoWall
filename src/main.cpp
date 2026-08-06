#include "WallpaperParser.h"
#include "WallpaperScheduler.h"
#include <iostream>

int main()
{
    WallpaperParser parser;

    auto frames = parser.parse("../MagicLake.xml");

    WallpaperScheduler scheduler;

    auto currentWallpaper =
        scheduler.getCurrentWallpaper(frames, 20000);

    if (currentWallpaper)
    {
        std::cout << "Current Wallpaper\n";
        std::cout << "Image: "
                  << currentWallpaper->getImagePath()
                  << '\n';

        std::cout << "Duration: "
                  << currentWallpaper->getDuration()
                  << '\n';
    }
    else
    {
        std::cout << "No wallpaper found.\n";
    }

    return 0;
}
