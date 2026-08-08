#include "TransitionRenderer.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

namespace
{
constexpr const char *TEMP_FRAME = "/tmp/chronowall-transition.bmp";

SDL_Surface *loadRGBA(const std::string &path)
{
    SDL_Surface *surface = SDL_LoadSurface(path.c_str());
    if (!surface)
        return nullptr;

    SDL_Surface *rgba = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    return rgba;
}

uint8_t blendChannel(uint8_t from, uint8_t to, double alpha)
{
    return static_cast<uint8_t>(
        from + (static_cast<double>(to) - from) * alpha + 0.5);
}
}

bool TransitionRenderer::init(IDesktopSurface &surface)
{
    if (!surface.getWindow())
    {
        std::cerr << "Transition renderer: desktop surface has no window.\n";
        return false;
    }

    renderer = SDL_CreateGPURenderer(nullptr, surface.getWindow());

    if (!renderer)
    {
        std::cerr << "GPU renderer creation failed: "
                  << SDL_GetError() << '\n';
        return false;
    }

    if (!SDL_SetRenderVSync(renderer, 1))
    {
        std::cerr << "Warning: VSync could not be enabled: "
                  << SDL_GetError() << '\n';
    }

    return true;
}

bool TransitionRenderer::render(
    const Transition &transition,
    const volatile std::sig_atomic_t *shutdownFlag)
{
    if (!renderer)
    {
        std::cerr << "Transition renderer is not initialized.\n";
        return false;
    }

    SDL_Surface *fromSurface =
        SDL_LoadSurface(transition.getFromImage().c_str());
    SDL_Surface *toSurface =
        SDL_LoadSurface(transition.getToImage().c_str());

    if (!fromSurface || !toSurface)
    {
        std::cerr << "Transition image loading failed: "
                  << SDL_GetError() << '\n';

        if (fromSurface)
            SDL_DestroySurface(fromSurface);
        if (toSurface)
            SDL_DestroySurface(toSurface);
        return false;
    }

    SDL_Texture *fromTexture =
        SDL_CreateTextureFromSurface(renderer, fromSurface);
    SDL_Texture *toTexture =
        SDL_CreateTextureFromSurface(renderer, toSurface);

    SDL_DestroySurface(fromSurface);
    SDL_DestroySurface(toSurface);

    if (!fromTexture || !toTexture)
    {
        std::cerr << "Transition texture creation failed: "
                  << SDL_GetError() << '\n';

        if (fromTexture)
            SDL_DestroyTexture(fromTexture);
        if (toTexture)
            SDL_DestroyTexture(toTexture);
        return false;
    }

    SDL_SetTextureBlendMode(fromTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(toTexture, SDL_BLENDMODE_BLEND);

    const double duration =
        std::max(0.001, transition.getDuration());

    const auto start = std::chrono::steady_clock::now();
    bool running = true;

    while (running)
    {
        if (shutdownFlag && *shutdownFlag)
            running = false;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        if (!running)
            break;

        const auto now = std::chrono::steady_clock::now();
        const double elapsed =
            std::chrono::duration<double>(now - start).count();
        const double progress =
            std::clamp(elapsed / duration, 0.0, 1.0);

        SDL_SetTextureAlphaModFloat(
            fromTexture, static_cast<float>(1.0 - progress));
        SDL_SetTextureAlphaModFloat(
            toTexture, static_cast<float>(progress));

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, fromTexture, nullptr, nullptr);
        SDL_RenderTexture(renderer, toTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        if (progress >= 1.0)
            running = false;
    }

    SDL_DestroyTexture(fromTexture);
    SDL_DestroyTexture(toTexture);

    return shutdownFlag == nullptr || !*shutdownFlag;
}

std::string TransitionRenderer::preBlendFrame(
    const Transition &transition,
    double alpha)
{
    alpha = std::clamp(alpha, 0.0, 1.0);

    SDL_Surface *from = loadRGBA(transition.getFromImage());
    SDL_Surface *to = loadRGBA(transition.getToImage());

    if (!from || !to)
    {
        std::cerr << "Offline transition image loading failed: "
                  << SDL_GetError() << '\n';
        if (from)
            SDL_DestroySurface(from);
        if (to)
            SDL_DestroySurface(to);
        return {};
    }

    if (from->w != to->w || from->h != to->h)
    {
        std::cerr << "Offline transition images have different dimensions.\n";
        SDL_DestroySurface(from);
        SDL_DestroySurface(to);
        return {};
    }

    SDL_Surface *output = SDL_CreateSurface(
        from->w,
        from->h,
        SDL_PIXELFORMAT_RGBA32);

    if (!output)
    {
        std::cerr << "Offline transition output creation failed: "
                  << SDL_GetError() << '\n';
        SDL_DestroySurface(from);
        SDL_DestroySurface(to);
        return {};
    }

    const auto *fromFormat = SDL_GetPixelFormatDetails(from->format);
    const auto *toFormat = SDL_GetPixelFormatDetails(to->format);
    SDL_Palette *fromPalette = SDL_GetSurfacePalette(from);
    SDL_Palette *toPalette = SDL_GetSurfacePalette(to);

    for (int y = 0; y < output->h; ++y)
    {
        const auto *fromRow = reinterpret_cast<const uint32_t *>(
            static_cast<const uint8_t *>(from->pixels) + y * from->pitch);
        const auto *toRow = reinterpret_cast<const uint32_t *>(
            static_cast<const uint8_t *>(to->pixels) + y * to->pitch);
        auto *outRow = reinterpret_cast<uint32_t *>(
            static_cast<uint8_t *>(output->pixels) + y * output->pitch);

        for (int x = 0; x < output->w; ++x)
        {
            uint8_t fr, fg, fb, fa;
            uint8_t tr, tg, tb, ta;

            SDL_GetRGBA(
                fromRow[x], fromFormat, fromPalette,
                &fr, &fg, &fb, &fa);
            SDL_GetRGBA(
                toRow[x], toFormat, toPalette,
                &tr, &tg, &tb, &ta);

            outRow[x] = SDL_MapSurfaceRGBA(
                output,
                blendChannel(fr, tr, alpha),
                blendChannel(fg, tg, alpha),
                blendChannel(fb, tb, alpha),
                blendChannel(fa, ta, alpha));
        }
    }

    const bool saved = SDL_SaveBMP(output, TEMP_FRAME);

    SDL_DestroySurface(output);
    SDL_DestroySurface(from);
    SDL_DestroySurface(to);

    if (!saved)
    {
        std::cerr << "Offline transition frame save failed: "
                  << SDL_GetError() << '\n';
        return {};
    }

    return TEMP_FRAME;
}

void TransitionRenderer::shutdown()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    std::remove(TEMP_FRAME);
}
