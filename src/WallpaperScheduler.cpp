#include "WallpaperScheduler.h"
#include <cmath>

const TimelineEvent *WallpaperScheduler::getCurrentEvent(
    const DynamicWallpaper &wallpaper,
    double elapsedSeconds)
{
    const auto &timeline = wallpaper.getTimeline();

    if (timeline.empty())
    {
        return nullptr;
    }

    double totalDuration =
        timeline.back().getEndTime();

    elapsedSeconds =
        std::fmod(elapsedSeconds, totalDuration);

    for (const auto &event : timeline)
    {
        if (elapsedSeconds >= event.getStartTime() &&
            elapsedSeconds < event.getEndTime())
        {
            return &event;
        }
    }

    return &timeline.back();
}
