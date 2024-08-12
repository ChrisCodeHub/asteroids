#pragma once
#include <cstdint>
#include "raylib.h"


class Ship {

public: 

    Vector2 position;
    Vector2 speed;
    float acceleration;
    float rotation;
    Vector3 collider;
    Color color;

    int shipHeight;

    Ship(const int screenWidth, const int screenHeight, const int playerBaseSize);
    void updateShipPosition(const int screenWidth, const int screenHeight, const float playerSpeed);
    Vector2 getShipPosition(void)const;
    void updateCollider(void);

};