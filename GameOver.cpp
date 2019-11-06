#include "GameOver.h"
#include <SDL.h>
#include "Player.h"
#include "MapBlocks.h"
#include <SDL_image.h>
#include "CaveSystem.h"

GameOver::GameOver(){};

void GameOver::stopGame(Player *player, MapBlocks *map_blocks)
{
    player->setVelX(0);
    player->setVelY(0);
    map_blocks->BLOCKS_N = 0;
}

void GameOver::handleEvent(SDL_Event &e, Player *player, MapBlocks *map_blocks, SDL_Renderer *gRenderer)
{

    //If mouse event happened
    if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
    {
        //Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        bool inside_button = true;

        if (x < RESTART_BUTTON_X){ inside_button = false; }
        else if (x > RESTART_BUTTON_X + RESTART_BUTTON_WIDTH){ inside_button = false; }
        else if (y < RESTART_BUTTON_Y){ inside_button = false; }
        else if (y > RESTART_BUTTON_Y + RESTART_BUTTON_HEIGHT){ inside_button = false; }

        if (inside_button && e.type == SDL_MOUSEBUTTONUP){ 
            restart(player, map_blocks, gRenderer); 
        }
    }
}

void GameOver::render(SDL_Renderer *gRenderer)
{
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xa1);
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_Rect fillRectOverlay = {0, 0, 1280, 720};
    SDL_RenderFillRect(gRenderer, &fillRectOverlay);

    // SDL_SetRenderDrawColor(gRenderer, 0x4F, 0xa7, 0x00, 0xFF);
    SDL_Rect fillRectButton = {RESTART_BUTTON_X, RESTART_BUTTON_Y, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT};
    // SDL_RenderFillRect(gRenderer, &fillRectButton);

    SDL_Texture *button_texture = nullptr;
    SDL_Surface *button_image_surface = IMG_Load("sprites/restart_button.png");
    button_texture = SDL_CreateTextureFromSurface(gRenderer, button_image_surface);
    SDL_FreeSurface(button_image_surface);
    SDL_RenderCopyEx(gRenderer, button_texture, nullptr, &fillRectButton, 0.0, nullptr, SDL_FLIP_NONE);
}

void GameOver::restart(Player *player, MapBlocks *map_blocks, SDL_Renderer* gRenderer)
{
    map_blocks->BLOCKS_N = map_blocks->BLOCKS_STARTING_N;
    map_blocks = new MapBlocks(LEVEL_WIDTH, LEVEL_HEIGHT,gRenderer, CaveSystem::CAVE_SYSTEM_FREQ, CaveBlock::CAVE_SYSTEM_PIXEL_WIDTH, 0,0);
    isGameOver = false;
    player->hit(-1*(100-player->getHealth())); // reset hp to 100
}
