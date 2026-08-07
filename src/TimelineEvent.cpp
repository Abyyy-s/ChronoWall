#include "TimelineEvent.h"

TimelineEvent::TimelineEvent(
    int startTime,
    int endTime,
    TimelineEventType type,
    int frameIndex,
    int transitionIndex)
    : startTime(startTime),
      endTime(endTime),
      type(type),
      frameIndex(frameIndex),
      transitionIndex(transitionIndex)
{
}

int TimelineEvent::getStartTime() const
{
    return startTime;
}

int TimelineEvent::getEndTime() const
{
    return endTime;
}

TimelineEventType TimelineEvent::getType() const
{
    return type;
}

int TimelineEvent::getFrameIndex() const
{
    return frameIndex;
}

int TimelineEvent::getTransitionIndex() const
{
    return transitionIndex;
}
