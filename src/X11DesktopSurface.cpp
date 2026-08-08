#include "X11DesktopSurface.h"

#include <SDL3/SDL.h>

#include <X11/Xlib.h>
#include <iostream>

namespace
{
void lowerWindow(SDL_Window *window)
{
    if (!window)
        return;

    SDL_PropertiesID properties = SDL_GetWindowProperties(window);

    auto *display = static_cast<Display *>(SDL_GetPointerProperty(
        properties,
        SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
        nullptr));

    const ::Window xWindow = static_cast<::Window>(SDL_GetNumberProperty(
        properties,
        SDL_PROP_WINDOW_X11_WINDOW_NUMBER,
        0));

    if (!display || xWindow == 0)
        return;

    XLowerWindow(display, xWindow);
    XFlush(display);
}
}

bool X11DesktopSurface::init()
{
    // Ask SDL to create this as an X11 desktop window and bypass WM
    // management. We still explicitly lower it after creation/showing.
    SDL_SetHint(SDL_HINT_X11_WINDOW_TYPE, "_NET_WM_WINDOW_TYPE_DESKTOP");
    SDL_SetHint(SDL_HINT_X11_FORCE_OVERRIDE_REDIRECT, "1");
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
    lowerWindow(window);

    return true;
}

void X11DesktopSurface::show()
{
    if (!window)
        return;

    SDL_ShowWindow(window);
    lowerWindow(window);
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
