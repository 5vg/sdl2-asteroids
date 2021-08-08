#include "asteroids.h"

#include <SDL.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <string>
#include "drawer.h"

int GenerateSpeed(float multiplier);
void LifeCheck();
void MoveState(double radians, int speed, const Uint8* keystate);
void ShotPositions(int shotspeed);
void ShotAsteroid();
void ShotSmallAsteroid();
void ShipCollision();
void AsteroidPositions();
void OutOfBoundsShots();
void OutOfBoundsAsteroids();
void NewShots(const Uint8* keystate, unsigned long ms);
void NewAsteroids(int speed, float multiplier);
void ScreenWrap();

Asteroids* Asteroids::Create(Drawer* drawer)
{
    Asteroids* newAsteroids = new Asteroids();
    newAsteroids->drawer = drawer;
    return newAsteroids;
}

struct asteroidInformation
{
    int x;
    int y;
    int dirX;
    int dirY;
};

struct shotInformation
{
    int x;
    int y;
    double angle;
    bool mark = false;
};

static bool gameOver = false;
static bool paused = false;
static bool start = false;

static int px = 200;
static int py = 300;

double angle = 0;

static int score = 0;
static int lives = 3;

const double PI = 3.14159265;

unsigned long prevms = 0;

static int gameOverCounter = 0;

static std::vector<shotInformation> shots;
static std::vector<asteroidInformation> asteroids;
static std::vector<asteroidInformation> asteroids1;

void Asteroids::Update(float deltatime)
{
    float frames = 1/deltatime;
    float multiplier = 100 / frames;
    int speed = 5 * multiplier;
    int shotspeed = 2 * speed;

    double radians = angle * PI / 180.0;

    unsigned long ms = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);

    const Uint8* keystate = SDL_GetKeyboardState(nullptr);

    if(keystate[SDL_SCANCODE_ESCAPE] && start)
    {
        if(paused)
        {
            SDL_Delay(150);
            paused = false;
        }
        else
        {
            SDL_Delay(150);
            paused = true;
        }
    }

    if(!start)
    {
        gameOver = false;
        if(keystate[SDL_SCANCODE_RETURN])
        {
            start = true;
            SDL_Delay(20);
            lives = 3;
            score = 0;
            gameOverCounter = 0;
        }
    }

    if(gameOver)
    {
        if(keystate[SDL_SCANCODE_RETURN])
        {
            SDL_Delay(150);
            start = false;
        }
    }

    if(!paused && start)
    {

        if (!gameOver)
        {
            LifeCheck();
        }

        if (!gameOver)
        {
            MoveState(radians, speed, keystate);
        }
        ShotPositions(shotspeed);

        AsteroidPositions();

        ShotAsteroid();

        ShotSmallAsteroid();

        if (!gameOver)
        {
            ShipCollision();
        }
        OutOfBoundsShots();

        OutOfBoundsAsteroids();

        if (!gameOver)
        {
            NewShots(keystate, ms);
        }

        NewAsteroids(speed, multiplier);

        if (!gameOver)
        {
            ScreenWrap();
        }
    }

}

void Asteroids::Draw()
{
    drawer->DrawImage("bg.png");
    for (shotInformation shot : shots)
    {
        drawer->DrawImage("shot.png", shot.x - 14, shot.y - 16, shot.angle);
    }
    for (asteroidInformation asteroid : asteroids)
    {
        drawer->DrawImage("asteroid.png", asteroid.x - 100, asteroid.y - 100);
    }
    for (asteroidInformation asteroid : asteroids1)
    {
        drawer->DrawImage("asteroid1.png", asteroid.x - 100, asteroid.y - 100);
    }

    std::string scoreString = std::to_string(score);
    std::string fullStringScore = "Score: " + scoreString;
    const char* constCharScore = fullStringScore.c_str();

    if(!gameOver && start)
    {
        drawer->DrawImage("ship.png", px - 20, py - 20, angle);

        drawer->DrawText("arial.ttf", constCharScore, 30, 10, 40);

        std::string livesString = std::to_string(lives);
        std::string fullStringLives = "Lives: " + livesString;
        const char* constCharLives = fullStringLives.c_str();
        drawer->DrawText("arial.ttf", constCharLives, 20, 10, 60);
    }

    if(paused)
    {
        drawer->DrawText("arial.ttf", "Game Paused", 80, 175, 100);
        drawer->DrawText("arial.ttf", "Please press 'Esc' to unpause", 30, 225, 150);
    }

    if(!start)
    {
        drawer->DrawText("arial.ttf", "Asteroids", 80, 250, 100);
        drawer->DrawText("arial.ttf", "Please press 'Enter' to start", 30, 230, 150);
        drawer->DrawText("arial.ttf", "To move, use the arrow keys", 30, 225, 550);
        drawer->DrawText("arial.ttf", "To shoot, use the space bar", 30, 230, 580);
    }

    if(gameOver)
    {
        drawer->DrawText("arial.ttf", "Game Over", 80, 200, 100);

        drawer->DrawText("arial.ttf", constCharScore, 30, 325, 200);

        drawer->DrawText("arial.ttf", "Press 'Enter' to play again", 30, 235, 580);

        if(gameOverCounter<16)
        {
            std::string imageNumber = std::to_string(gameOverCounter);
            if (gameOverCounter < 10)
            {
                imageNumber = "0" + imageNumber;
            }
            std::string fileName = "explosion_0" + imageNumber + ".png";
            const char* constCharFileName = fileName.c_str();
            drawer->DrawImage(constCharFileName, px - 32, py - 32);
            SDL_Delay(20);
            gameOverCounter++;
        }

    }

}


int GenerateSpeed(float multiplier)
{
    int speed = ((rand() % 3) * multiplier);
    return speed;
}

void LifeCheck()
{
    if(lives < 1)
    {
        gameOver = true;
    }
}

void MoveState(double radians, int speed, const Uint8* keystate)
{
    if (keystate[SDL_SCANCODE_LEFT])
    {
        angle += 3;
    }
    if (keystate[SDL_SCANCODE_RIGHT])
    {
        angle -= 3;
    }
    if (keystate[SDL_SCANCODE_UP])
    {
        py -= cos(radians) * speed;
        px += sin(radians) * speed;
    }
    if (keystate[SDL_SCANCODE_DOWN])
    {
        py += cos(radians) * speed;
        px -= sin(radians) * speed;
    }
}

void ShotPositions(int shotspeed)
{
    for (shotInformation& shot : shots)
    {
        double shotRad = shot.angle * PI / 180.0;
        shot.y -= cos(shotRad) * shotspeed;
        shot.x += sin(shotRad) * shotspeed;
    }
}

void ShotAsteroid()
{
    int c = 0;
    for (shotInformation shot : shots)
    {
        for (std::vector<asteroidInformation>::iterator itr = asteroids.begin(); itr != asteroids.end(); )
        {
            SDL_Rect* result = new SDL_Rect;

            SDL_Rect asteroidRect;
            asteroidRect.x = (itr->x) - 40;
            asteroidRect.y = (itr->y) - 40;
            asteroidRect.w = 126;
            asteroidRect.h = 165;

            SDL_Rect shotRect;
            shotRect.x = (shot.x) - 14;
            shotRect.y = (shot.y) - 16;
            shotRect.w = 12;
            shotRect.h = 22;


            if (SDL_IntersectRect(&asteroidRect, &shotRect, result))
            {
                asteroids1.push_back(asteroidInformation{ itr->x, itr->y, itr->dirX, itr->dirY });
                asteroids1.push_back(asteroidInformation{ itr->x, itr->y, itr->dirX + 1, itr->dirY + 1 });
                asteroids1.push_back(asteroidInformation{ itr->x, itr->y, itr->dirX - 1, itr->dirY - 1 });
                itr = asteroids.erase(itr);
                shotInformation changedShot{ shot.x, shot.y, shot.angle, true };
                shots[c] = changedShot;
                score++;
            }
            else
            {
                ++itr;
            }
        }
        c++;
    }
}

void ShotSmallAsteroid()
{
    int c1 = 0;
    for (shotInformation shot : shots)
    {
        bool marked = shot.mark;
        if (!marked)
        {
            for (std::vector<asteroidInformation>::iterator itr = asteroids1.begin(); itr != asteroids1.end(); )
            {
                SDL_Rect* result = new SDL_Rect;

                SDL_Rect asteroidRect;
                asteroidRect.x = (itr->x) - 40;
                asteroidRect.y = (itr->y) - 40;
                asteroidRect.w = 64;
                asteroidRect.h = 83;

                SDL_Rect shotRect;
                shotRect.x = (shot.x) - 14;
                shotRect.y = (shot.y) - 16;
                shotRect.w = 12;
                shotRect.h = 22;


                if (SDL_IntersectRect(&asteroidRect, &shotRect, result))
                {
                    itr = asteroids1.erase(itr);
                    shotInformation changedShot{ shot.x, shot.y, shot.angle, true };
                    shots[c1] = changedShot;
                    score++;
                }
                else
                {
                    ++itr;
                }
            }
        }
        c1++;
    }
}

void ShipCollision()
{
    for (std::vector<asteroidInformation>::iterator itr = asteroids.begin(); itr != asteroids.end(); )
    {
        SDL_Rect* result = new SDL_Rect;

        SDL_Rect asteroidRect;
        asteroidRect.x = (itr->x) - 40;
        asteroidRect.y = (itr->y) - 40;
        asteroidRect.w = 126;
        asteroidRect.h = 165;

        SDL_Rect shipRect;
        shipRect.x = px;
        shipRect.y = py;
        shipRect.w = 40;
        shipRect.h = 40;


        if (SDL_IntersectRect(&asteroidRect, &shipRect, result))
        {
            lives -= 1;
            itr = asteroids.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

    for (std::vector<asteroidInformation>::iterator itr = asteroids1.begin(); itr != asteroids1.end(); )
    {
        SDL_Rect* result = new SDL_Rect;

        SDL_Rect asteroidRect;
        asteroidRect.x = (itr->x) - 20;
        asteroidRect.y = (itr->y) - 20;
        asteroidRect.w = 64;
        asteroidRect.h = 83;

        SDL_Rect shipRect;
        shipRect.x = px;
        shipRect.y = py;
        shipRect.w = 40;
        shipRect.h = 40;


        if (SDL_IntersectRect(&asteroidRect, &shipRect, result))
        {
            lives -= 1;
            itr = asteroids1.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void AsteroidPositions()
{
    for (asteroidInformation& asteroid : asteroids)
    {
        asteroid.y -= asteroid.dirY;
        asteroid.x -= asteroid.dirX;
    }

    for (asteroidInformation& asteroid : asteroids1)
    {
        asteroid.y -= asteroid.dirY;
        asteroid.x -= asteroid.dirX;
    }
}

void OutOfBoundsShots()
{
    for (std::vector<shotInformation>::iterator itr = shots.begin(); itr != shots.end(); )
    {
        if (itr->mark == true || itr->y > 600 || itr->y < 0 || itr->x > 800 || itr->x < 0)
        {
            itr = shots.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void OutOfBoundsAsteroids()
{
    for (std::vector<asteroidInformation>::iterator itr = asteroids.begin(); itr != asteroids.end(); )
    {
        if (itr->y > 800 || itr->y < -200 || itr->x > 1000 || itr->x < -200)
        {
            itr = asteroids.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

    for (std::vector<asteroidInformation>::iterator itr = asteroids1.begin(); itr != asteroids1.end(); )
    {
        if (itr->y > 800 || itr->y < -200 || itr->x > 1000 || itr->x < -200)
        {
            itr = asteroids1.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void NewShots(const Uint8* keystate, unsigned long ms)
{
    if (keystate[SDL_SCANCODE_SPACE])
    {
        if (ms - prevms >= 100)
        {
            shots.push_back(shotInformation{ px, py , angle });
            prevms = ms;
        }
    }
}

void NewAsteroids(int speed, float multiplier)
{
    if (asteroids.size() < 2)
    {
        int pos = rand() % 4;
        if (pos == 0)
        {
            asteroids.push_back(asteroidInformation{ 1000, rand() % 600, speed, GenerateSpeed(multiplier) });
        }
        if (pos == 1)
        {
            asteroids.push_back(asteroidInformation{ -150, rand() % 600, -speed, GenerateSpeed(multiplier) });
        }
        if (pos == 2)
        {
            asteroids.push_back(asteroidInformation{ rand() % 800, 800, GenerateSpeed(multiplier) , speed });
        }
        if (pos == 3)
        {
            asteroids.push_back(asteroidInformation{ rand() % 800, -150, GenerateSpeed(multiplier), -speed });
        }

    }
}

void ScreenWrap()
{
    if (px > 800)
    {
        px = 0;
    }
    if (px < 0)
    {
        px = 800;
    }
    if (py > 600)
    {
        py = 0;
    }
    if (py < 0)
    {
        py = 600;
    }
}
