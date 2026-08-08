#include "X11DesktopSurface.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_syswm.h>

#include <X11/Xlib.h>
#include <iostream>

bool X11DesktopSurface::init()
{
    SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE_DESKTOP");

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
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_NOT_FOCUSABLE);

    if (!window)
    {
        std::cerr << "Desktop surface creation failed: "
                  << SDL_GetError() << '\n';
        SDL_Quit();
        return false;
    }

    SDL_SetWindowPosition(window, bounds.x, bounds.y);

    SDL_SysWMinfo wmInfo{};
    wmInfo.version.major = SDL_MAJOR_VERSION;
    wmInfo.version.minor = SDL_MINOR_VERSION;
    wmInfo.version.patch = SDL_MICRO_VERSION;

    if (!SDL_GetWindowWMInfo(window, &wmInfo))
    {
        std::cerr << "Could not get X11 window information: "
                  << SDL_GetError() << '\n';
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return false;
    }

    if (wmInfo.subsystem != SDL_SYSWM_X11)
    {
        std::cerr << "ChronoWall requires an X11 SDL video subsystem.\n";
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return false;
    }

    Display *display = wmInfo.info.x11.display;
    ::Window xWindow = wmInfo.info.x11.window;

    XLowerWindow(display, xWindow);
    XFlush(display);

    SDL_HideWindow(window);
    return true;
}

void X11DesktopSurface::show()
{
    if (!window)
        return;

    SDL_ShowWindow(window);

    SDL_SysWMinfo wmInfo{};
    wmInfo.version.major = SDL_MAJOR_VERSION;
    wmInfo.version.minor = SDL_MINOR_VERSION;
    wmInfo.version.patch = SDL_MICRO_VERSION;

    if (SDL_GetWindowWMInfo(window, &wmInfo) &&
        wmInfo.subsystem == SDL_SYSWM_X11)
    {
        XLowerWindow(wmInfo.info.x11.display, wmInfo.info.x11.window);
        XFlush(wmInfo.info.x11.display);
    }
}

void X11DesktopSurface::hide()
{
    if (window)
        SDL_HideWindow(window);
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

    SDL_Quit();
}
