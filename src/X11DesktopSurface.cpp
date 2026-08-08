#include "X11DesktopSurface.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <iostream>

namespace
{
void setNemoDesktopIcons(bool enabled)
{
    const char *value = enabled ? "true" : "false";
    const std::string command =
        std::string("gsettings set org.nemo.desktop show-desktop-icons ") + value;

    if (std::system(command.c_str()) != 0)
    {
        std::cerr << "Warning: could not set Nemo desktop icons to "
                  << value << "\n";
    }
}
}

bool X11DesktopSurface::init()
{
    // Nemo owns the normal desktop surface. ChronoWall only takes it over
    // briefly for short live transitions, so do not create another DESKTOP
    // window or use override-redirect here.
    SDL_SetHint(SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "0");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL initialization failed: "
                  << SDL_GetError() << '\n';
        return false;
    }

    const SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
    SDL_Rect bounds{};

    if (!SDL_GetDisplayBounds(displayID, &bounds))
    {
        std::cerr << "Could not get primary display bounds: "
                  << SDL_GetError() << '\n';
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow(
        "ChronoWall",
        bounds.w,
        bounds.h,
        SDL_WINDOW_BORDERLESS |
        SDL_WINDOW_NOT_FOCUSABLE |
        SDL_WINDOW_HIDDEN);

    if (!window)
    {
        std::cerr << "Desktop surface creation failed: "
                  << SDL_GetError() << '\n';
        SDL_Quit();
        return false;
    }

    SDL_SetWindowPosition(window, bounds.x, bounds.y);

    return true;
}

void X11DesktopSurface::show()
{
    if (!window)
        return;

    // Nemo paints icons and wallpaper as one surface. Cede that role before
    // mapping ChronoWall so the renderer does not cover desktop icons.
    setNemoDesktopIcons(false);
    SDL_ShowWindow(window);
}

void X11DesktopSurface::hide()
{
    if (!window)
        return;

    SDL_HideWindow(window);

    // Restore Nemo only after ChronoWall is hidden. The final frame is first
    // handed to gsettings by the daemon, so the desktop returns without a gap.
    setNemoDesktopIcons(true);
}

SDL_Window *X11DesktopSurface::getWindow()
{
    return window;
}

void X11DesktopSurface::shutdown()
{
    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // Always leave the user's desktop under Nemo's control.
    setNemoDesktopIcons(true);
    SDL_Quit();
}
