#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include "Math.hpp"

class Entity
{
    Vector2i pos;
    SDL_Texture *tex = nullptr;
    SDL_Rect currentFrame = { 0, 0, 0, 0 };

public:
    Entity(Vector2i p_pos, SDL_Texture *p_tex);
    Entity(){};
    ~Entity() { SDL_DestroyTexture(tex); }
    void updateVals(Vector2i p_pos, SDL_Texture *p_tex);
    void updatePos(Vector2i p_pos);
    void updateTex(SDL_Texture *p_tex);

    Vector2i &getPos() { return pos; }
    SDL_Texture *getTex() { return tex; }
    SDL_Rect getCurrentFrame() { return currentFrame; }
    int getHeight() { return currentFrame.h; }
    int getWidth() { return currentFrame.w; }
};