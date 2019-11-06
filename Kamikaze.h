#ifndef Kamikaze_H
#define Kamikaze_H

#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Player.h"

class Kamikaze
{
  public:
    SDL_Texture* loadImage(std::string fname, SDL_Renderer *gRenderer);

    //Constructor initializing the variables
    Kamikaze(int x, int y, int w, int h, int delay, SDL_Renderer *gRenderer);

    //Shows Kamikaze on screen
    void renderKam(int SCREEN_WIDTH, SDL_Renderer* gRenderer);

    //Move the plane into the frame, follows the player along
    //the y axis then assults the player after a certain period
    //of time
    void move(Player* p, int SCREEN_WIDTH);

	bool checkCollisionBullet(int bullX, int bullY, int bullW, int bullH);
	bool checkCollide(int x, int y, int pWidth, int pHeight, int xTwo, int yTwo, int pTwoWidth, int pTwoHeight);

    //Position, dimensions, and velocity accessors
    int getX();
    int getY();
    void setX(int x);
    void setY(int y);
    int getWidth();
    int getHeight();
    void setArrivalTime(int a);

    SDL_Rect* getHitbox();
    SDL_Texture* sprite1;
    SDL_Texture* sprite2;


  private:
    static const int ANIMATION_FREQ = 100;
    static const int ASSAULT_FREQ = 4000;
    static const int MAX_MOVE_VELO = 175;
    static const int MAX_ASSAULT_VELO = 800;
    double xPos;
    double yPos;
    int xVelo;
    int yVelo;
    const int width;
    const int height;

    double tiltAngle;

    // Move times, used for handling framerate-independent movement
	  int time_since_move;
	  int last_move;

	// Time when the kamikaze arrives on the screen
	  int arrival_time;

    SDL_Rect kam_sprite;
    SDL_Rect kam_hitbox;


};

#endif
