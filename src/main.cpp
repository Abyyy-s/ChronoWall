#include "DynamicWallpaper.h"
#include "WallpaperChanger.h"
#include "WallpaperParser.h"
#include "WallpaperScheduler.h"

#include <chrono>
#include <csignal>
#include <cerrno>
#include <iostream>
#include <time.h>

namespace
{
volatile std::sig_atomic_t shutdownRequested = 0;

void handleShutdownSignal(int)
{
    shutdownRequested = 1;
}

int getElapsedSeconds(const DynamicWallpaper &wallpaper)
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t currentTime =
        std::chrono::system_clock::to_time_t(now);

    const std::tm *localTime = std::localtime(&currentTime);

    int elapsedSeconds =
        localTime->tm_hour * 3600 +
        localTime->tm_min * 60 +
        localTime->tm_sec;

    elapsedSeconds -= wallpaper.getStartTime();

    if (elapsedSeconds < 0)
        elapsedSeconds += 24 * 60 * 60;

    return elapsedSeconds;
}

void sleepUntilOrShutdown(double seconds)
{
    if (seconds <= 0.0)
        return;

    timespec remaining{};
    remaining.tv_sec = static_cast<time_t>(seconds);
    remaining.tv_nsec = static_cast<long>(
        (seconds - remaining.tv_sec) * 1'000'000'000.0);

    while (!shutdownRequested && nanosleep(&remaining, &remaining) == -1)
    {
        if (errno != EINTR)
            break;
    }
}
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./ChronoWall <wallpaper.xml>\n";
        return 1;
    }

    std::signal(SIGINT, handleShutdownSignal);
    std::signal(SIGTERM, handleShutdownSignal);

    WallpaperParser parser;
    DynamicWallpaper wallpaper = parser.parse(argv[1]);

    WallpaperScheduler scheduler;
    WallpaperChanger changer;

    std::cout << "ChronoWall daemon started.\n";

    while (!shutdownRequested)
    {
        const int elapsedSeconds = getElapsedSeconds(wallpaper);
        const TimelineEvent *event =
            scheduler.getCurrentEvent(wallpaper, elapsedSeconds);

        if (!event)
        {
            std::cerr << "No event found.\n";
            break;
        }

        if (event->getType() == TimelineEventType::Static)
        {
            const WallpaperFrame &frame =
                wallpaper.getFrames()[event->getFrameIndex()];

            changer.setWallpaper(frame);
        }
        else
        {
            // Keep transitions lightweight for now: immediately select the
            // destination image through the normal desktop wallpaper API.
            // The scheduler still honours the transition duration; there is
            // no rendering window, GPU loop, or extra desktop surface.
            const Transition &transition =
                wallpaper.getTransitions()[event->getTransitionIndex()];

            const WallpaperFrame *destination = nullptr;
            for (const auto &frame : wallpaper.getFrames())
            {
                if (frame.getImagePath() == transition.getToImage())
                {
                    destination = &frame;
                    break;
                }
            }

            if (!destination)
            {
                std::cerr << "Could not find transition destination: "
                          << transition.getToImage() << '\n';
                break;
            }

            changer.setWallpaper(*destination);
        }

        const double remaining =
            static_cast<double>(event->getEndTime()) -
            static_cast<double>(elapsedSeconds);

        sleepUntilOrShutdown(remaining);
    }

    std::cout << "ChronoWall stopped.\n";
    return 0;
}
