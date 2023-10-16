#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <optional>
#include <unordered_set>
#include <vector>

#include "Entity.hpp"
#include "Settings.hpp"
#include "Math.hpp"

class RenderWindow
{
public:
    // initializations
    RenderWindow(const char *p_title, int p_w, int p_h);
    RenderWindow(const char *p_title);
    ~RenderWindow();
    SDL_Texture *loadTexture(const char *p_filePath);
    SDL_Texture *createBoardTexture();

    // drawing
    void clear();
    void render(Entity &p_entity);
    void renderPiece(uint32_t p_pieceType, Vector2i pos);
    void drawHoverInd(Vector2i p_pos);
    void renderSquareHighlight(uint32_t color, Vector2i p_pos);
    void display() { SDL_RenderPresent(renderer); }

    // getters
    int getWidth() { return displayInfo.w; };
    int getHeight() { return displayInfo.h; };
    int getMSPF() { return 1000 / displayInfo.refresh_rate; }

    // setters
    void updateMousePos(Vector2i& mousePos) { SDL_GetMouseState(&mousePos.x, &mousePos.y); }

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_DisplayMode displayInfo;

    uint32_t backgroundColor = 0x112233ff;
    SDL_Rect sheetRect = {0, 0, 0, 0};
    SDL_Rect squareRect = {0, 0, 0, 0};

    SDL_Texture *spriteSheet = nullptr;
    
    void initTextures();
};
