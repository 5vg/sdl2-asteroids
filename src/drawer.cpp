#include "drawer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBTTF_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION

#include <algorithm>
#include <stb_image.h>
#include <stbttf.h>
#include <string>
#include <SDL.h>

static SDL_Surface* CreateImageSurface(const char* img)
{
    int req_format = STBI_rgb_alpha;
    int width, height, orig_format;
    unsigned char* data = stbi_load(img, &width, &height, &orig_format, req_format);
    if (data == nullptr)
    {
        SDL_Log("Loading image %s failed: %s", img, stbi_failure_reason());
        return nullptr;
    }

    int depth, pitch;
    Uint32 pixel_format;
    if (req_format == STBI_rgb_alpha)
    {
        depth = 32;
        pitch = 4 * width;
        pixel_format = SDL_PIXELFORMAT_RGBA32;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)data, width, height,
                                                           depth, pitch, pixel_format);

    if (surface == nullptr)
    {
        SDL_Log("Creating surface failed: %s", SDL_GetError());
        return nullptr;
    }

    return surface;
}

 Drawer* Drawer::Create(SDL_Window* window, SDL_Renderer* renderer)
{
    Drawer* newDrawer = new Drawer();
    newDrawer->m_window = window;
    newDrawer->m_renderer = renderer;
    return newDrawer;
}

void Drawer::LoadImage(const char* img)
{
    SDL_Surface* surface = CreateImageSurface(img);

    SDL_Texture* optimizedSurface = SDL_CreateTextureFromSurface(m_renderer, surface);

    std::string str(img);

    textureMap[str] = optimizedSurface;
    surfaceMap[str] = surface;
}


bool Drawer::DrawImage(const char* img, int posX, int posY, double angle)
{
    std::string str(img);
    /*check made if the image exists in the surface map*/
    if(surfaceMap.count(str))
    {
        /*check made if image exists in the texture map*/
        if (textureMap.count(str))
        {
            SDL_Rect sizeRect;
            sizeRect.x = 0;
            sizeRect.y = 0;
            sizeRect.w = surfaceMap[str]->w;
            sizeRect.h = surfaceMap[str]->h;

            SDL_Rect posRect;
            posRect.x = posX;
            posRect.y = posY;
            posRect.w = sizeRect.w;
            posRect.h = sizeRect.h;

            SDL_RenderCopyEx(m_renderer, textureMap[str], &sizeRect, &posRect, angle, nullptr, SDL_FLIP_NONE);

            return true;
        }
        else
        {
            LoadImage(img);
            DrawImage(img, posX, posY);
        }
    }else
    {
        LoadImage(img);
        DrawImage(img, posX, posY);
    }
}

bool Drawer::DrawText(const char* font, const char* text, float size, int posX, int posY)
{
    STBTTF_Font* stbFont = STBTTF_OpenFont(m_renderer, font, size);
    if (stbFont)
    {
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
        STBTTF_RenderText(m_renderer, stbFont, posX, posY, text);

        STBTTF_CloseFont(stbFont);
        return true;
    }
    else
    {
        SDL_Log("Error loading font: %s", font);
        return false;
    }
}
