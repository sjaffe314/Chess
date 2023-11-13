#include "Entity.hpp"
#include <SDL.h>
#include <SDL_image.h>

Entity::Entity(Vector2i p_pos, Vector2i p_dims)
    : pos(p_pos), currentFrame{ 0, 0, p_dims.x, p_dims.y }
{}

void Entity::setTexture(SDL_Texture *p_tex, int frames)
{
    SDL_DestroyTexture(tex);
    tex = p_tex;
    currentFrame.x = 0;
    currentFrame.y = 0;
    SDL_QueryTexture(tex, NULL, NULL, &currentFrame.w, &currentFrame.h);
    currentFrame.w /= frames;
}

DoubleEntity::DoubleEntity(Vector2i p_pos, SDL_Texture* p_texBack, int framesBack, SDL_Texture* p_texFore, int framesFore, uint8_t p_alignment)
    : background(p_pos, p_texBack, framesBack), foreground(p_texFore, framesFore), alignment(p_alignment)
{
    updateForegroundPos();
}

void DoubleEntity::setBackground(SDL_Texture* p_tex, int frames)
{
    background.setTexture(p_tex, frames);
    updateForegroundPos();
}

void DoubleEntity::setForeground(SDL_Texture* p_tex, int frames)
{
    foreground.setTexture(p_tex, frames);
    updateForegroundPos();
}

void DoubleEntity::setPos(Vector2i p_pos)
{
    background.setPos(p_pos);
    updateForegroundPos();
}

void DoubleEntity::setAlignment(uint8_t p_alignment)
{
    alignment = p_alignment;
    updateForegroundPos();
}

void DoubleEntity::updateForegroundPos()
{
    Vector2i offset{ 0, (background.getHeight() - foreground.getHeight()) / 2 };
    switch (alignment)
    {
    case Center:
        offset.x = (background.getWidth() - foreground.getWidth()) / 2;
        break;
    case FloatLeft:
        offset.x = padding;
        break;
    case FloatRight:
        offset.x = background.getWidth() - foreground.getWidth() - padding;
        break;
    }
    foreground.setPos(background.getPos() + offset);
}
