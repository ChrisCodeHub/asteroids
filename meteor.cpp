#include "raylib.h"
#include "meteor.hpp"
#include <cstdint>
#include <iostream>


Asteroid::Asteroid(void)
{
    std::cout<<" made me an asteroid"<<std::endl;
}

void Asteroid::initialiseAMeteor(uint32_t rockNumber, bool active)
{
    position = (Vector2){-100, -100};
    speed = (Vector2){0,0};
    radius = 10;
    active = false;
    color = GREEN;
}

void Asteroid::initialiseAMeteor(uint32_t rockNumber, const int screenWidth, const int screenHeight, uint32_t MeteorSpeed, RockType meteorType)
{
    std::cout<< " setting to " << rockNumber << "    " ;
    rockNum = rockNumber;
    int posx, posy;
    int velx, vely;
    bool correctRange{false};

    // TODO
    // create lambda(s) for this
    posx = 0 + (rand() % screenWidth);//  GetRandomValue(0, screenWidth);
    posy = 0 + (rand() % screenHeight); //GetRandomValue(0, screenHeight);

    while (!correctRange)
    {
        if (posx > screenWidth/2 - 150 && posx < screenWidth/2 + 150) 
            posx = 0 + (rand() % screenWidth);// GetRandomValue(0, screenWidth);
        else 
            correctRange = true;
    }

    correctRange = false;
    while (!correctRange)
    {
        if (posy > screenHeight/2 - 150 && posy < screenHeight/2 + 150)  
            posy = 0 + (rand() % screenHeight); // GetRandomValue(0, screenHeight);
        else 
            correctRange = true;
    }

    position = (Vector2){posx, posy};

    correctRange = false;
    velx = GetRandomValue(-MeteorSpeed, MeteorSpeed);
    vely = GetRandomValue(-MeteorSpeed, MeteorSpeed);

    while (!correctRange)
    {
        if (velx == 0 && vely == 0)
        {
            velx = GetRandomValue(-MeteorSpeed, MeteorSpeed);
            vely = GetRandomValue(-MeteorSpeed, MeteorSpeed);
        }
        else correctRange = true;
    }

    speed = (Vector2){velx, vely};
    radius = 40;
    active = true;
    color = BLUE;
    rockType = meteorType;
}

void Asteroid::initialiseAMeteor(Vector2 positionIn, Vector2 speedIn,float radiusIn, RockType meteorType)
{
        position = positionIn;
        speed = speedIn;
        active = true;
        radius = radiusIn;
        color = BLUE;
        rockType = meteorType;
}

void Asteroid::updateposition(const int screenWidth, const int screenHeight)
{
    if (active)
    {
        // Movement
        position.x += speed.x;
        position.y += speed.y;

        // Collision logic: meteor vs edge of window
        if  (position.x > screenWidth + radius) 
            position.x = -radius;
        else if (position.x < 0 - radius) 
            position.x = screenWidth + radius;

        if (position.y > screenHeight + radius) 
            position.y = -radius;
        else if (position.y < 0 - radius) 
            position.y = screenHeight + radius;
    }
}