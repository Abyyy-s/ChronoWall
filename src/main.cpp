#include "DynamicWallpaper.h"
#include "IDesktopSurface.h"
#include "TransitionRenderer.h"
#include "WallpaperChanger.h"
#include "WallpaperParser.h"
#include "WallpaperScheduler.h"
#include "X11DesktopSurface.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

namespace
{
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

const WallpaperFrame *findFrame(
    const DynamicWallpaper &wallpaper,
    const std::string &imagePath)
{
    for (const auto &frame : wallpaper.getFrames())
    {
        if (frame.getImagePath() == imagePath)
            return &frame;
    }

    return nullptr;
}
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./ChronoWall <wallpaper.xml>\n";
        return 1;
    }

    WallpaperParser parser;
    DynamicWallpaper wallpaper = parser.parse(argv[1]);

    WallpaperScheduler scheduler;
    WallpaperChanger changer;

    X11DesktopSurface surface;

    if (!surface.init())
    {
        std::cerr << "Desktop surface initialization failed.\n";
        return 1;
    }

    TransitionRenderer renderer;

    if (!renderer.init(surface))
    {
        std::cerr << "Transition renderer initialization failed.\n";
        surface.shutdown();
        return 1;
    }

    std::cout << "ChronoWall daemon started.\n";

    bool running = true;

    while (running)
    {
        const int elapsedSeconds = getElapsedSeconds(wallpaper);

        const TimelineEvent *event =
            scheduler.getCurrentEvent(
                wallpaper,
                elapsedSeconds);

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
            surface.hide();

            const double remaining =
                static_cast<double>(event->getEndTime()) -
                static_cast<double>(elapsedSeconds);

            if (remaining > 0.0)
            {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(remaining));
            }
        }
        else
        {
            const Transition &transition =
                wallpaper.getTransitions()[event->getTransitionIndex()];

            surface.show();

            if (!renderer.render(transition))
            {
                std::cerr << "Transition rendering failed.\n";
                running = false;
                continue;
            }

            // The SDL surface is still showing the final frame here.
            // Hand the exact same image to the desktop first, then hide
            // the surface so there is no visible gap at the handoff.
            const WallpaperFrame *finalFrame =
                findFrame(wallpaper, transition.getToImage());

            if (!finalFrame)
            {
                std::cerr
                    << "Could not find final transition frame: "
                    << transition.getToImage() << '\n';
                running = false;
                continue;
            }

            changer.setWallpaper(*finalFrame);
            surface.hide();
        }
    }

    surface.hide();
    renderer.shutdown();
    surface.shutdown();

    return running ? 0 : 1;
}
