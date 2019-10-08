#ifndef Enemy_H
#define Enemy_H

#include <SDL.h>
#include "bullet.h"

class Enemy
{
    
    public:
        //Initializes the variables, Constructor
        Enemy(int xPos, int yPos);

        //Shows the enemy 
        void renderEnemy(SDL_Renderer* gRenderer);

        //Moves the enemy
        void move();
        //For testing purposes
        void move(int x, int y);

        //Position and velocity accessors
        int getX();
        int getY();
        void setyVelo(int y);
        void setxVelo(int x);
        SDL_Rect* getHitbox();
        Bullet* shoot();
    
    private:
    
        //Position and size of the enemy sprite on screen
        double xPos;
        double yPos;
        const int width;
        const int height;

        //Velocity of the enemy
        int xVelo;
        int yVelo;

        //defines the enemy asset
        SDL_Rect enemy_sprite;
        //defines the hitbox of the enemy
        SDL_Rect enemy_hitbox;
		
		// Move times, used for handling framerate-independent movement
		int time_since_move;
		int last_move;
};

#endif