#pragma once
#include <map>
#include <string>
#include <SDL.h>

struct SDL_Window;
struct SDL_Renderer;

class Drawer
{
public:
    static Drawer* Create(SDL_Window* window, SDL_Renderer* renderer);

    bool DrawImage(const char* imagePath, int posX = 0, int posY = 0, double angle = 0);
    bool DrawText(const char* font, const char* text, float size, int posX, int posY);

    void LoadImage(const char* imagePath);



private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    std::map<std::string, SDL_Surface*> surfaceMap;
    std::map<std::string, SDL_Texture*> textureMap;

};
