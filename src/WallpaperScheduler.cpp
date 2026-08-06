#include "WallpaperScheduler.h"

std::optional<WallpaperFrame> WallpaperScheduler::getCurrentWallpaper(
    const std::vector<WallpaperFrame> &frames,
    int elapsedSeconds)
{
    if (frames.empty())
    {
        return std::nullopt;
    }

    int totalDuration = 0;

    for (const auto &frame : frames)
    {
        totalDuration += frame.getDuration();
    }

    elapsedSeconds %= totalDuration;

    for (const auto &frame : frames)
    {
        if (elapsedSeconds < frame.getDuration())
        {
            return frame;
        }

        elapsedSeconds -= frame.getDuration();
    }

    return frames.back();
}
