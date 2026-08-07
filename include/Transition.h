#ifndef TRANSITION_H
#define TRANSITION_H

#include <string>

class Transition
{
private:
    std::string fromImage;
    std::string toImage;

    double duration;

public:
    Transition(
        std::string fromImage,
        std::string toImage,
        double duration);

    std::string getFromImage() const;

    std::string getToImage() const;

    double getDuration() const;
};

#endif
