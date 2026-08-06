#include "WallpaperParser.h"
#include "WallpaperScheduler.h"
#include "WallpaperChanger.h"

#include <iostream>

int main()
{
    WallpaperParser parser;

    auto frames = parser.parse("../MagicLake.xml");

    WallpaperScheduler scheduler;

    auto currentWallpaper =
        scheduler.getCurrentWallpaper(frames, 20000);

    if (!currentWallpaper)
    {
        std::cout << "No wallpaper found.\n";
        return 1;
    }

    WallpaperChanger changer;

    changer.setWallpaper(*currentWallpaper);

    std::cout << "Wallpaper changed successfully!\n";

    return 0;
}
