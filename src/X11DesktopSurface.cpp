#include "X11DesktopSurface.h"

#include <SDL3/SDL.h>
#include <X11/Xlib.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace
{
bool getNemoDesktopIconsEnabled()
{
    FILE *pipe = popen(
        "gsettings get org.nemo.desktop show-desktop-icons",
        "r");

    if (!pipe)
        return true;

    char buffer[32]{};
    const bool read = std::fgets(buffer, sizeof(buffer), pipe) != nullptr;
    pclose(pipe);

    if (!read)
        return true;

    return std::string(buffer).find("true") != std::string::npos;
}

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

bool nemoDesktopExists(Display *display)
{
    if (!display)
        return false;

    Window root = DefaultRootWindow(display);
    Window rootReturn = 0;
    Window parentReturn = 0;
    Window *children = nullptr;
    unsigned int childCount = 0;

    if (!XQueryTree(
            display,
            root,
            &rootReturn,
            &parentReturn,
            &children,
            &childCount))
    {
        return false;
    }

    bool found = false;

    for (unsigned int i = 0; i < childCount; ++i)
    {
        XClassHint classHint{};

        if (XGetClassHint(display, children[i], &classHint))
        {
            const bool isNemo =
                (classHint.res_name && std::string(classHint.res_name) == "nemo-desktop") ||
                (classHint.res_class && std::string(classHint.res_class) == "Nemo-desktop");

            if (classHint.res_name)
                XFree(classHint.res_name);
            if (classHint.res_class)
                XFree(classHint.res_class);

            if (isNemo)
            {
                found = true;
                break;
            }
        }
    }

    if (children)
        XFree(children);

    return found;
}
}

bool X11DesktopSurface::init()
{
    // Nemo owns the normal desktop surface. ChronoWall only takes it over
    // briefly for short live transitions, so do not create another DESKTOP
    // window or use override-redirect here.
    originalDesktopIconsEnabled = getNemoDesktopIconsEnabled();

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

    // Nemo paints icons and wallpaper as one opaque surface. Cede that role
    // and wait for its X11 window to disappear before mapping ChronoWall.
    setNemoDesktopIcons(false);

    SDL_PropertiesID properties = SDL_GetWindowProperties(window);
    Display *display = properties
        ? static_cast<Display *>(SDL_GetPointerProperty(
              properties,
              SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
              nullptr))
        : nullptr;

    if (display)
    {
        constexpr int maxAttempts = 40;
        constexpr auto interval = std::chrono::milliseconds(25);

        for (int attempt = 0; attempt < maxAttempts; ++attempt)
        {
            XSync(display, False);

            if (!nemoDesktopExists(display))
                break;

            std::this_thread::sleep_for(interval);
        }
    }

    SDL_ShowWindow(window);
}

void X11DesktopSurface::hide()
{
    if (!window)
        return;

    SDL_HideWindow(window);

    // Give X11 a chance to process the unmap before Nemo recreates its desktop.
    SDL_PropertiesID properties = SDL_GetWindowProperties(window);
    Display *display = properties
        ? static_cast<Display *>(SDL_GetPointerProperty(
              properties,
              SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
              nullptr))
        : nullptr;

    if (display)
        XSync(display, False);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    setNemoDesktopIcons(originalDesktopIconsEnabled);

    if (display && originalDesktopIconsEnabled)
    {
        constexpr int maxAttempts = 40;
        constexpr auto interval = std::chrono::milliseconds(25);

        for (int attempt = 0; attempt < maxAttempts; ++attempt)
        {
            XSync(display, False);

            if (nemoDesktopExists(display))
                break;

            std::this_thread::sleep_for(interval);
        }
    }
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

    setNemoDesktopIcons(originalDesktopIconsEnabled);
    SDL_Quit();
}
