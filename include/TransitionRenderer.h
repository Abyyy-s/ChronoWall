#ifndef TRANSITIONRENDERER_H
#define TRANSITIONRENDERER_H

#include "IDesktopSurface.h"
#include "Transition.h"

#include <csignal>
#include <string>

class TransitionRenderer
{
public:
    bool init(IDesktopSurface &surface);

    // Live GPU crossfade for short transitions. The optional shutdown flag
    // lets Ctrl+C/SIGTERM interrupt an active transition immediately.
    bool render(
        const Transition &transition,
        const volatile std::sig_atomic_t *shutdownFlag = nullptr);

    // Generate one blended frame for long transitions. Each call writes a
    // uniquely named temporary BMP so the desktop wallpaper URI changes and
    // the desktop reloads the new frame.
    std::string preBlendFrame(
        const Transition &transition,
        double alpha);

    void shutdown();

private:
    SDL_Renderer *renderer = nullptr;
    std::string lastTempFrame;
    unsigned long long tempFrameCounter = 0;
};

#endif
