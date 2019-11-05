#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Player.h"

SDL_Texture* Player::loadImage(std::string fname, SDL_Renderer *gRenderer) {
	SDL_Texture* newText = nullptr;

	SDL_Surface* startSurf = IMG_Load(fname.c_str());
	if (startSurf == nullptr) {
		std::cout << "Unable to load image " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
		return nullptr;
	}

	newText = SDL_CreateTextureFromSurface(gRenderer, startSurf);
	if (newText == nullptr) {
		std::cout << "Unable to create texture from " << fname << "! SDL Error: " << SDL_GetError() << std::endl;
	}

	SDL_FreeSurface(startSurf);

	return newText;
}

Player::Player(int xPos, int yPos, SDL_Renderer *gRenderer)
{
    x_pos = xPos;
    y_pos = yPos;
    x_vel = 0;
    y_vel = 0;
    x_accel = 0;
    y_accel = 0;
	sprite1 = loadImage("sprites/PlayerPlane1.png", gRenderer);
	sprite2 = loadImage("sprites/PlayerPlane3.png", gRenderer);
	gBackground = loadImage("sprites/cave.png", gRenderer);
    bg_X = 0;
    tiltAngle = 0;
	last_move = SDL_GetTicks();
    xp_decel = false;
    xn_decel = false;
    yp_decel = false;
    yn_decel = false;
	last_fshot = SDL_GetTicks() - SHOOT_FREQ;
	last_bshot = SDL_GetTicks() - SHOOT_FREQ;
	time_hit = SDL_GetTicks() - FLICKER_TIME;
	health = 100;
}

//Takes key presses and adjusts the player's velocity
void Player::handleEvent(SDL_Event &e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_w:
            yn_decel = true;
            break;

        case SDLK_a:
            xn_decel = true;
            break;

        case SDLK_s:
            yp_decel = true;
            break;

        case SDLK_d:
            x_vel += MAX_PLAYER_VEL;
            xp_decel = true;
            break;
        }
    }
    else if (e.type == SDL_KEYUP)
    {
        switch (e.key.keysym.sym)
        {
        case SDLK_w:
            yn_decel = false;
            break;

        case SDLK_a:
            xn_decel = false;
            break;

        case SDLK_s:
            yp_decel = false;
            break;

        case SDLK_d:
            xp_decel = false;
            break;
        }
    }
}

void Player::acceleration(bool &increasing, bool &decreasing, float &accel, float &accelerate_by, float &deccelerate_factor, int &vel){
    if(decreasing) accel -= accelerate_by;
    if(increasing) accel += accelerate_by;
    if(!decreasing && !increasing){
        if(vel < 0) accel += deccelerate_factor*accelerate_by;
        else if(vel > 0) accel -= deccelerate_factor*accelerate_by;
        float vel_increment = accel*time_since_move;
        vel += vel_increment * 2;
        if(vel != 0 && vel <= abs((int) (deccelerate_factor*vel_increment)) && vel >= -abs((int) (deccelerate_factor*vel_increment))){
            accel = 0;
            vel = 0;
        }
    } else{
        float vel_increment = accel*time_since_move;
        vel += vel_increment * 2;
    }
    if(accel > 3) accel = 3;
    else if(accel < -3) accel = -3;
}

//Moves the player
void Player::move(int SCREEN_WIDTH, int SCREEN_HEIGHT, int LEVEL_HEIGHT, int camY)
{
    float accelerate_by = 0.003*time_since_move;
    float deccelerate_factor = 4.0;
    acceleration(yp_decel, yn_decel, y_accel, accelerate_by, deccelerate_factor, y_vel);
    tiltAngle = 180 * sin(y_accel / 12);
    acceleration(xp_decel, xn_decel, x_accel, accelerate_by, deccelerate_factor, x_vel);

    if (y_vel > MAX_PLAYER_VEL)
        y_vel = MAX_PLAYER_VEL;
    else if (y_vel < -MAX_PLAYER_VEL)
        y_vel = -MAX_PLAYER_VEL;
    if (x_vel > MAX_PLAYER_VEL)
        x_vel = MAX_PLAYER_VEL;
    else if (x_vel < -MAX_PLAYER_VEL)
        x_vel = -MAX_PLAYER_VEL;

	time_since_move = SDL_GetTicks() - last_move;
    x_pos += (double) (x_vel * time_since_move) / 1000;
    y_pos += (double) (y_vel * time_since_move) / 1000;
    bg_X += (double) (time_since_move) / 10;

    // Move the player horizontally
    if (x_pos < 0)
    {
        x_pos = 0;
    }
    else if (x_pos > SCREEN_WIDTH - PLAYER_WIDTH)
    {
        x_pos = SCREEN_WIDTH - PLAYER_WIDTH;
    }

    // Move the player vertically.
    // If they are near the top of the screen, scroll up
    /*
    if (y_pos < SCREEN_HEIGHT / 10 && camY > 0)
    {
        y_pos = SCREEN_HEIGHT / 10;
        camY += (double) (y_vel * time_since_move) / 1000;
    }
    */
    // Stop the player if they hit the top of the level  
    else if (y_pos < 0)
    {
        y_pos = 0;
    }
    
    // If they are near the bottom of the screen, scroll down
    else if (y_pos > (9 * SCREEN_HEIGHT) / 10 - PLAYER_HEIGHT && camY < LEVEL_HEIGHT - SCREEN_HEIGHT)
    {
        y_pos = (9 * SCREEN_HEIGHT) / 10 - PLAYER_HEIGHT;
        camY += (double) (y_vel * time_since_move) / 1000;
    }
    // Stop the player if they hit the bottom
    else if (y_pos > SCREEN_HEIGHT - PLAYER_HEIGHT)
    {
        y_pos = SCREEN_HEIGHT - PLAYER_HEIGHT;
    }
    

    if (camY < 0)
    {
        camY = 0;
    }
    else if (camY > LEVEL_HEIGHT - SCREEN_HEIGHT)
    {
        camY = LEVEL_HEIGHT - SCREEN_HEIGHT;
    }
	last_move = SDL_GetTicks();
}

//Shows the player on the screen relative to the camera
void Player::render(SDL_Renderer *gRenderer, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	// Background loading (not sure why this is in the Player class )
	SDL_Rect bgRect = {-((int)bg_X % SCREEN_WIDTH), 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(gRenderer, gBackground, nullptr, &bgRect);
    bgRect.x += SCREEN_WIDTH;
    SDL_RenderCopy(gRenderer, gBackground, nullptr, &bgRect);
	
	// Don't render the player if they're flickering after being hit
	if ((SDL_GetTicks() - time_hit) <= FLICKER_TIME && ((SDL_GetTicks() - time_hit) / FLICKER_FREQ) % 2 == 0) {
		return;
	}
    
    SDL_Rect playerLocation = {(int) x_pos, (int) y_pos, PLAYER_WIDTH, PLAYER_HEIGHT};
	// Alternates through the two sprites every ANIMATION_FREQ ticks
    if ((SDL_GetTicks() / ANIMATION_FREQ) % 2 == 1) {
		SDL_RenderCopyEx(gRenderer, sprite1, nullptr, &playerLocation, tiltAngle, nullptr, SDL_FLIP_NONE);
	}
	else {
		SDL_RenderCopyEx(gRenderer, sprite2, nullptr, &playerLocation, tiltAngle, nullptr, SDL_FLIP_NONE);
	}
}

// Damages the player if they've been hit
void Player::hit(int damage) {
	// If the player has just been hit, they should be invunerable, so don't damage them
	// if ((SDL_GetTicks() - time_hit) <= FLICKER_TIME) {
	// 	return;
	// }
	
	// time_hit = SDL_GetTicks();
	// health -= damage;
	// if (health < 0) {
	// 	health = 0;
	// }
}

// Checks if the player collided with a kamikaze, returning true if so
bool Player::checkCollisionKami(int kamiX, int kamiY, int kamiW, int kamiH) {
	return checkCollide(kamiX, kamiY, kamiW, kamiH, x_pos, y_pos, PLAYER_WIDTH, PLAYER_HEIGHT);
}

// Checks if the player collided with a bullet, returning true if so
bool Player::checkCollisionBullet(int bullX, int bullY, int bullW, int bullH) {
	return checkCollide(bullX, bullY, bullW, bullH, x_pos, y_pos, PLAYER_WIDTH, PLAYER_HEIGHT);
}

bool Player::checkCollide(int x, int y, int pWidth, int pHeight, int xTwo, int yTwo, int pTwoWidth, int pTwoHeight)
{
    if (x + pWidth < xTwo || x > xTwo + pTwoWidth)
        return false;
    if (y + pHeight < yTwo || y > yTwo + pTwoHeight)
        return false;
    return true;
}

Bullet* Player::handleForwardFiring()
{
	time_since_fshot = SDL_GetTicks() - last_fshot;
	if (time_since_fshot > SHOOT_FREQ) {
		Bullet* b = new Bullet(x_pos+PLAYER_WIDTH+5,y_pos+PLAYER_HEIGHT/2,450);
		last_fshot = SDL_GetTicks();
		return b;
	}
	return nullptr;
}

Bullet* Player::handleBackwardFiring()
{
	time_since_bshot = SDL_GetTicks() - last_bshot;
	if (time_since_bshot > SHOOT_FREQ) {
		Bullet* b = new Bullet(x_pos - 10,y_pos+PLAYER_HEIGHT/2,-450);
		last_bshot = SDL_GetTicks();
		return b;
	}
	return nullptr;
}

//Position and velocity accessors
int Player::getPosX() { return x_pos; };
int Player::getPosY() { return y_pos; };
void Player::setVelX(int vel_x) { x_vel = vel_x; };
void Player::setVelY(int vel_y) { y_vel = vel_y; };
int Player::getVelX() { return x_vel; };
int Player::getVelY() { return y_vel; };
void Player::setPosX(int x) { x_pos = x; }
void Player::setPosY(int y) { y_pos = y; }
int Player::getHealth() { return health; };

// Methods that can be used to undo the user's moves when dealing with collisions
void Player::undoXMove() {x_pos -= (double) (x_vel * time_since_move) / 1000;}
void Player::undoYMove() {y_pos -= (double) (y_vel * time_since_move) / 1000;}
void Player::redoXMove() {x_pos += (double) (x_vel * time_since_move) / 1000;}
void Player::redoYMove() {y_pos += (double) (y_vel * time_since_move) / 1000;}
