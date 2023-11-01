#include "Entity.hpp"
#include <SDL.h>
#include <SDL_image.h>

Entity::Entity(Vector2i p_pos, SDL_Texture *p_tex)
    : pos(p_pos), tex(p_tex)
{
    currentFrame.x = 0;
    currentFrame.y = 0;
    SDL_QueryTexture(tex, NULL, NULL, &currentFrame.w, &currentFrame.h);
}

void Entity::updateVals(Vector2i p_pos, SDL_Texture *p_tex)
{
    updatePos(p_pos);
    updateTex(p_tex);
}

void Entity::updatePos(Vector2i p_pos)
{
    pos = p_pos;
}

void Entity::updateTex(SDL_Texture *p_tex)
{
    SDL_DestroyTexture(tex);
    tex = p_tex;
    currentFrame.x = 0;
    currentFrame.y = 0;
    SDL_QueryTexture(tex, NULL, NULL, &currentFrame.w, &currentFrame.h);
}
