#ifndef DYNAMICWALLPAPER_H
#define DYNAMICWALLPAPER_H

#include <vector>
#include "WallpaperFrame.h"
#include "Transition.h"
#include "TimelineEvent.h"

class DynamicWallpaper
{
private:
    std::vector<WallpaperFrame> frames;

    std::vector<Transition> transitions;

    std::vector<TimelineEvent> timeline;

    int startTimeSeconds;

public:
    DynamicWallpaper();

    void setFrames(const std::vector<WallpaperFrame> &frames);

    const std::vector<WallpaperFrame> &getFrames() const;

    void setStartTime(int seconds);

    int getStartTime() const;

    void setTransitions(
        const std::vector<Transition> &transitions);

    const std::vector<Transition> &
    getTransitions() const;

    void setTimeline(
        const std::vector<TimelineEvent> &timeline);

    const std::vector<TimelineEvent> &
    getTimeline() const;
};

#endif
