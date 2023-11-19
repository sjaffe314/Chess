#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <filesystem>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Utils.hpp"
#include "Settings.hpp"

#define TO_COLOR_COMPONENTS(color) (uint8_t)((color & 0xff000000) >> 24), (uint8_t)((color & 0xff0000) >> 16), (uint8_t)((color & 0xff00) >> 8), (uint8_t)(color & 0xff)

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


    scale = displayInfo.h / 11;
    font = TTF_OpenFont("res/dev/UbuntuMono-B.ttf", scale * 9 / 24);
    if (font == NULL)
        std::cout << "Font failed to init. Error: " << TTF_GetError() << std::endl;

    initTextures();
}

RenderWindow::~RenderWindow()
{
    SDL_DestroyTexture(spriteSheet);
    TTF_CloseFont(font);
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

SDL_Texture* RenderWindow::loadText(const char* text, uint32_t color)
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, { TO_COLOR_COMPONENTS(color) });
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

    int squareSize = scale;
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

SDL_Texture* RenderWindow::createButtonTexture(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, const char* text, uint32_t textColor)
{
    SDL_Surface* image = TTF_RenderText_Solid(font, text, { TO_COLOR_COMPONENTS(textColor) });

    return createButtonTextureHelper(dims, r, primaryColor, secondaryColor, image, {0, 0, image->w, image->h});
}

SDL_Texture* RenderWindow::createButtonTexture(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, uint32_t pieceType)
{
    SDL_Surface* image = IMG_Load("res/gfx/SpriteSheet.png");
    sheetRect.x = sheetRect.w * (pieceType % 8 - 1);
    sheetRect.y = sheetRect.h * (pieceType / 8);

    return createButtonTextureHelper(dims, r, primaryColor, secondaryColor, image, sheetRect);
}

SDL_Texture* RenderWindow::createButtonTextureHelper(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, SDL_Surface* image, SDL_Rect srcrect)
{
    SDL_Surface* surf = SDL_CreateRGBSurface(0, dims.x * 3, dims.y, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    
    SDL_Rect dstrect = { 0, 0, dims.x, dims.y };
    fillRoundedBox(surf, dstrect, r, primaryColor);
    dstrect.x += dstrect.w;
    fillRoundedBox(surf, dstrect, r, secondaryColor);
    
    dstrect.x += dstrect.w;
    fillRoundedBox(surf, dstrect, r, primaryColor);
    int border = 5;
    dstrect.x += border;
    dstrect.y += border;
    dstrect.w -= 2 * border;
    dstrect.h -= 2 * border;
    fillRoundedBox(surf, dstrect, r - border, secondaryColor);



    dstrect = { (dims.x - srcrect.w) / 2, (dims.y - srcrect.h) / 2, srcrect.w, srcrect.h };
    for (int i = 0; i < 3; i++) {
        SDL_BlitSurface(image, &srcrect, surf, &dstrect);
        dstrect.x += dims.x;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    SDL_FreeSurface(image);
    return texture;
}

SDL_Texture* RenderWindow::createTwoColoredBox(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor)
{
    SDL_Surface* surf = SDL_CreateRGBSurface(0, dims.x * 2, dims.y, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    SDL_Rect rect = { 0, 0, dims.x, dims.y };
    fillRoundedBox(surf, rect, r, primaryColor);
    rect.x = rect.w;
    fillRoundedBox(surf, rect, r, secondaryColor);

    SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return text;
}

SDL_Texture* RenderWindow::createThreeColoredBox(Vector2i dims, int r, uint32_t primaryColor, uint32_t secondaryColor, uint32_t tertiaryColor)
{
    SDL_Surface* surf = SDL_CreateRGBSurface(0, dims.x * 3, dims.y, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    SDL_Rect rect = { 0, 0, dims.x, dims.y };
    fillRoundedBox(surf, rect, r, primaryColor);
    rect.x += rect.w;
    fillRoundedBox(surf, rect, r, secondaryColor);
    rect.x += rect.w;
    fillRoundedBox(surf, rect, r, tertiaryColor);

    SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return text;
}

SDL_Texture* RenderWindow::createTexture(Vector2i p_dims)
{
    SDL_Texture* text = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, p_dims.x, p_dims.y);
    SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);

    return text;
}

void RenderWindow::blit(SDL_Texture* target, Entity& source)
{
    SDL_SetRenderTarget(renderer, target);
    render(source);
    SDL_SetRenderTarget(renderer, NULL);
}

SDL_Texture* RenderWindow::combineTextures(SDL_Texture* texture1, SDL_Texture* texture2)
{
    SDL_Rect src1{ 0 }, src2{ 0 };
    SDL_QueryTexture(texture1, NULL, NULL, &src1.x, &src1.y);
    SDL_QueryTexture(texture2, NULL, NULL, &src2.x, &src2.y);
    SDL_Texture* target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, src1.x + src2.x, src1.y + src2.y);
    
    SDL_SetRenderTarget(renderer, target);

    SDL_Rect dst = src1;
    SDL_RenderCopy(renderer, texture1, &src1, &dst);
    dst.x = src1.w;
    SDL_RenderCopy(renderer, texture1, &src1, &dst);

    SDL_SetRenderTarget(renderer, NULL);

    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    return target;
}

void RenderWindow::fillRoundedBox(SDL_Surface* surf, SDL_Rect dst, int r, uint32_t color)
{
    int x, y;
    int xo = dst.x, yo = dst.y, w = dst.w, h = dst.h;
    r = CLAMP(r, 0, h / 2);
    int mid = (int)(r / sqrt(2)) + 1;
    // the last x (incremented by one so that we don't double color a pixel)

    uint32_t* pixels = NULL;
    int dy = surf->pitch / surf->format->BytesPerPixel;  // width of image in pixels

    SDL_LockSurface(surf);
    pixels = (uint32_t*)surf->pixels;

    // fills middle cross (leaves out the corners)
    int sy = (yo + r) * dy;
    int ey = (yo + h - 1 - r) * dy;
    int sx = xo + r;
    int ex = xo + w - 1 - r;
    for (y = sy; y <= ey; y += dy) {
        for (int i = mid; i <= r; i++) {
            pixels[y + sx - i] = color;
            pixels[y + ex + i] = color;
        }
    }

    sy = yo * dy;
    ey = (yo + h - 1) * dy;
    for (y = sy; y <= ey; y += dy)
        for (x = sx; x <= ex; x++)
            pixels[y + x] = color;
    // start is at top left indented horizontally
    // end is bottom right indented horizontally

    // Bresenhams circle drawing algorithim
    int cx = 0, cy = r;  // points on the Bresenham circle, from 90 to 45 degrees
    int d = 3 - 2 * r;
    // Difference between x incremented and real: (x+1)^2 +y^2 - r^2
    // Difference between x and y incremented and real: (x+1)^2 +(y+1)^2 - r^2
    // d is the sum of those two, if pos, then the x++ is farther away than x++ & y--, so need to do the y--
    // the increments in the loop can be derived algebraically 
    while (cy > cx)
    {
        if (d > 0)
        {
            d += 4 * (cx - cy) + 10;
            
            cy--;
            sy += dy;
            ey -= dy;
        }
        else
            d += 4 * cx + 6;

        cx++;

        // draw
        for (y = sy; y <= ey; y += dy) // fills in whole column
        {
            pixels[y + sx - cx] = color;
            pixels[y + ex + cx] = color;
        }

        for (int i = mid; i <= cy; i++)
        {
            pixels[(yo + r - cx) * dy + sx - i] = color;
            pixels[(yo + r - cx) * dy + ex + i] = color;
            pixels[(yo + h - 1 - r + cx) * dy + sx - i] = color;
            pixels[(yo + h - 1 - r + cx) * dy + ex + i] = color;
        }
    }
    SDL_UnlockSurface(surf);
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

void RenderWindow::render(DoubleEntity& p_doubleEntity)
{
    render(p_doubleEntity.getBackgroundEntity());
    render(p_doubleEntity.getForegroundEntity());
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

void RenderWindow::renderPiece(uint32_t p_pieceType, SDL_Rect dst)
{
    /*Renders an piece to the screen*/
    sheetRect.x = sheetRect.w * (p_pieceType % 8 - 1); // very specific for just this sprite sheet, must change if add more textures
    sheetRect.y = sheetRect.h * (p_pieceType / 8);

    SDL_RenderCopy(renderer, spriteSheet, &sheetRect, &dst);
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
