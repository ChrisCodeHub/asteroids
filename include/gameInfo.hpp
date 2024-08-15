#pragma once



#include <vector>
#include <math.h>
#include <vector>
#include "meteor.hpp"
#include "shots.hpp"
#include "gameUtils.hpp"
#include "ship.hpp"
#include <memory>
#include <cstdint>
#include <iostream>
#include <algorithm>


struct Values{
    float PlayerBaseSize;
    float PlayerSpeed;
    uint32_t PlayerMaxShoots;
    uint32_t MeteorsSpeed;
    uint32_t MaxBigMeteors;
    int screenWidth;
    int screenHeight;
    bool gameOver;
    bool pause;
    bool victory;

    Values(): PlayerBaseSize{20.0},
              PlayerSpeed{6.0},
              PlayerMaxShoots{200},
              MeteorsSpeed{2},
              MaxBigMeteors{4},
              screenWidth{800},
              screenHeight{450},
              gameOver{false},
              pause{false},
              victory{false}
              {};
};



class GameInfo
{

    public:
            GameInfo(void);
            void UpdateGame(void);
            void InitGame(void);

            Values values;
            std::vector<Shots> shoots;
            std::vector<std::unique_ptr<Asteroid>> asteroids;
            std::unique_ptr<Ship> ship;


};