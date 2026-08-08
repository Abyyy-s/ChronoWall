#ifndef X11DESKTOPSURFACE_H
#define X11DESKTOPSURFACE_H

#include "IDesktopSurface.h"

class X11DesktopSurface : public IDesktopSurface
{
public:
    bool init() override;
    void show() override;
    void hide() override;
    SDL_Window *getWindow() override;
    void shutdown() override;

private:
    SDL_Window *window = nullptr;
};

#endif
