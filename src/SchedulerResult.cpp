#include "SchedulerResult.h"

SchedulerResult::SchedulerResult(
    SchedulerState state,
    const WallpaperFrame *frame,
    const Transition *transition,
    double progress)
    : state(state),
      frame(frame),
      transition(transition),
      progress(progress)
{
}

SchedulerState SchedulerResult::getState() const
{
    return state;
}

const WallpaperFrame *SchedulerResult::getFrame() const
{
    return frame;
}

const Transition *SchedulerResult::getTransition() const
{
    return transition;
}

double SchedulerResult::getProgress() const
{
    return progress;
}
