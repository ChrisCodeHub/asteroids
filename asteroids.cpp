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


static Values values;
static std::vector<Shots> shoots;
static std::vector<Asteroid> asteroids;

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
        asteroids.emplace_back(i, values.screenWidth, values.screenHeight, values.MeteorsSpeed, big);
    }

    // todo make this into a std::unique_ptr, add variable in at construction
    Ship ship(values.screenWidth, values.screenHeight, values.PlayerBaseSize);
    
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

            
            for (auto &rock : asteroids)
            {
                if (CheckCollisionCircles((Vector2){ship.collider.x, ship.collider.y}, ship.collider.z, rock.position, rock.radius)) values.gameOver = true;
            }


            //meteor logic
            // TODO - there is a beter way to apply a function to each and every entry in a vector
            // this works, but is a bit verbose to type

            for (auto &rock : asteroids)
            {                
                rock.updateposition(values.screenWidth, values.screenHeight);
            }

            for (auto &shot: shoots)
            {
                if ((shot.active))
                {
                    for (auto &rock : asteroids)
                    {   
                        if (CheckCollisionCircles(shot.position, shot.radius, rock.position, rock.radius))
                        {
                            shot.active = false;
                            rock.active = false;                            
                            if ((rock.rockType == big) || (rock.rockType == medium))
                            {
                                Vector2 position = (Vector2){rock.position.x, rock.position.y};
                                for (int j = 0; j < 2; j ++)
                                {
                                    Vector2 speed{0,0};
                                    speed = (j == 0)? gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, true)
                                                    : gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, false);
                                    
                                    
                                    if (rock.rockType == big) 
                                    {
                                        asteroids.emplace_back(position, speed, 20, medium);
                                    }
                                    else if (rock.rockType == medium)
                                    {
                                        asteroids.emplace_back(position, speed, 10, small);
                                    }                                   
                                }
                            }
                        }
                    }
                }
            }
        }

        asteroids.erase( std::remove_if( asteroids.begin(), 
                            asteroids.end(),
                            [](Asteroid const & r) { return r.active == false; }
                           ), asteroids.end());
        
        values.victory = (0 == (asteroids.size()));
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
            for (const auto &rock : asteroids)
            {
                DrawCircleV(rock.position, rock.radius, rock.color);
            }

            // Draw shots            
            for (const auto &shot: shoots)
            {
               DrawCircleV(shot.position, shot.radius, shot.color);
            }

            if (values.victory)
            {
                DrawText("Victory", values.screenWidth/2 - MeasureText("Victory", 20)/2, values.screenHeight/2, 20, LIGHTGRAY);
            }
            if (values.pause)
            {
                DrawText("GAME paused", values.screenWidth/2 - MeasureText("GAME paused", 40)/2, values.screenHeight/2 - 40, 40, GRAY);
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