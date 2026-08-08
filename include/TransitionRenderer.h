#ifndef TRANSITIONRENDERER_H
#define TRANSITIONRENDERER_H

#include "IDesktopSurface.h"
#include "Transition.h"

class TransitionRenderer
{
public:
    bool init(IDesktopSurface &surface);
    bool render(const Transition &transition);
    void shutdown();

private:
    SDL_Renderer *renderer = nullptr;
};

#endif
