#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>

#include "Utils.hpp"

using namespace utils;

bool utils::initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        std::cout << "SDL_Init failed. Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!IMG_Init(IMG_INIT_PNG))
    {
        std::cout << "IMG_Init failed. Error: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void utils::fill_rounded_box(SDL_Surface *dst, int xo, int yo, int w, int h, int r, uint32_t color)
{
    int yd = dst->pitch / dst->format->BytesPerPixel;
    uint32_t *pixels = NULL;

    int x, y, i, j;
    int rpsqrt2 = (int)(r / sqrt(2));

    w /= 2;
    h /= 2;

    xo += w;
    yo += h;

    w -= r;
    h -= r;

    SDL_LockSurface(dst);
    pixels = (uint32_t *)(dst->pixels);

    int sy = (yo - h) * yd;
    int ey = (yo + h) * yd;
    int sx = (xo - w);
    int ex = (xo + w);
    for (i = sy; i <= ey; i += yd)
        for (j = sx - r; j <= ex + r; j++)
            pixels[i + j] = color;

    int d = -r;
    int x2m1 = -1;
    y = r;
    for (x = 0; x <= rpsqrt2; x++)
    {
        x2m1 += 2;
        d += x2m1;
        if (d >= 0)
        {
            y--;
            d -= (y * 2);
        }

        for (i = sx - x; i <= ex + x; i++)
            pixels[sy - y * yd + i] = color;

        for (i = sx - y; i <= ex + y; i++)
            pixels[sy - x * yd + i] = color;

        for (i = sx - y; i <= ex + y; i++)
            pixels[ey + x * yd + i] = color;

        for (i = sx - x; i <= ex + x; i++)
            pixels[ey + y * yd + i] = color;
    }
    SDL_UnlockSurface(dst);
}