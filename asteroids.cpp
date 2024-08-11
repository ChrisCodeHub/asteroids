/*******************************************************************************************
*
*   raylib - classic game: asteroids
*
*   Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria
*
*   This game has been created using raylib v1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

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


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif


struct Values{
    float PlayerBaseSize = 20.0f;
    float PlayerSpeed     = 6.0;
    uint32_t PlayerMaxShoots = 200;
    uint32_t MeteorsSpeed = 2;
    uint32_t MaxBigMeteors = 4;
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


static Values values;
static std::vector<Shots> shoots;
static std::vector<Asteroid> bigAsteroids;
static std::vector<Asteroid> mediumAsteroids;
static std::vector<Asteroid> smallAsteroids;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(Ship &ship);         // Initialize game
static void UpdateGame(Ship &ship);       // Update game (one frame)
static void DrawGame(Ship &ship);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(Ship &ship);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    // Providing a seed value to teh random number generator
	srand((unsigned) time(NULL));
    auto seed = (rand() % 10);
    SetRandomSeed(seed);


   // start to create t} Player;he low level blocks we will need 
   // which is largely just teh frist set if large asteroids
   // todo make these using make_unique, pass in variables to constructor
    
    for (uint32_t i = 0; i < values.MaxBigMeteors; i++)
    {
        Asteroid bigAsteroid;
        bigAsteroid.initialiseAMeteor(i, values.screenWidth, values.screenHeight, values.MeteorsSpeed, big);
        bigAsteroids.push_back(bigAsteroid);
    }

    // todo make this into a std::unique_ptr, add variable in at construction
    Ship ship;
    ship.initShip(values.screenWidth, values.screenHeight, values.PlayerBaseSize);
    
    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(values.screenWidth, values.screenHeight, "classic game: asteroids");

    InitGame(ship);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame(ship);
        //----------------------------------------------------------------------------------
    }
#endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models...)

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(Ship &ship)
{
    values.victory = false;
    values.pause = false;
    
}

// Update game (one frame)
void UpdateGame(Ship &ship)
{
    if (!values.gameOver)
    {
        if (IsKeyPressed('P')) 
        {
            values.pause = !values.pause;
        }

        if (!values.pause)
        {
            ship.updateShipPosition(values.screenWidth, values.screenHeight, values.PlayerSpeed);

            // Player shoot logic
            // create new Shots upto values.PlayerMaxShoots as player presesd SPACE key 
            if (IsKeyPressed(KEY_SPACE))
            {
                
                if (shoots.size() < values.PlayerMaxShoots)
                {
                    Vector2 playerPosition = ship.getShipPosition();
                    Shots newShot(ship.rotation, values.PlayerSpeed, playerPosition);
                    shoots.push_back(newShot);                   
                }
            }

            // Shoot life timer
            for (auto &shot: shoots)
            {
                shot.updateLife(values.screenHeight, values.screenWidth);
            }

            shoots.erase(std::remove_if( shoots.begin(), 
                            shoots.end(),
                            [](Shots const & p) { return p.active == false; }
                            ), shoots.end());


            // Collision logic: player vs meteors            
            ship.updateCollider();

            for (auto &rock : bigAsteroids)
            {
                gameUtils::hasCollided();
                if (CheckCollisionCircles((Vector2){ship.collider.x, ship.collider.y}, ship.collider.z, rock.position, rock.radius)) values.gameOver = true;
            }

            for (auto &rock : mediumAsteroids)
            {
                if (CheckCollisionCircles((Vector2){ship.collider.x, ship.collider.y}, ship.collider.z, rock.position, rock.radius)) values.gameOver = true;
            }

            for (auto &rock : smallAsteroids)
            {
                if (CheckCollisionCircles((Vector2){ship.collider.x, ship.collider.y}, ship.collider.z, rock.position, rock.radius)) values.gameOver = true;
            }

            //meteor logic
            // TODO - there is a beter way to apply a function to each and every entry in a vector
            // this works, but is a bit verbose to type

            for (auto &asteroid : bigAsteroids)
            {                
                asteroid.updateposition(values.screenWidth, values.screenHeight);
            }

            for (auto &asteroid : mediumAsteroids)
            {
                asteroid.updateposition(values.screenWidth, values.screenHeight);
            }

            for (auto &asteroid : smallAsteroids)
            {
                asteroid.updateposition(values.screenWidth, values.screenHeight);
            }                        

            // Collision logic: player-shoots vs meteors
            //for (int i = 0; i < values.PlayerMaxShoots; i++)
            for (auto &shot: shoots)
            {
                if ((shot.active))
                {
                    for (auto &rock : bigAsteroids)
                    {   
                        if (CheckCollisionCircles(shot.position, shot.radius, rock.position, rock.radius))
                        {
                            shot.active = false;
                            rock.active = false;                            
                            if (rock.rockType == big)
                            {
                                Vector2 position = (Vector2){rock.position.x, rock.position.y};
                                for (int j = 0; j < 2; j ++)
                                {
                                    Vector2 speed{0,0};
                                    speed = (j == 0)? gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, true)
                                                    : gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, false);
                                    Asteroid mediumAsteroid;
                                    mediumAsteroid.initialiseAMeteor(position, speed, 20, medium);
                                    mediumAsteroids.push_back(mediumAsteroid);
                                }
                            }
                        }
                    }

                    for (auto &rock : mediumAsteroids)
                    {
                        if (CheckCollisionCircles(shot.position, shot.radius, rock.position, rock.radius))
                        {
                            shot.active = false;
                            rock.active=false;                            
                            if (rock.rockType == medium)
                            {
                                Vector2 position = (Vector2){rock.position.x, rock.position.y};
                                for (int j = 0; j < 2; j ++)
                                {
                                    Vector2 speed;
                                    speed = ( j == 0)? gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, true)
                                                     : gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, false);
                                    Asteroid smallRock;
                                    smallRock.initialiseAMeteor(position, speed, 10, small);
                                    smallAsteroids.push_back(smallRock);
                                }
                            }
                        }
                    }

                    for (auto &rock : smallAsteroids)
                    {
                        if (CheckCollisionCircles(shot.position, shot.radius, rock.position, rock.radius))
                        {
                            shot.active = false;
                            rock.active = false;                            
                        }
                    }
                }
            }
        }

        bigAsteroids.erase( std::remove_if( bigAsteroids.begin(), 
                            bigAsteroids.end(),
                            [](Asteroid const & r) { return r.active == false; }
                           ), bigAsteroids.end());

        mediumAsteroids.erase( std::remove_if( mediumAsteroids.begin(), 
                            mediumAsteroids.end(),
                            [](Asteroid const & r) { return r.active == false; }
                           ), mediumAsteroids.end());

        smallAsteroids.erase( std::remove_if( smallAsteroids.begin(), 
                            smallAsteroids.end(),
                            [](Asteroid const & r) { return r.active == false; }
                           ), smallAsteroids.end());                           
        
        values.victory = (0 == (bigAsteroids.size() + mediumAsteroids.size() + smallAsteroids.size()) );
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame(ship);
            values.gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(Ship &ship)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!values.gameOver)
        {
            // Draw spaceship
            Vector2 v1 = { ship.position.x + sinf(ship.rotation*DEG2RAD)*(ship.shipHeight), ship.position.y - cosf(ship.rotation*DEG2RAD)*(ship.shipHeight) };
            Vector2 v2 = { ship.position.x - cosf(ship.rotation*DEG2RAD)*(values.PlayerBaseSize/2), ship.position.y - sinf(ship.rotation*DEG2RAD)*(values.PlayerBaseSize/2) };
            Vector2 v3 = { ship.position.x + cosf(ship.rotation*DEG2RAD)*(values.PlayerBaseSize/2), ship.position.y + sinf(ship.rotation*DEG2RAD)*(values.PlayerBaseSize/2) };
            DrawTriangle(v1, v2, v3, ship.color);

            // Draw meteors
            //for (int i = 0; i < bigAsteroids.size(); i++)
            for (const auto &rock : bigAsteroids)
            {
                DrawCircleV(rock.position, rock.radius, rock.color);
            }

            for (const auto &rock : mediumAsteroids)
            {
                DrawCircleV(rock.position, rock.radius, rock.color);
            }

            for (const auto &rock : smallAsteroids)
            {
                DrawCircleV(rock.position, rock.radius, rock.color);
            }

            // Draw shoot
            //for (int i = 0; i < values.PlayerMaxShoots; i++)
            for (const auto &shot: shoots)
            {
               DrawCircleV(shot.position, shot.radius, shot.color);
            }

            if (values.victory)
            {
                DrawText("values.victory", values.screenWidth/2 - MeasureText("values.victory", 20)/2, values.screenHeight/2, 20, LIGHTGRAY);
            }
            if (values.pause)
            {
                DrawText("GAME values.pauseD", values.screenWidth/2 - MeasureText("GAME values.pauseD", 40)/2, values.screenHeight/2 - 40, 40, GRAY);
            }
        }
        else 
        {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
        }

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(Ship &ship)
{
    UpdateGame(ship);
    DrawGame(ship);
}