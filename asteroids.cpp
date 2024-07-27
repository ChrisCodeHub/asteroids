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
#include <memory>
#include <cstdint>
#include <iostream>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define PLAYER_BASE_SIZE    20.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_MAX_SHOOTS   10

uint32_t constexpr METEORS_SPEED = 2;
uint32_t constexpr MAX_BIG_METEORS = 4;
uint32_t constexpr MAX_MEDIUM_METEORS = 8;
uint32_t constexpr MAX_SMALL_METEORS = 16;

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    Vector2 speed;
    float acceleration;
    float rotation;
    Vector3 collider;
    Color color;
} Player;

typedef struct Shoot {
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;
} Shoot;

typedef struct Meteor {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Color color;
} Meteor;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static bool gameOver = false;
static bool pause = false;
static bool victory = false;

// NOTE: Defined triangle is isosceles with common angles of 70 degrees.
static float shipHeight = 0.0f;

static Player player = { 0 };
static Shoot shoot[PLAYER_MAX_SHOOTS] = { 0 };
static Meteor bigMeteor[MAX_BIG_METEORS] = { 0 };
static Meteor mediumMeteor[MAX_MEDIUM_METEORS] = { 0 };
static Meteor smallMeteor[MAX_SMALL_METEORS] = { 0 };
static std::vector<Asteroid> bigAsteroids;
static std::vector<Asteroid> mediumAsteroids;
static std::vector<Asteroid> smallAsteroids;


static int midMeteorsCount = 0;
static int smallMeteorsCount = 0;
static int destroyedMeteorsCount = 0;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    // Providing a seed value to teh random number generator
	srand((unsigned) time(NULL));
    auto seed = (rand() % 10);
    SetRandomSeed(seed);


   // start to create the low level blocks we will need (asteroids)    
    for (uint32_t i = 0; i < MAX_BIG_METEORS; i++)
    {
        Asteroid bigAsteroid;
        bigAsteroid.initialiseAMeteor(i, screenWidth, screenHeight, METEORS_SPEED);
        bigAsteroids.push_back(bigAsteroid);
    }
    

    // for (uint32_t i = 0; i < MAX_MEDIUM_METEORS; ++i)
    // {
    //     Asteroid mediumAsteroid;
    //     bool active=false;
    //     mediumAsteroid.initialiseAMeteor(i, active);
    //     mediumAsteroids.push_back(mediumAsteroid);
    // }
    // for (uint32_t i = 0; i < MAX_SMALL_METEORS; ++i)
    // {
    //     Asteroid smallAsteroid;
    //     bool active=false;
    //     smallAsteroid.initialiseAMeteor(i, active);
    //     smallAsteroids.push_back(smallAsteroid);
    // }




    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "classic game: asteroids");

    InitGame();

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
        UpdateDrawFrame();
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
void InitGame(void)
{
    int posx, posy;
    int velx, vely;
    bool correctRange = false;
    victory = false;
    pause = false;

    shipHeight = (PLAYER_BASE_SIZE/2)/tanf(20*DEG2RAD);

    // Initialization player
    player.position = (Vector2){screenWidth/2, screenHeight/2 - shipHeight/2};
    player.speed = (Vector2){0, 0};
    player.acceleration = 0;
    player.rotation = 0;
    player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight/2.5f), 12};
    player.color = LIGHTGRAY;

    destroyedMeteorsCount = 0;

    // Initialization shoot
    for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
    {
        shoot[i].position = (Vector2){0, 0};
        shoot[i].speed = (Vector2){0, 0};
        shoot[i].radius = 2;
        shoot[i].active = false;
        shoot[i].lifeSpawn = 0;
        shoot[i].color = WHITE;
    }

    for (int i = 0; i < MAX_BIG_METEORS; i++)
    {
        posx = GetRandomValue(0, screenWidth);

        while (!correctRange)
        {
            if (posx > screenWidth/2 - 150 && posx < screenWidth/2 + 150) posx = GetRandomValue(0, screenWidth);
            else correctRange = true;
        }

        correctRange = false;

        posy = GetRandomValue(0, screenHeight);

        while (!correctRange)
        {
            if (posy > screenHeight/2 - 150 && posy < screenHeight/2 + 150)  posy = GetRandomValue(0, screenHeight);
            else correctRange = true;
        }

        bigMeteor[i].position = (Vector2){posx, posy};

        correctRange = false;
        velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
        vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);

        while (!correctRange)
        {
            if (velx == 0 && vely == 0)
            {
                velx = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
                vely = GetRandomValue(-METEORS_SPEED, METEORS_SPEED);
            }
            else correctRange = true;
        }

        bigMeteor[i].speed = (Vector2){velx, vely};
        bigMeteor[i].radius = 40;
        bigMeteor[i].active = true;
        bigMeteor[i].color = BLUE;
    }

    for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
    {
        mediumMeteor[i].position = (Vector2){-100, -100};
        mediumMeteor[i].speed = (Vector2){0,0};
        mediumMeteor[i].radius = 20;
        mediumMeteor[i].active = false;
        mediumMeteor[i].color = RED;
    }

    for (int i = 0; i < MAX_SMALL_METEORS; i++)
    {
        smallMeteor[i].position = (Vector2){-100, -100};
        smallMeteor[i].speed = (Vector2){0,0};
        smallMeteor[i].radius = 10;
        smallMeteor[i].active = false;
        smallMeteor[i].color = GREEN;
    }

    midMeteorsCount = 0;
    smallMeteorsCount = 0;
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Player logic: rotation
            if (IsKeyDown(KEY_LEFT)) player.rotation -= 5;
            if (IsKeyDown(KEY_RIGHT)) player.rotation += 5;

            // Player logic: speed
            player.speed.x = sin(player.rotation*DEG2RAD)*PLAYER_SPEED;
            player.speed.y = cos(player.rotation*DEG2RAD)*PLAYER_SPEED;

            // Player logic: acceleration
            if (IsKeyDown(KEY_UP))
            {
                if (player.acceleration < 1) player.acceleration += 0.04f;
            }
            else
            {
                if (player.acceleration > 0) player.acceleration -= 0.02f;
                else if (player.acceleration < 0) player.acceleration = 0;
            }
            if (IsKeyDown(KEY_DOWN))
            {
                if (player.acceleration > 0) player.acceleration -= 0.04f;
                else if (player.acceleration < 0) player.acceleration = 0;
            }

            // Player logic: movement
            player.position.x += (player.speed.x*player.acceleration);
            player.position.y -= (player.speed.y*player.acceleration);

            // Collision logic: player vs walls
            if (player.position.x > screenWidth + shipHeight) player.position.x = -(shipHeight);
            else if (player.position.x < -(shipHeight)) player.position.x = screenWidth + shipHeight;
            if (player.position.y > (screenHeight + shipHeight)) player.position.y = -(shipHeight);
            else if (player.position.y < -(shipHeight)) player.position.y = screenHeight + shipHeight;

            // Player shoot logic
            if (IsKeyPressed(KEY_SPACE))
            {
                for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
                {
                    if (!shoot[i].active)
                    {
                        shoot[i].position = (Vector2){ player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight) };
                        shoot[i].active = true;
                        shoot[i].speed.x = 1.5*sin(player.rotation*DEG2RAD)*PLAYER_SPEED;
                        shoot[i].speed.y = 1.5*cos(player.rotation*DEG2RAD)*PLAYER_SPEED;
                        shoot[i].rotation = player.rotation;
                        break;
                    }
                }
            }

            // Shoot life timer
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active) shoot[i].lifeSpawn++;
            }

            // Shot logic
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active)
                {
                    // Movement
                    shoot[i].position.x += shoot[i].speed.x;
                    shoot[i].position.y -= shoot[i].speed.y;

                    // Collision logic: shoot vs walls
                    if  (shoot[i].position.x > screenWidth + shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    else if (shoot[i].position.x < 0 - shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    if (shoot[i].position.y > screenHeight + shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }
                    else if (shoot[i].position.y < 0 - shoot[i].radius)
                    {
                        shoot[i].active = false;
                        shoot[i].lifeSpawn = 0;
                    }

                    // Life of shoot
                    if (shoot[i].lifeSpawn >= 60)
                    {
                        shoot[i].position = (Vector2){0, 0};
                        shoot[i].speed = (Vector2){0, 0};
                        shoot[i].lifeSpawn = 0;
                        shoot[i].active = false;
                    }
                }
            }

            // Collision logic: player vs meteors
            player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(shipHeight/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(shipHeight/2.5f), 12};

            for (int a = 0; a < MAX_BIG_METEORS; a++)
            {
                if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, bigMeteor[a].position, bigMeteor[a].radius) && bigMeteor[a].active) gameOver = true;
            }

            for (int a = 0; a < MAX_MEDIUM_METEORS; a++)
            {
                if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, mediumMeteor[a].position, mediumMeteor[a].radius) && mediumMeteor[a].active) gameOver = true;
            }

            for (int a = 0; a < smallAsteroids.size(); a++)
            {
                if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, smallAsteroids[a].position, smallAsteroids[a].radius) && smallAsteroids[a].active) gameOver = true;
            }

            //meteor logic
            // TODO - there is a beter way to apply a function to each and every entry in a vector
            // this works, but is a bit verbose to type
            int i = 0;
            for (auto &asteroid : bigAsteroids)
            {                
                asteroid.updateposition(screenWidth, screenHeight);
                bigMeteor[i].position.x = asteroid.position.x;
                bigMeteor[i].position.y = asteroid.position.y;
                i++;
            }

            i = 0;
            for (auto &asteroid : mediumAsteroids)
            {
                asteroid.updateposition(screenWidth, screenHeight);
                mediumMeteor[i].position.x = asteroid.position.x;
                mediumMeteor[i].position.y = asteroid.position.y;
                mediumMeteor[i].active = asteroid.active;
                i++;                
            }

            i = 0;
            for (auto &asteroid : smallAsteroids)
            {
                asteroid.updateposition(screenWidth, screenHeight);
                smallMeteor[i].position.x = asteroid.position.x;
                smallMeteor[i].position.y = asteroid.position.y;
                smallMeteor[i].active = asteroid.active;
                i++;
            }                        

            // Collision logic: player-shoots vs meteors
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if ((shoot[i].active))
                {
                    for (int a = 0; a < MAX_BIG_METEORS; a++)
                    {   
                        // if (bigAsteroids[a].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, 
                        //                                                     bigAsteroids[a].position, bigAsteroids[a].radius))
                        if (bigMeteor[a].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, bigMeteor[a].position, bigMeteor[a].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            bigMeteor[a].active = false;
                            bigAsteroids[a].active = false;
                            destroyedMeteorsCount++;

                            for (int j = 0; j < 2; j ++)
                            {
                                Vector2 position = (Vector2){bigMeteor[a].position.x, bigMeteor[a].position.y};
                                Vector2 speed{0,0};
                                if (midMeteorsCount%2 == 0)
                                {                                    
                                    speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1};
                                }
                                else
                                {
                                    speed = (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED, sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED};                                
                                }
                                Asteroid mediumAsteroid;
                                mediumAsteroid.initialiseAMeteor(position, speed, 20);
                                mediumAsteroids.push_back(mediumAsteroid);
                                midMeteorsCount ++;
                            }
                            bigMeteor[a].color = RED;
                            a = MAX_BIG_METEORS;
                        }
                    }

                    for (int b = 0; b < mediumAsteroids.size(); b++)
                    {
                        if (mediumAsteroids[b].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius, 
                                                                               mediumAsteroids[b].position, mediumAsteroids[b].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            mediumAsteroids[b].active=false;
                            destroyedMeteorsCount++;

                            for (int j = 0; j < 2; j ++)
                            {
                                Vector2 position = (Vector2){mediumAsteroids[b].position.x, mediumAsteroids[b].position.y};
                                Vector2 speed;
                                speed = (smallMeteorsCount%2 == 0) ? (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1, 
                                                                               sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED*-1}
                                                                   :  (Vector2){cos(shoot[i].rotation*DEG2RAD)*METEORS_SPEED, 
                                                                                sin(shoot[i].rotation*DEG2RAD)*METEORS_SPEED};
                                Asteroid smallRock;
                                smallRock.initialiseAMeteor(position, speed, 10);
                                smallAsteroids.push_back(smallRock);
                                smallMeteorsCount++;
                            }                            
                        }
                    }

                    for (int c = 0; c < smallAsteroids.size() ; c++)
                    {
                        if (smallAsteroids[c].active && CheckCollisionCircles(shoot[i].position, shoot[i].radius,
                                                                              smallAsteroids[c].position, smallAsteroids[c].radius))
                        {
                            shoot[i].active = false;
                            shoot[i].lifeSpawn = 0;
                            smallAsteroids[c].active = false;
                            destroyedMeteorsCount++;
                        }
                    }
                }
            }
        }

        if (destroyedMeteorsCount == MAX_BIG_METEORS + MAX_MEDIUM_METEORS + MAX_SMALL_METEORS) victory = true;
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

        ClearBackground(RAYWHITE);

        if (!gameOver)
        {
            // Draw spaceship
            Vector2 v1 = { player.position.x + sinf(player.rotation*DEG2RAD)*(shipHeight), player.position.y - cosf(player.rotation*DEG2RAD)*(shipHeight) };
            Vector2 v2 = { player.position.x - cosf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2), player.position.y - sinf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2) };
            Vector2 v3 = { player.position.x + cosf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2), player.position.y + sinf(player.rotation*DEG2RAD)*(PLAYER_BASE_SIZE/2) };
            DrawTriangle(v1, v2, v3, MAROON);

            // Draw meteors
            for (int i = 0; i < MAX_BIG_METEORS; i++)
            {
                if (bigMeteor[i].active) 
                {
                    DrawCircleV(bigMeteor[i].position, bigMeteor[i].radius, DARKGRAY);
                }

            }

            for (int i = 0; i < MAX_MEDIUM_METEORS; i++)
            {
                if (mediumMeteor[i].active) 
                {
                    DrawCircleV(mediumMeteor[i].position, mediumMeteor[i].radius, mediumMeteor[i].color);
                }

            }

            for (int i = 0; i < MAX_SMALL_METEORS; i++)
            {
                if (smallMeteor[i].active) 
                {
                    DrawCircleV(smallMeteor[i].position, smallMeteor[i].radius, smallMeteor[i].color);
                }

            }

            // Draw shoot
            for (int i = 0; i < PLAYER_MAX_SHOOTS; i++)
            {
                if (shoot[i].active) DrawCircleV(shoot[i].position, shoot[i].radius, BLACK);
            }

            if (victory) DrawText("VICTORY", screenWidth/2 - MeasureText("VICTORY", 20)/2, screenHeight/2, 20, LIGHTGRAY);

            if (pause) DrawText("GAME PAUSED", screenWidth/2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}