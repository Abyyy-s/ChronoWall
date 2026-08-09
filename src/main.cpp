#include "DynamicWallpaper.h"
#include "WallpaperChanger.h"
#include "WallpaperLibrary.h"
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
    {
        // Never allow the daemon to spin if an event boundary has already
        // been reached due to clock granularity or rounding.
        seconds = 0.05;
    }

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
        << "  chronowall list                 List available wallpapers\n"
        << "  chronowall set <name>            Switch to a wallpaper\n"
        << "  chronowall current              Show the current wallpaper\n"
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

std::string joinArguments(int argc, char *argv[], int first)
{
    std::string result;
    for (int index = first; index < argc; ++index)
    {
        if (!result.empty())
            result += ' ';
        result += argv[index];
    }
    return result;
}

int listWallpapers()
{
    WallpaperLibrary library;
    if (!library.isConfigured())
    {
        std::cerr << "Error: wallpaper library is not configured.\n"
                  << "Install a wallpaper with install.sh first.\n";
        return 1;
    }

    const auto wallpapers = library.list();
    if (wallpapers.empty())
    {
        std::cout << "No wallpapers found in the configured library.\n";
        return 0;
    }

    std::cout << "Available wallpapers (" << wallpapers.size() << "):\n\n";
    for (const auto &name : wallpapers)
        std::cout << "  " << name << '\n';

    return 0;
}

int setWallpaper(const std::string &name)
{
    WallpaperLibrary library;
    std::string error;

    std::cout << "Switching wallpaper to " << name << "...\n";

    if (!library.setWallpaper(name, error))
    {
        std::cerr << "Error: " << error << '\n';
        return 1;
    }

    if (runServiceCommand("restart") != 0)
    {
        std::cerr << "Error: wallpaper was installed, but the ChronoWall service could not be restarted.\n";
        return 1;
    }

    std::cout << "✓ Wallpaper changed to " << library.current() << '\n';
    return 0;
}

int showCurrentWallpaper()
{
    WallpaperLibrary library;
    const std::string current = library.current();

    if (current.empty())
    {
        std::cerr << "No current wallpaper is recorded.\n"
                  << "Install a wallpaper with install.sh first.\n";
        return 1;
    }

    std::cout << "Current wallpaper: " << current << '\n';
    return 0;
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

    // An event is executed once. The daemon then sleeps until its end time.
    // This prevents repeated gsettings calls while a transition is active.
    const TimelineEvent *lastEvent = nullptr;

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

        if (event != lastEvent)
        {
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

            lastEvent = event;
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

    if (command == "list")
        return listWallpapers();

    if (command == "current")
        return showCurrentWallpaper();

    if (command == "set")
    {
        if (argc < 3)
        {
            std::cerr << "Usage: chronowall set <wallpaper name>\n";
            return 1;
        }
        return setWallpaper(joinArguments(argc, argv, 2));
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
