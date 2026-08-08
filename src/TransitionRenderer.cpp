#include "TransitionRenderer.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <iostream>

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

bool TransitionRenderer::render(const Transition &transition)
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
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

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

    return true;
}

void TransitionRenderer::shutdown()
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
}
