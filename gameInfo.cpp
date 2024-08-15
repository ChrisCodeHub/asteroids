#include "gameInfo.hpp"


GameInfo::GameInfo(void)
{
   // start to create the low level blocks we will need 
   // and a ship to shoot from
   ship = std::make_unique<Ship>(values.screenWidth, values.screenHeight, values.PlayerBaseSize);
}

// Update game (one frame)
void GameInfo::UpdateGame(void)
{
    if (!values.gameOver)
    {
        if (IsKeyPressed('P')) 
        {
            values.pause = !values.pause;
        }

        if (!values.pause)
        {
            ship->updateShipPosition(values.screenWidth, values.screenHeight, values.PlayerSpeed);

            // Player shoot logic
            // create new Shots upto values.PlayerMaxShoots as player presesd SPACE key 
            if (IsKeyPressed(KEY_SPACE))
            {
                
                if (shoots.size() < values.PlayerMaxShoots)
                {
                    Vector2 playerPosition = ship->getShipPosition();
                    Shots newShot(ship->rotation, values.PlayerSpeed, playerPosition);
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
            ship->updateCollider();

            
            for (auto &rock : asteroids)
            {
                if (CheckCollisionCircles((Vector2){ship->collider.x, ship->collider.y}, ship->collider.z, rock->position, rock->radius)) values.gameOver = true;
            }


            //meteor logic
            for (auto &rock : asteroids)
            {                
                rock->updateposition(values.screenWidth, values.screenHeight);
            }

            for (auto &shot: shoots)
            {
                if ((shot.active))
                {
                    for (auto &rock : asteroids)
                    {   
                        if (CheckCollisionCircles(shot.position, shot.radius, rock->position, rock->radius))
                        {
                            shot.active = false;
                            rock->active = false;                            
                            if ((rock->rockType == big) || (rock->rockType == medium))
                            {
                                Vector2 position = (Vector2){rock->position.x, rock->position.y};
                                auto speed1 = gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, true);
                                auto speed2 = gameUtils::calcNewSpeed(shot.rotation, values.MeteorsSpeed, false);                                  
                                
                                if (rock->rockType == big) 
                                {                                        
                                    asteroids.emplace_back(std::make_unique<Asteroid>(position, speed1, 20, medium));                                        
                                    asteroids.emplace_back(std::make_unique<Asteroid>(position, speed2, 20, medium));                                        
                                }
                                else if (rock->rockType == medium)
                                {
                                    asteroids.emplace_back(std::make_unique<Asteroid>(position, speed1, 10, small));
                                    asteroids.emplace_back(std::make_unique<Asteroid>(position, speed2, 10, small));
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }

        asteroids.erase( std::remove_if( asteroids.begin(), 
                            asteroids.end(),
                            [](std::unique_ptr<Asteroid> const & r) { if (r == nullptr) return true; else return (r->active == false); }
                           ), asteroids.end());
        
        values.victory = (0 == (asteroids.size()));
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            
        }
    }
}


// Initialize game variables
void GameInfo::InitGame()
{
    values.victory = false;
    values.pause = false;    
    values.gameOver = false;

    // delete amy pre-existing meteors, 
    // since these are unique ptrs they should be cleaned up
    // automatically so this clear should not result
    // in a memory leak
    // asteroids.clear();
    
    // then create the first set of large asteroids
    for (uint32_t i = 0; i < values.MaxBigMeteors; i++)
    {        
        asteroids.emplace_back(std::make_unique<Asteroid>(i, values.screenWidth, values.screenHeight, values.MeteorsSpeed, big));
    }
}