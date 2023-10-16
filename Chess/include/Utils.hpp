#pragma once

#include <SDL.h>
#include <cstdint>

#include "Math.hpp"

namespace utils
{
    bool initSDL();

    inline float hireTimeInSeconds() { return SDL_GetTicks() * 0.001f; }

    void fill_rounded_box(SDL_Surface *dst, int xo, int yo, int w, int h, int r, Uint32 color);
}