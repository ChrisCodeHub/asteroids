#pragma once
#include "raylib.h"

/*
This class is the basis of all the meteors we will need, its an abstract base class so 
can be made into concrete 
*/

class Asteroid{

public:
    void initialiseAMeteor(void);

private:
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;

};