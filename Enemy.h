#ifndef Enemy_H
#define Enemy_H

#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "bullet.h"
#include "CaveSystem.h"
#include <vector>
using std::vector;
class Enemy
{

    public:

		SDL_Texture* loadImage(std::string fname, SDL_Renderer *gRenderer);

        //Initializes the variables, Constructor
        Enemy(int x, int y, int w, int h, int xvel, int yvel, SDL_Renderer *gRenderer);

        //Shows the enemy
        void renderEnemy(SDL_Renderer* gRenderer);

        //Moves the enemy
        void move(int playerX, int playerY, std::vector<int> bulletX, std::vector<int> bulletY, std::vector<int> bulletVelX, int kamiX, int kamiY, CaveSystem * cave);
        bool checkCollide(int x, int y, int pWidth, int pHeight, int xTwo, int yTwo, int pTwoWidth, int pTwoHeight);
        bool checkCollision(int x, int y, int w, int h);
        void hit(int d);

        //Position, dimensions, and velocity accessors
        int getX();
        int getY();
		int getWidth();
		int getHeight();
        int getHealth();
        void setyVelo(int y);
        void setxVelo(int x);
		void setPosX(int x);
		void setPosY(int y);

		// Methods that can be used to undo the enemy's moves when dealing with collisions
		void undoXMove();
		void undoYMove();
		void redoXMove();
		void redoYMove();

        SDL_Rect* getHitbox();
        Bullet* handleFiring();
		int chooseDirection();
		void calculateRiskscores(int playerX, int playerY, std::vector<int> bulletX, std::vector<int> bulletY, std::vector<int> bulletVelX, int kamiX, int kamiY, CaveSystem* cave);

		// Sprites for the enemy
		SDL_Texture* sprite1;
		SDL_Texture* sprite2;

    private:

		// Animation and shooting frequencies of the enemy
		static const int ANIMATION_FREQ = 100;
		static const int FIRING_FREQ = 1000;
    static const int FLICKER_FREQ = 50;
    static const int FLICKER_TIME = 500;
    static const int SPAWN_FREQ = 3000;

		// Limits on where the enemy can go
		static const int MIN_X = 0;
		static const int MAX_X = 500;
		static const int MIN_Y = 110;
		static const int MAX_Y = 610;

		// Variables for squares and their corresponding risk scores
		static const int SQUARE_WIDTH = 100;
		static const int NUM_HORIZONTAL_SQUARES = (MAX_X - MIN_X) / SQUARE_WIDTH;
		static const int NUM_VERTICAL_SQUARES = (MAX_Y - MIN_Y) / SQUARE_WIDTH;
		double riskScores[NUM_HORIZONTAL_SQUARES][NUM_VERTICAL_SQUARES];

        //Position and size of the enemy sprite on screen
        double xPos;
        double yPos;
		int current_x_square;
		int current_y_square;
        const int width;
        const int height;

        //Velocity of the enemy
		int maxXVelo;
		int maxYVelo;
        int xVelo;
        int yVelo;
        double tiltAngle;

        //defines the enemy asset
        SDL_Rect enemy_sprite;
        //defines the hitbox of the enemy
        SDL_Rect enemy_hitbox;

		// Move times, used for handling framerate-independent movement
		int time_since_move;
		int last_move;

		// Shooting times, used for handling framerate-independent firing rate
		int time_since_shoot;
		int last_shot;

    //health variables for the enemy
    int time_hit;
    int time_destroyed;
    int health;
    bool is_destroyed;
};

#endif
