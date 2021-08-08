#include <SDL.h>
#include <stdio.h>

#include "drawer.h"
#include "asteroids.h"

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    const int width = 800;
    const int height = 600;
    SDL_Window* window = SDL_CreateWindow(
        "Asteroids",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN);

    if (window == nullptr)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    Drawer* drawer = Drawer::Create(window, renderer);
    Asteroids* asteroids = Asteroids::Create(drawer);

    float lastFrame = (float) SDL_GetTicks() * 0.001f;

    while (true)
    {
        SDL_Event currentEvent;
        SDL_PollEvent(&currentEvent);

        float currentFrame = (float) SDL_GetTicks() * 0.001f;
        float elapsedTime = currentFrame - lastFrame;

        SDL_RenderClear(renderer);

        asteroids->Update(elapsedTime);
        asteroids->Draw();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        lastFrame = currentFrame;

        int framerate = 1 / elapsedTime;
        printf("\r                               ");
        printf("\r fps: %d", framerate);

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    delete asteroids;
    delete drawer;

    return 0;
}
