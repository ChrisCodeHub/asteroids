#include "ship.hpp"
#include "raylib.h"
#include "gameUtils.hpp"
#include <iostream>
#include <cstdint>
#include "math.h"
#include <algorithm>

void Ship::updateCollider(void)
{
    collider = (Vector3){position.x + sin(rotation*DEG2RAD)*(shipHeight/2.5f),
                         position.y - cos(rotation*DEG2RAD)*(shipHeight/2.5f), 12};
}


Ship::Ship(const int screenWidth, const int screenHeight, const int playerBaseSize)
{
    shipHeight = (playerBaseSize/2)/tanf(20*DEG2RAD);

    // Initialization player
    position = (Vector2){screenWidth/2, screenHeight/2 - shipHeight/2};
    speed = (Vector2){0, 0};
    acceleration = 0;
    rotation = 0;    
    color = MAROON;
    updateCollider();
}



void Ship::updateShipPosition(const int screenWidth, const int screenHeight, const float playerSpeed)
{

    // Player logic: rotation
    if (IsKeyDown(KEY_LEFT))  rotation -= 5;
    if (IsKeyDown(KEY_RIGHT)) rotation += 5;

    // Player logic: speed
    speed = gameUtils::calcNewSpeed(rotation, playerSpeed, false);

    // Player logic: acceleration
    if (IsKeyDown(KEY_UP))
    {
        if (acceleration < 1) acceleration += 0.04f;
    }
    else
    {
        if (acceleration > 0) acceleration -= 0.02f;
        else if (acceleration < 0) acceleration = 0;
    }
    if (IsKeyDown(KEY_DOWN))
    {
        if (acceleration > 0) acceleration -= 0.04f;
        else if (acceleration < 0) acceleration = 0;
    }


    // Player logic: movement
    position.x += (speed.x*acceleration);
    position.y -= (speed.y*acceleration);

    // Collision logic: player vs walls
    if (position.x > screenWidth + shipHeight) 
    {
        position.x = -(shipHeight);
    }
    else if (position.x < -(shipHeight)) 
    {
        position.x = screenWidth + shipHeight;
    }

    if (position.y > (screenHeight + shipHeight)) 
    {
        position.y = -(shipHeight);
    }
    else if (position.y < -(shipHeight)) 
    {
        position.y = screenHeight + shipHeight;
    } 
}

Vector2 Ship::getShipPosition(void) const
{
    Vector2 shipPosition = (Vector2){ position.x + sin(rotation*DEG2RAD)*(shipHeight), 
                                  position.y - cos(rotation*DEG2RAD)*(shipHeight) };
    
    return shipPosition;
}