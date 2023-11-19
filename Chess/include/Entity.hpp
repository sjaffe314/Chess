#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include "Math.hpp"

class Entity
{
public:
    //initializers
    Entity(Vector2i p_pos, SDL_Texture *p_tex, int frames = 1) : pos(p_pos) { setTexture(p_tex, frames); };
    Entity(Vector2i p_pos, SDL_Texture* p_tex, bool center, int frames = 1) { setTexture(p_tex, frames); pos = p_pos - Vector2i{getWidth(), getHeight()} / 2; };
    Entity(SDL_Texture* p_tex, int frames = 1) { setTexture(p_tex, frames); };
    Entity(Vector2i p_pos, Vector2i p_dims);
    Entity(){};
    ~Entity() { SDL_DestroyTexture(tex); }
    void setPos(Vector2i p_pos) { pos = p_pos; };
    void setTexture(SDL_Texture *p_tex, int frames = 1);

    //setters
    void setFrame(int frame) { currentFrame.x = frame * currentFrame.w; }

    //getters
    Vector2i getPos() { return pos; }
    SDL_Texture *getTex() { return tex; }
    SDL_Rect getCurrentFrame() { return currentFrame; }
    int getHeight() { return currentFrame.h; }
    int getWidth() { return currentFrame.w; }
    Vector2i getDimensions() { return { currentFrame.h, currentFrame.w }; }
protected:
    Vector2i pos;
    SDL_Texture *tex = nullptr;
    SDL_Rect currentFrame = { 0, 0, 0, 0 };
};

enum AlignementCodes : uint8_t
{
    Center = 1,
    FloatLeft = 2,
    FloatRight = 3
};

class DoubleEntity
{
public:
    DoubleEntity(Vector2i p_pos, SDL_Texture* p_texBack, int framesBack, SDL_Texture* p_texFore, int framesFore, uint8_t p_alignment);
    DoubleEntity() {}
    void setBackground(SDL_Texture* p_tex, int frames = 1);
    void setForeground(SDL_Texture* p_tex, int frames = 1);
    void setPos(Vector2i p_pos);
    void setAlignment(uint8_t p_alignment);
    
    void setBackgroundFrame(int frame) { background.setFrame(frame); }
    void setForegroundFrame(int frame) { foreground.setFrame(frame); }

    //getters
    Entity& getBackgroundEntity() { return background; }
    Entity& getForegroundEntity() { return foreground; }
    int getHeight() { return background.getHeight(); }
    int getWidth() { return background.getWidth(); }
protected:
    void updateForegroundPos();

    Entity background;
    Entity foreground;

    uint8_t alignment = 1;
    int padding = 10;
};