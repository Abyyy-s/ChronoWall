#include "Transition.h"

Transition::Transition(
    std::string fromImage,
    std::string toImage,
    double duration)
    : fromImage(fromImage),
      toImage(toImage),
      duration(duration)
{
}

std::string Transition::getFromImage() const
{
    return fromImage;
}

std::string Transition::getToImage() const
{
    return toImage;
}

double Transition::getDuration() const
{
    return duration;
}
