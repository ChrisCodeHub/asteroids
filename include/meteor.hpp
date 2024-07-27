#pragma once
#include "raylib.h"
#include <cstdint>

/*
This class is the basis of all the meteors we will need, its an abstract base class so 
can be made into concrete 
*/

class Asteroid{

public:
    Asteroid(void);
    void initialiseAMeteor(uint32_t rockNumber, const int screenWidth, const int screenHeight, uint32_t METEORS_SPEED);
    void initialiseAMeteor(uint32_t rockNumber, bool active);
    void initialiseAMeteor(Vector2 position, Vector2 speed,float radius);
    void updateposition(const int screenWidth, const int screenHeight);
    uint32_t rockNum;
public:
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;
    

};