#ifndef CaveSystem_H
#define CaveSystem_H

#include <SDL.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include "Player.h"

class CaveBlock
{
public:
    // absolute coordinates of each CaveBlock
    int CAVE_BLOCK_ABS_X;
    int CAVE_BLOCK_ABS_Y;

    // coordinates of each CaveBlock relative to camera
    int CAVE_BLOCK_REL_X;
    int CAVE_BLOCK_REL_Y;

    int enabled; // Is the cave block going to be visible? 1 = yes, 0 = no

    static const int CAVE_BLOCK_HEIGHT = 20;
    static const int CAVE_BLOCK_WIDTH = 20;
	static const int CAVE_SYSTEM_PIXEL_HEIGHT = 720;
	static const int CAVE_SYSTEM_PIXEL_WIDTH = 4000;
    CaveBlock();
    std::string toString();
};


class CaveSystem
{
public:
    static const int CAVE_SYSTEM_HEIGHT = CaveBlock::CAVE_SYSTEM_PIXEL_HEIGHT / CaveBlock::CAVE_BLOCK_HEIGHT;
    static const int CAVE_SYSTEM_WIDTH = CaveBlock::CAVE_SYSTEM_PIXEL_WIDTH / CaveBlock::CAVE_BLOCK_WIDTH;
	static const int CAVE_SYSTEM_FREQ = 10000;
    
    static int CAVE_START_ABS_X;
    static int CAVE_END_ABS_X; 

    bool isEnabled = false;
    CaveBlock *cave_system[CAVE_SYSTEM_HEIGHT][CAVE_SYSTEM_WIDTH];

    CaveSystem();
    CaveSystem(int camX, int camY, int SCREEN_WIDTH);

    void render(SDL_Renderer *gRenderer);
    void moveCaveBlocks(int camX, int camY);
    void checkCollision(Player *p);
    void render(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *gRenderer);

    // get the abs value of the cave blocks
    int get_CB_rel_x();
    int get_CB_rel_y();
    

private:
    void generateRandomCave();
};

class PathSequence
{
public:
    int x[CaveSystem::CAVE_SYSTEM_WIDTH];
    int y[CaveSystem::CAVE_SYSTEM_WIDTH];
    int length;

    PathSequence();
};

#endif
