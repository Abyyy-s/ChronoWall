#ifndef TIMELINEEVENT_H
#define TIMELINEEVENT_H

#include "WallpaperFrame.h"
#include "Transition.h"

enum class TimelineEventType
{
    Static,
    Transition
};

class TimelineEvent
{
private:
    int startTime;
    int endTime;

    TimelineEventType type;

    int frameIndex;

    int transitionIndex;

public:
    TimelineEvent(
        int startTime,
        int endTime,
        TimelineEventType type,
        int frameIndex,
        int transitionIndex);

    int getStartTime() const;

    int getEndTime() const;

    TimelineEventType getType() const;

    int getFrameIndex() const;

    int getTransitionIndex() const;
};

#endif
