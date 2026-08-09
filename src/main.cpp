#include "DynamicWallpaper.h"
#include "WallpaperChanger.h"
#include "WallpaperParser.h"
#include "WallpaperScheduler.h"

#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <time.h>

namespace
{
constexpr const char *VERSION = "1.0.0";
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
        (seconds - static_cast<double>(remaining.tv_sec)) * 1'000'000'000.0);

    while (!shutdownRequested && nanosleep(&remaining, &remaining) == -1)
    {
        if (errno != EINTR)
            break;
    }
}

void printUsage()
{
    std::cout
        << "ChronoWall " << VERSION << "\n\n"
        << "Usage:\n"
        << "  chronowall run <wallpaper.xml>  Run the daemon in the foreground\n"
        << "  chronowall start                Start the installed user service\n"
        << "  chronowall stop                 Stop the installed user service\n"
        << "  chronowall restart              Restart the installed user service\n"
        << "  chronowall status               Show service status\n"
        << "  chronowall --version            Show version\n"
        << "  chronowall --help               Show this help\n";
}

int runServiceCommand(const char *command)
{
    std::string shellCommand = "systemctl --user ";
    shellCommand += command;
    shellCommand += " chronowall.service";
    return std::system(shellCommand.c_str());
}

int runDaemon(const char *xmlPath)
{
    if (!std::filesystem::exists(xmlPath))
    {
        std::cerr << "Error: XML file not found: " << xmlPath << '\n';
        return 1;
    }

    WallpaperParser parser;
    DynamicWallpaper wallpaper = parser.parse(xmlPath);

    if (wallpaper.getTimeline().empty())
    {
        std::cerr << "Error: no valid timeline events were loaded from: "
                  << xmlPath << '\n';
        return 1;
    }

    WallpaperScheduler scheduler;
    WallpaperChanger changer;

    std::signal(SIGINT, handleShutdownSignal);
    std::signal(SIGTERM, handleShutdownSignal);

    std::cout << "ChronoWall " << VERSION << " daemon started.\n"
              << "Wallpaper: " << xmlPath << '\n';

    while (!shutdownRequested)
    {
        const int elapsedSeconds = getElapsedSeconds(wallpaper);
        const TimelineEvent *event =
            scheduler.getCurrentEvent(wallpaper, elapsedSeconds);

        if (!event)
        {
            std::cerr << "Error: no timeline event found.\n";
            return 1;
        }

        if (event->getType() == TimelineEventType::Static)
        {
            const WallpaperFrame &frame =
                wallpaper.getFrames()[event->getFrameIndex()];

            changer.setWallpaper(frame);
            std::cout << "[static] " << frame.getImagePath() << '\n';
        }
        else
        {
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
                std::cerr << "Error: transition destination not found: "
                          << transition.getToImage() << '\n';
                return 1;
            }

            changer.setWallpaper(*destination);
            std::cout << "[transition] " << transition.getFromImage()
                      << " -> " << transition.getToImage()
                      << " (" << transition.getDuration() << "s)\n";
        }

        const double remaining =
            static_cast<double>(event->getEndTime()) -
            static_cast<double>(elapsedSeconds);

        sleepUntilOrShutdown(remaining);
    }

    std::cout << "ChronoWall stopped.\n";
    return 0;
}
}

int main(int argc, char *argv[])
{
    if (argc == 1 || std::string(argv[1]) == "--help" ||
        std::string(argv[1]) == "-h")
    {
        printUsage();
        return argc == 1 ? 1 : 0;
    }

    const std::string command = argv[1];

    if (command == "--version" || command == "-v")
    {
        std::cout << "ChronoWall " << VERSION << '\n';
        return 0;
    }

    if (command == "run")
    {
        if (argc != 3)
        {
            std::cerr << "Usage: chronowall run <wallpaper.xml>\n";
            return 1;
        }
        return runDaemon(argv[2]);
    }

    if (command == "start" || command == "stop" ||
        command == "restart" || command == "status")
    {
        return runServiceCommand(command.c_str());
    }

    // Backwards-compatible shorthand: chronowall <wallpaper.xml>
    if (argc == 2 && command.size() >= 4 &&
        command.substr(command.size() - 4) == ".xml")
    {
        return runDaemon(argv[1]);
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    printUsage();
    return 1;
}
