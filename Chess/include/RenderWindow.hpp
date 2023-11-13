#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <iostream>
#include <string>
#include <optional>
#include <unordered_set>
#include <vector>

#include "Entity.hpp"
#include "Settings.hpp"
#include "Math.hpp"

class RenderWindow
{
public:
    static RenderWindow& get()
    {
        static RenderWindow INSTANCE("Chess");
        return INSTANCE;
    }
    
    // initializations
    RenderWindow(const char *p_title, int p_w, int p_h);
    RenderWindow(const char *p_title);
    ~RenderWindow();
    SDL_Texture *loadTexture(const char *p_filePath);
    SDL_Texture *loadText(const char* text, uint32_t color);
    
    // texture initializers
    SDL_Texture *createBoardTexture();
    SDL_Texture* createButtonTexture(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, const char* text, uint32_t textColor = 0xffffffff);
    SDL_Texture* createButtonTexture(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, uint32_t pieceType);
    SDL_Texture* createTwoColoredBox(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor);

    //creating 
    SDL_Texture* createTexture(Vector2i p_dims);
    void blit(SDL_Texture* target, Entity& source);
    SDL_Texture* combineTextures(SDL_Texture* texture1, SDL_Texture* texture2);
    static void fillRoundedBox(SDL_Surface* surf, SDL_Rect dst, int r, uint32_t color);

    // drawing
    void clear();
    void render(Entity &p_entity);
    void render(DoubleEntity &p_doubleEntity);
    void renderPiece(uint32_t p_pieceType, Vector2i pos);
    void renderPiece(uint32_t p_pieceType, SDL_Rect dst);
    void drawHoverInd(Vector2i p_pos);
    void renderSquareHighlight(uint32_t color, Vector2i p_pos);
    void display() { SDL_RenderPresent(renderer); }

    // getters
    int getWidth() { return displayInfo.w; }
    int getHeight() { return displayInfo.h; }
    int getScale() { return scale; }
    int getMSPF() { return 1000 / displayInfo.refresh_rate; }

    // setters
    void updateMousePos(Vector2i& mousePos) { SDL_GetMouseState(&mousePos.x, &mousePos.y); }

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    TTF_Font *font = nullptr;
    SDL_DisplayMode displayInfo;

    uint32_t backgroundColor = 0x112233ff;
    SDL_Rect sheetRect = {0, 0, 0, 0};
    SDL_Rect squareRect = {0, 0, 0, 0};
    int scale = 0;

    SDL_Texture *spriteSheet = nullptr;
    
    void initTextures();
    SDL_Texture* createButtonTextureHelper(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, SDL_Surface* image, SDL_Rect srcrect);
};
