#ifndef IDESKTOPSURFACE_H
#define IDESKTOPSURFACE_H

#include <SDL3/SDL.h>

class IDesktopSurface
{
public:
    virtual bool init() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual SDL_Window *getWindow() = 0;
    virtual void shutdown() = 0;
    virtual ~IDesktopSurface() = default;
};

#endif
