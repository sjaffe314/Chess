#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <filesystem>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Utils.hpp"
#include "Settings.hpp"

RenderWindow::RenderWindow(const char *p_title)
{
    if (SDL_GetDesktopDisplayMode(0, &displayInfo) != 0)
        std::cout << "SDL_GetDisplayMode failed. Error: " << SDL_GetError() << std::endl;

    window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayInfo.w, displayInfo.h, SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (window == NULL)
        std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
        std::cout << "Renderer failed to init. Error: " << SDL_GetError() << std::endl;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    font = TTF_OpenFont("res/dev/MontserratBold.ttf", 30);
    if (font == NULL)
        std::cout << "Font failed to init. Error: " << TTF_GetError() << std::endl;

    initTextures();
}

RenderWindow::~RenderWindow()
{
    SDL_DestroyTexture(spriteSheet);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}

SDL_Texture* RenderWindow::loadTexture(const char *p_filePath)
{
    SDL_Texture *texture = NULL;
    texture = IMG_LoadTexture(renderer, p_filePath);

    if (texture == NULL)
        std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;

    return texture;
}

SDL_Texture* RenderWindow::loadFont(std::string text, SDL_Color color)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == NULL)
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (texture == NULL)
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());

    SDL_FreeSurface(textSurface);

    return texture;
}

SDL_Texture *RenderWindow::createBoardTexture()
{
    // hardcoded color values
    uint32_t LC = 0xFF6600FF; // old: 0x512A2AFF
    uint32_t DC = 0xFFC89BFF; // old: 0x7C4C3EFF

    int squareSize = displayInfo.h / 11;
    int offset = squareSize / 3;
    int brddim = (offset + (squareSize << 2)) << 1;

    squareRect = { offset, offset, squareSize, squareSize };

    SDL_Surface* brdsurf = SDL_CreateRGBSurface(0, brddim, brddim, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

    if (!brdsurf)
    {
        std::cout << "Create surface failed. Error: " << SDL_GetError() << std::endl;
    }

    utils::fill_rounded_box(brdsurf, 0, 0, brddim, brddim, offset, 0x7a390dff);

    bool toggle_color = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            SDL_FillRect(brdsurf, &squareRect, toggle_color ? LC : DC);
            squareRect.x += squareSize;
            if (j != 7)
                toggle_color = !toggle_color;
        }
        squareRect.x = offset;
        squareRect.y += squareSize;
    }
    SDL_Texture* brdtex = SDL_CreateTextureFromSurface(renderer, brdsurf);
    SDL_FreeSurface(brdsurf);
    brdsurf = NULL;

    if (!brdtex)
        std::cout << "Create texture failed. Error: " << SDL_GetError() << std::endl;

    return brdtex;
}

void RenderWindow::clear()
{
    /*Clears window and renders background objects*/
    SDL_SetRenderDrawColor(renderer, (backgroundColor & 0xff000000) >> 24, (backgroundColor & 0xff0000) >> 16, (backgroundColor & 0xff00) >> 8, (backgroundColor & 0xff));
    SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity &p_entity)
{
    /*Renders an entity to the screen*/
    SDL_Rect src = p_entity.getCurrentFrame();

    SDL_Rect dst = src;
    dst.x = p_entity.getPos().x;
    dst.y = p_entity.getPos().y;

    SDL_RenderCopy(renderer, p_entity.getTex(), &src, &dst);
}

void RenderWindow::renderPiece(uint32_t p_pieceType, Vector2i p_pos)
{
    /*Renders an piece to the screen*/
    sheetRect.x = sheetRect.w * (p_pieceType % 8 - 1); // very specific for just this sprite sheet, must change if add more textures
    sheetRect.y = sheetRect.h * (p_pieceType / 8);

    squareRect.x = p_pos.x;
    squareRect.y = p_pos.y;

    SDL_RenderCopy(renderer, spriteSheet, &sheetRect, &squareRect);
}

void RenderWindow::drawHoverInd(Vector2i p_pos)
{
    squareRect.x = p_pos.x;
    squareRect.y = p_pos.y;

    // SDL_SetRenderDrawColor(renderer, 0x77, 0x80, 0x8f, 0xff);
    // SDL_RenderDrawRect(renderer, &squareRect);

    sheetRect.x = sheetRect.w * (6); // very specific for just this sprite sheet, must change if add more textures
    sheetRect.y = sheetRect.h * (0);

    SDL_RenderCopy(renderer, spriteSheet, &sheetRect, &squareRect);
}

void RenderWindow::renderSquareHighlight(uint32_t p_color, Vector2i p_pos)
{
    squareRect.x = p_pos.x;
    squareRect.y = p_pos.y;

    SDL_SetRenderDrawColor(renderer, (p_color & 0xff000000) >> 24, (p_color & 0xff0000) >> 16, (p_color & 0xff00) >> 8, (p_color & 0xff));
    SDL_RenderFillRect(renderer, &squareRect);
}

void RenderWindow::initTextures()
{
    spriteSheet = loadTexture("res/gfx/SpriteSheet.png");
    SDL_QueryTexture(spriteSheet, NULL, NULL, &sheetRect.w, &sheetRect.h);
    sheetRect.w /= 7;
    sheetRect.h /= 2;
}

// unused
RenderWindow::RenderWindow(const char *p_title, int p_w, int p_h)
{
    /*Creates a window that is a specific size, not full screen*/
    window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayInfo.w, displayInfo.h, SDL_WINDOW_SHOWN);

    if (window == NULL)
        std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
        std::cout << "Renderer failed to init. Error: " << SDL_GetError() << std::endl;

    if (SDL_GetDesktopDisplayMode(SDL_GetWindowDisplayIndex(window), &displayInfo) != 0)
        std::cout << "SDL_GetDisplayMode failed. Error: " << SDL_GetError() << std::endl;

    displayInfo.w = p_w;
    displayInfo.h = p_h;

    initTextures();
}
