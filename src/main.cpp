#include "DynamicWallpaper.h"
#include "WallpaperChanger.h"
#include "WallpaperParser.h"
#include "WallpaperScheduler.h"

#include <chrono>
#include <ctime>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./ChronoWall <wallpaper.xml>\n";
        return 1;
    }

    std::string xmlPath = argv[1];

    WallpaperParser parser;

    DynamicWallpaper wallpaper =
        parser.parse(xmlPath);

    auto now = std::chrono::system_clock::now();

    std::time_t currentTime =
        std::chrono::system_clock::to_time_t(now);

    std::tm *localTime =
        std::localtime(&currentTime);

    int elapsedSeconds =
        localTime->tm_hour * 3600 +
        localTime->tm_min * 60 +
        localTime->tm_sec;

    // Account for the wallpaper's start time
    elapsedSeconds -= wallpaper.getStartTime();

    // If the current time is before the start time,
    // wrap around to the previous day.
    if (elapsedSeconds < 0)
    {
        elapsedSeconds += 24 * 60 * 60;
    }

    WallpaperScheduler scheduler;

    auto currentWallpaper =
        scheduler.getCurrentWallpaper(
            wallpaper.getFrames(),
            elapsedSeconds);

    if (!currentWallpaper)
    {
        std::cout << "No wallpaper found.\n";
        return 1;
    }

    std::cout << "Current Time: "
              << localTime->tm_hour << ":"
              << localTime->tm_min << ":"
              << localTime->tm_sec << '\n';

    std::cout << "Start Time (seconds): "
              << wallpaper.getStartTime()
              << '\n';

    std::cout << "Elapsed Seconds: "
              << elapsedSeconds
              << '\n';

    std::cout << "Selected Image: "
              << currentWallpaper->getImagePath()
              << '\n';

    std::cout << "Duration: "
              << currentWallpaper->getDuration()
              << '\n';

    WallpaperChanger changer;

    changer.setWallpaper(*currentWallpaper);

    std::cout << "Wallpaper changed successfully!\n";

    return 0;
}
