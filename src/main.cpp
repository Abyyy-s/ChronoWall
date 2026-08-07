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

    std::cout << "\nTransitions\n";
    std::cout << "====================\n";

    for (const Transition &transition : wallpaper.getTransitions())
    {
        std::cout << "From: "
                  << transition.getFromImage()
                  << '\n';

        std::cout << "To: "
                  << transition.getToImage()
                  << '\n';

        std::cout << "Duration: "
                  << transition.getDuration()
                  << '\n';

        std::cout << "---------------------\n";
    }

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

    if (event->getType() == TimelineEventType::Static)
    {
        const WallpaperFrame &frame =
            wallpaper.getFrames()[event->getFrameIndex()];

        std::cout << "Selected Image: "
                  << frame.getImagePath()
                  << '\n';

        std::cout << "Duration: "
                  << frame.getDuration()
                  << '\n';

        WallpaperChanger changer;

        changer.setWallpaper(frame);
    }
    else
    {
        const Transition &transition =
            wallpaper.getTransitions()[event->getTransitionIndex()];

        std::cout << "Transition\n";

        std::cout << "From: "
                  << transition.getFromImage()
                  << '\n';

        std::cout << "To: "
                  << transition.getToImage()
                  << '\n';

        std::cout << "Duration: "
                  << transition.getDuration()
                  << '\n';
    }

    std::cout << "Wallpaper changed successfully!\n";

    return 0;
}
