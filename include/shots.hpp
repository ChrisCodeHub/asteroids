#pragma once
#include "raylib.h"
#include <cstdint>

class Shots {

public:
    Shots(float playerRotation, float playerSpeed,  Vector2 playerPosition);
    void initialiseShot(void);
public:
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;
};