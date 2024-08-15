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
#include "gameInfo.hpp"
#include "ship.hpp"
#include <memory>
#include <cstdint>
#include <iostream>
#include <algorithm>


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif


//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void DrawGame(std::shared_ptr<GameInfo> gameInfo);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(std::shared_ptr<GameInfo> gameInfo);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    std::shared_ptr<GameInfo> game = std::make_shared<GameInfo>();

    // Providing a seed value to teh random number generator
	srand((unsigned) time(NULL));
    auto seed = (rand() % 10);
    SetRandomSeed(seed);

    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(game->values.screenWidth, game->values.screenHeight, "classic game: asteroids");

    game->InitGame();

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
        UpdateDrawFrame(game);
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

// Draw game (one frame)
void DrawGame(std::shared_ptr<GameInfo> gameInfo)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gameInfo->values.gameOver)
        {
            // Draw spaceship
            ShipCorners Points = gameInfo->ship->getShipCorners(gameInfo->values.PlayerBaseSize);
            DrawTriangle(Points.V1, Points.V2, Points.V3, gameInfo->ship->color);

            // Draw meteors
            for (const auto &rock : gameInfo->asteroids)
            {
                DrawCircleV(rock->position, rock->radius, rock->color);
            }

            // Draw shots            
            for (const auto &shot: gameInfo->shoots)
            {
               DrawCircleV(shot.position, shot.radius, shot.color);
            }

            if (gameInfo->values.victory)
            {
                DrawText("Victory", gameInfo->values.screenWidth/2 - MeasureText("Victory", 20)/2, gameInfo->values.screenHeight/2, 20, LIGHTGRAY);
            }
            if (gameInfo->values.pause)
            {
                DrawText("GAME paused", gameInfo->values.screenWidth/2 - MeasureText("GAME paused", 40)/2, gameInfo->values.screenHeight/2 - 40, 40, GRAY);
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
void UpdateDrawFrame(std::shared_ptr<GameInfo> gameInfo)
{
    gameInfo->UpdateGame();
    DrawGame(gameInfo);
}