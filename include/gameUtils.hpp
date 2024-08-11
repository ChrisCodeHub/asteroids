#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include "meteor.hpp"
#include "shots.hpp"
#include "raylib.h"



//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------




//  this isutility class that just contains static functions that can be applied in the main game loop
//  Th eidea is not to instantiate the class, rather to just use it as a holdingpen/namespace for static helpers
class gameUtils 
{

    public:
        static bool hasCollided(void);
        //static Vector2 calcNewSpeed(float rotation, uint32_t speedConst, bool negativeDirection);
        static Vector2 calcNewSpeed(float &rotation, uint32_t speedConst, bool negativeDirection);        

};

