#ifndef SCHEDULERRESULT_H
#define SCHEDULERRESULT_H

#include "WallpaperFrame.h"
#include "Transition.h"

enum class SchedulerState
{
    Static,
    Transition
};

class SchedulerResult
{
private:
    SchedulerState state;

    const WallpaperFrame *frame;

    const Transition *transition;

    double progress;

public:
    SchedulerResult(
        SchedulerState state,
        const WallpaperFrame *frame,
        const Transition *transition,
        double progress);

    SchedulerState getState() const;

    const WallpaperFrame *getFrame() const;

    const Transition *getTransition() const;

    double getProgress() const;
};

#endif
