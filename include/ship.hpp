#pragma once
#include <cstdint>
#include "raylib.h"

typedef struct{
    Vector2 V1;
    Vector2 V2;
    Vector2 V3;    
}ShipCorners;


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
    ShipCorners getShipCorners(float PlayerBaseSize);

};