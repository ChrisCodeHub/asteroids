#include "gameUtils.hpp"
#include "raylib.h"
#include <math.h>

#include <iostream>


bool gameUtils::hasCollided(void)
{
    std::cout<< "check for collision" << std::endl;
    return false;
}


Vector2 gameUtils::calcNewSpeed(float &rotation, uint32_t speedConst, bool negativeDirection)
{   
    Vector2 newSpeed{0, 0};
    newSpeed = (Vector2){sin(rotation*DEG2RAD)*speedConst, cos(rotation*DEG2RAD)*speedConst,};
    if (negativeDirection == true)
    {
        std::cout<<" negate speed "<< negativeDirection << " " << std::endl;
        newSpeed.x *= -1;
        newSpeed.y *= -1;
    }

    return newSpeed;
}