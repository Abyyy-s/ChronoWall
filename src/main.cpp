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

    const TimelineEvent *event =
        scheduler.getCurrentEvent(
            wallpaper,
            elapsedSeconds);

    if (event == nullptr)
    {
        std::cout << "No event found.\n";
        return 1;
    }

    if (event->getType() == TimelineEventType::Static)
    {
        const WallpaperFrame &frame =
            wallpaper.getFrames()[event->getFrameIndex()];


        WallpaperChanger changer;

        changer.setWallpaper(frame);
    }
    else
    {
        const Transition &transition =
            wallpaper.getTransitions()[event->getTransitionIndex()];

    }

    std::cout << "Wallpaper changed successfully!\n";

    return 0;
}
