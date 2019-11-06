
#include "bullet.h"
#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "Enemy.h"


SDL_Texture* Enemy::loadImage(std::string fname, SDL_Renderer *gRenderer) {
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

    Enemy::Enemy(int x, int y, int w, int h, int xvel, int yvel, SDL_Renderer *gRenderer) :xPos{(double) x}, yPos{(double) y},width{w},height{h},maxXVelo{xvel},maxYVelo{yvel}{
	  	enemy_sprite = {(int) xPos, (int) yPos, width, height};
		enemy_hitbox = enemy_sprite;
		sprite1 = loadImage("sprites/EnemyPlane1.png", gRenderer);
		sprite2 = loadImage("sprites/EnemyPlane3.png", gRenderer);
		tiltAngle = 0;
	  	last_move = SDL_GetTicks();
		time_hit = SDL_GetTicks() - FLICKER_TIME;
		last_shot = SDL_GetTicks() - FIRING_FREQ;
		is_destroyed = false;
		health = 10;
    }

    void Enemy::renderEnemy(SDL_Renderer* gRenderer){
			if ((SDL_GetTicks() - time_hit) <= FLICKER_TIME && ((SDL_GetTicks() - time_hit) / FLICKER_FREQ) % 2 == 0) {
				return;
			}

			if (!is_destroyed){
				if ((SDL_GetTicks() / ANIMATION_FREQ) % 2 == 1) {
      		SDL_RenderCopyEx(gRenderer, sprite1, nullptr, &enemy_sprite, tiltAngle, nullptr, SDL_FLIP_NONE);
      	}else {
        	SDL_RenderCopyEx(gRenderer, sprite2, nullptr, &enemy_sprite, tiltAngle, nullptr, SDL_FLIP_NONE);
      	}
      	enemy_hitbox=enemy_sprite;
			}

			if ((SDL_GetTicks() - time_destroyed) >= SPAWN_FREQ && is_destroyed){
				health = 10;
				is_destroyed = false;
			}
    }

    void Enemy::move(int playerX, int playerY, std::vector<int> bulletX, std::vector<int> bulletY, std::vector<int> bulletVelX, int kamiX, int kamiY, int cave_x, int cave_y)
    {
        time_since_move = SDL_GetTicks() - last_move;
		xVelo = 0;
		yVelo = 0;

        // tiltAngle = 0;
		calculateRiskscores(playerX, playerY, bulletX, bulletY, bulletVelX, kamiX, kamiY, cave_x, cave_y);
		int direction = chooseDirection();

		// Move right if that's the optimal direction
        if(direction == 3 || direction == 4 || direction == 5 || (direction == 0 && ((xPos - width/2 - MIN_X % SQUARE_WIDTH) < SQUARE_WIDTH / 4))){
			xVelo = maxXVelo;
        }
		// Move left if that's the optimal direction
		if (direction == 1 || direction == 8 || direction == 7 || (direction == 0 && ((xPos - width/2 - MIN_X % SQUARE_WIDTH) > 3 * SQUARE_WIDTH / 4))) {
			xVelo = -maxXVelo;
		}
		// Move up if that's the optimal direction
		if (direction == 1 || direction == 2 || direction == 3 || (direction == 0 && ((yPos - height/2 - MIN_Y % SQUARE_WIDTH) > 3 * SQUARE_WIDTH / 4))) {
			yVelo = -maxYVelo;
			// tiltAngle = 15;
		}
		// Move down if that's the optimal direction
		if (direction == 5 || direction == 6 || direction == 7 || (direction == 0 && ((yPos - height/2 - MIN_Y % SQUARE_WIDTH) < SQUARE_WIDTH / 4))) {
			yVelo = maxYVelo;
			// tiltAngle = -15;
		}

		xPos += (double) (xVelo * time_since_move) / 1000;
		yPos += (double) (yVelo * time_since_move) / 1000;
		enemy_sprite = {(int)xPos,(int)yPos,width,height};
		last_move = SDL_GetTicks();
    }

	/*
	Directions are numbered as follows:
	1	2	3
	8	0	4
	7	6	5
	*/
	int Enemy::chooseDirection() {
		current_x_square = (xPos - width/2 - MIN_X) / SQUARE_WIDTH;
		current_y_square = (yPos - height/2 - MIN_Y) / SQUARE_WIDTH;
		double minRisk = riskScores[current_x_square][current_y_square];
		int leastRisky = 0;

		if (current_y_square > 0) {
			if (minRisk > riskScores[current_x_square][current_y_square-1]) {
				leastRisky = 2;
				minRisk = riskScores[current_x_square][current_y_square-1];
			}
			if (current_x_square > 0) {
				if (minRisk > riskScores[current_x_square-1][current_y_square-1]) {
					leastRisky = 1;
					minRisk = riskScores[current_x_square-1][current_y_square-1];
				}
			}
			if (current_x_square < NUM_HORIZONTAL_SQUARES - 1) {
				if (minRisk > riskScores[current_x_square+1][current_y_square-1]) {
					leastRisky = 3;
					minRisk = riskScores[current_x_square+1][current_y_square-1];
				}
			}
		}
		if (current_y_square < NUM_VERTICAL_SQUARES - 1) {
			if (minRisk > riskScores[current_x_square][current_y_square+1]) {
				leastRisky = 6;
				minRisk = riskScores[current_x_square][current_y_square+1];
			}
			if (current_x_square > 0) {
				if (minRisk > riskScores[current_x_square-1][current_y_square+1]) {
					leastRisky = 7;
					minRisk = riskScores[current_x_square-1][current_y_square+1];
				}
			}
			if (current_x_square < NUM_HORIZONTAL_SQUARES - 1) {
				if (minRisk > riskScores[current_x_square+1][current_y_square+1]) {
					leastRisky = 5;
					minRisk = riskScores[current_x_square+1][current_y_square+1];
				}
			}
		}
		if (current_x_square > 0) {
			if (minRisk > riskScores[current_x_square-1][current_y_square]) {
				leastRisky = 8;
				minRisk = riskScores[current_x_square-1][current_y_square];
			}
		}
		if (current_x_square > 0) {
			if (minRisk > riskScores[current_x_square+1][current_y_square]) {
				leastRisky = 4;
				minRisk = riskScores[current_x_square+1][current_y_square];
			}
		}
		//std::cout << current_x_square << " " << current_y_square << " " << leastRisky << " " << minRisk << std::endl;
		return leastRisky;
	}

	void Enemy::calculateRiskscores(int playerX, int playerY, std::vector<int> bulletX, std::vector<int> bulletY, std::vector<int> bulletVelX, int kamiX, int kamiY, int cave_x, int cave_y) 
	{
		for (int i = 0; i < NUM_HORIZONTAL_SQUARES; i++) {
			for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
				riskScores[i][j] = 0;
			}
		}
		int xBlock;
		int yBlock;
		// Factor bullets into the risk score
		for (int i = 0; i < bulletX.size(); i++) {
			// std::cout << "Made it" << std::endl;
			xBlock = (bulletX[i] - MIN_X) / SQUARE_WIDTH;
			yBlock = (bulletY[i] - MIN_Y) / SQUARE_WIDTH;
			if (bulletVelX[i] < 0) {
				if (yBlock >= 0 && yBlock < NUM_VERTICAL_SQUARES) {
					for (int j = 0; j < NUM_HORIZONTAL_SQUARES; j++) {
						riskScores[j][yBlock] += 200;
					}
				}
				if (xBlock >= 0 && xBlock < NUM_HORIZONTAL_SQUARES) {
					for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
						riskScores[xBlock][j] += 200;
					}
				}
			}
		}

		// Factor kamikazes into the risk score
		// std::cout << "kamis:" << kamiX.size() << std::endl;
		// for (int i = 0; i < kamiX.size(); i++) {
		// 	xBlock = (kamiX[i] - MIN_X) / SQUARE_WIDTH;
		// 	yBlock = (kamiY[i] - MIN_Y) / SQUARE_WIDTH;
		// 	if (yBlock >= 0 && yBlock < NUM_VERTICAL_SQUARES) {
		// 		for (int j = 0; j < NUM_HORIZONTAL_SQUARES; j++) {
		// 			riskScores[j][yBlock] += 10;
		// 		}
		// 	}
		// 	if (xBlock >= 0 && xBlock < NUM_HORIZONTAL_SQUARES) {
		// 		for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
		// 			riskScores[xBlock][j] += 10;
		// 		}
		// 	}
		// }

		//New kamikaze risk score calculations
		xBlock = (kamiX - MIN_X)/SQUARE_WIDTH;
		yBlock = (kamiY - MIN_Y)/SQUARE_WIDTH;
		if (yBlock >= 0 && yBlock < NUM_VERTICAL_SQUARES) {
			for (int j = 0; j < NUM_HORIZONTAL_SQUARES; j++) {
				riskScores[j][yBlock] += 10;
			}
		}
		if (xBlock >= 0 && xBlock < NUM_HORIZONTAL_SQUARES) {
			for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
				riskScores[xBlock][j] += 10;
			}
		}

		xBlock = (cave_x - MIN_X)/SQUARE_WIDTH;
		yBlock = (cave_y - MIN_Y)/SQUARE_WIDTH;
		if (yBlock >= 0 && yBlock < NUM_VERTICAL_SQUARES) {
			for (int j = 0; j < NUM_HORIZONTAL_SQUARES; j++) {
				riskScores[j][yBlock] += 100;
			}
		}
		if (xBlock >= 0 && xBlock < NUM_HORIZONTAL_SQUARES) {
			for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
				riskScores[xBlock][j] += 100;
			}
		}

		// Factor proximity to the edge of the screen into the risk score
		double center = ((MAX_Y - MIN_Y) / 2.0) / SQUARE_WIDTH - 0.5;
		// std::cout << "center:" << center << std::endl;
		for (int i = 0; i < NUM_HORIZONTAL_SQUARES; i++) {
			for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
				riskScores[i][j] += (j - center) * (j - center) * SQUARE_WIDTH / 10.0;
			}
		}

		// Factor player into the risk score
		xBlock = (playerX - MIN_X) / SQUARE_WIDTH;
		yBlock = (playerY - MIN_Y) / SQUARE_WIDTH;
		if (yBlock >= 0 && yBlock < NUM_VERTICAL_SQUARES) {
			for (int j = 0; j < NUM_HORIZONTAL_SQUARES; j++) {
				riskScores[j][yBlock] -= 30;
			}
		}
		if (xBlock >= 0 && xBlock < NUM_HORIZONTAL_SQUARES) {
			for (int j = 0; j < NUM_VERTICAL_SQUARES; j++) {
				riskScores[xBlock][j] -= 30;
			}
		}
	}

	bool Enemy::checkCollision(int objX, int objY, int objW, int objH) {
		if (is_destroyed) return false;
		return checkCollide(objX, objY, objW, objH, xPos, yPos, width, height);
	}

	bool Enemy::checkCollide(int x, int y, int pWidth, int pHeight, int xTwo, int yTwo, int pTwoWidth, int pTwoHeight)
	{
	    if (x + pWidth < xTwo || x > xTwo + pTwoWidth)
	        return false;
	    if (y + pHeight < yTwo || y > yTwo + pTwoHeight)
	        return false;
	    return true;
	}

	void Enemy::hit(int d){
		// If the player has just been hit, they should be invunerable, so don't damage them
		if ((SDL_GetTicks() - time_hit) <= FLICKER_TIME) {
			return;
		}

		time_hit = SDL_GetTicks();
		health -= d;
		if (health <= 0) {
			health = 0;
			time_destroyed = SDL_GetTicks();
			is_destroyed = true;
		}
	}

    int Enemy::getX(){
      return (int) xPos;
    }

    int Enemy::getY(){
      return (int) yPos;
    }

	int Enemy::getWidth() {
		return width;
	}

	int Enemy::getHeight() {
		return height;
	}

    void Enemy::setyVelo(int y){
      yVelo = y;
    }

    void Enemy::setxVelo(int x) {
      xVelo = x;
    }

    void Enemy::setPosX(int x) {
		xPos = x;
	}

	void Enemy::setPosY(int y) {
		yPos = y;
	}

	// Methods that can be used to undo the enemy's moves when dealing with collisions
	void Enemy::undoXMove() {
		xPos -= (double) (xVelo * time_since_move) / 1000;
	}
	void Enemy::undoYMove() {
		yPos -= (double) (yVelo * time_since_move) / 1000;
	}
	void Enemy::redoXMove() {
		xPos += (double) (xVelo * time_since_move) / 1000;
	}
	void Enemy::redoYMove() {
		yPos += (double) (yVelo * time_since_move) / 1000;
	}

    SDL_Rect* Enemy::getHitbox(){
      return &enemy_hitbox;
    }

    Bullet* Enemy::handleFiring()
    {
		if (!is_destroyed){
			time_since_shoot = SDL_GetTicks() - last_shot;
			if (time_since_shoot > FIRING_FREQ) {
				Bullet* b = new Bullet(xPos+width+5,yPos+height/2,450);
				last_shot = SDL_GetTicks();
				return b;
			}
		}
    	return nullptr;
    }

		int Enemy::getHealth(){
			return health;
		}
