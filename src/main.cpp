#include "DynamicWallpaper.h"
#include "IDesktopSurface.h"
#include "TransitionRenderer.h"
#include "WallpaperChanger.h"
#include "WallpaperParser.h"
#include "WallpaperScheduler.h"
#include "X11DesktopSurface.h"

#include <algorithm>
#include <chrono>
#include <csignal>
#include <ctime>
#include <cerrno>
#include <iostream>
#include <string>
#include <time.h>

namespace
{
volatile std::sig_atomic_t shutdownRequested = 0;

constexpr double LIVE_TRANSITION_THRESHOLD = 20.0;

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

double getPreBlendInterval(double duration)
{
    if (duration <= 60.0)
        return 2.0;

    if (duration <= 300.0)
        return 5.0;

    return 15.0;
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

    while (running && !shutdownRequested)
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
            surface.hide();

            const double remaining =
                static_cast<double>(event->getEndTime()) -
                static_cast<double>(elapsedSeconds);

            sleepUntilOrShutdown(remaining);
            continue;
        }

        const Transition &transition =
            wallpaper.getTransitions()[event->getTransitionIndex()];

        const double duration = std::max(0.001, transition.getDuration());

        if (duration < LIVE_TRANSITION_THRESHOLD)
        {
            // Mode A: short transition, full GPU animation.
            surface.show();

            if (!renderer.render(transition, &shutdownRequested))
            {
                if (!shutdownRequested)
                    std::cerr << "Live transition rendering failed.\n";
                running = false;
                continue;
            }

            if (shutdownRequested)
                break;

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

            // The SDL surface is still showing the final frame. Hand the
            // same image to the desktop before hiding the surface.
            changer.setWallpaper(*finalFrame);
            surface.hide();
        }
        else
        {
            // Mode B: long transition. Generate one blended frame at a time
            // and hand it to the normal desktop wallpaper API. No live SDL
            // surface is shown, so Nemo/icons remain untouched.
            const double interval = getPreBlendInterval(duration);
            const auto start = std::chrono::steady_clock::now();

            while (!shutdownRequested)
            {
                const auto now = std::chrono::steady_clock::now();
                const double elapsed =
                    std::chrono::duration<double>(now - start).count();
                const double progress =
                    std::clamp(elapsed / duration, 0.0, 1.0);

                const std::string framePath =
                    renderer.preBlendFrame(transition, progress);

                if (framePath.empty())
                {
                    std::cerr << "Offline transition rendering failed.\n";
                    running = false;
                    break;
                }

                changer.setWallpaper(WallpaperFrame(framePath, 0));

                if (progress >= 1.0)
                    break;

                const double remaining = duration - elapsed;
                sleepUntilOrShutdown(std::min(interval, remaining));
            }
        }
    }

    surface.hide();
    renderer.shutdown();
    surface.shutdown();

    if (shutdownRequested)
        std::cout << "ChronoWall stopped cleanly.\n";

    return running ? 0 : 1;
}
