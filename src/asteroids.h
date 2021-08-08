#pragma once

class Drawer;

class Asteroids
{
public:
    static Asteroids* Create(Drawer* drawer);

    void Update(float deltaTime);
    void Draw();


private:
    Drawer* drawer;
};
