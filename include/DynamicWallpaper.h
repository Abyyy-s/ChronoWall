#ifndef DYNAMICWALLPAPER_H
#define DYNAMICWALLPAPER_H

#include <vector>
#include "WallpaperFrame.h"
#include "Transition.h"

class DynamicWallpaper
{
private:
    std::vector<WallpaperFrame> frames;

    std::vector<Transition> transitions;

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
};

#endif
