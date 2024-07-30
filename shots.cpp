#include "shots.hpp"
#include "raylib.h"
#include <cstdint>
#include <iostream>
#include <math.h>



Shots::Shots(float playerRotation, float playerSpeed,  Vector2 playerPosition)
{
    position = playerPosition;

    speed.x = 1.5*sin(playerRotation*DEG2RAD)*playerSpeed;
    speed.y = 1.5*cos(playerRotation*DEG2RAD)*playerSpeed;
    radius = 2;
    rotation = playerRotation;
    lifeSpawn = 0;
    active = true;
    color = BLACK;
}

void Shots::initialiseShot(void)
{
    position = {0,0};
    Vector2 speed = {0,0};
    float radius = 2;
    float rotation = 0;
    int lifeSpawn = 0;
    bool active = false;
    Color color = BLACK;
}
