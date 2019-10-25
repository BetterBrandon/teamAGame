#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "MapBlocks.h"
#include "Player.h"
#include "Enemy.h"
#include "bullet.h"
#include "GameOver.h"
#include "CaveSystem.h"
#include "Text.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int LEVEL_WIDTH = 100000;
constexpr int LEVEL_HEIGHT = 2000;
constexpr int SCROLL_SPEED = 420;

// Function declarations
bool init();
SDL_Texture* loadImage(std::string fname);
void close();

// Globals
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

// X and y positions of the camera
double camX = 0;
double camY = LEVEL_HEIGHT - SCREEN_HEIGHT;

Player * player;
MapBlocks *blocks;
GameOver *game_over;
CaveSystem *cave_system;
std::vector<Bullet*> bullets;

// Scrolling-related times so that scroll speed is independent of framerate
int time_since_horiz_scroll;
int last_horiz_scroll = SDL_GetTicks();

//framerate timer
Uint32 fps_last_time = SDL_GetTicks();
Uint32 fps_cur_time = 0;
int framecount;


bool init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
	}

	if(TTF_Init()==-1){
		std::cout<<"TTF could not initialize";
		return false;
	}

	gWindow = SDL_CreateWindow("TeamAGame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == nullptr) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	// Adding VSync to avoid absurd framerates
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == nullptr) {
		std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return  false;
	}

	// Set renderer draw/clear color
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

	return true;
}

SDL_Texture* loadImage(std::string fname) {
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

void close() {
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;
	TTF_Quit();

	// Quit SDL subsystems
	SDL_Quit();
}

// Function that prepares for enemy movement. Put in a separate method to avoid cluttering the main loop
void moveEnemy(Enemy * en) {
	int playerX = player->getPosX() + player->PLAYER_WIDTH/2;
	int playerY = player->getPosY() + player->PLAYER_HEIGHT/2;
	std::vector<int> bulletX;
	std::vector<int> bulletY;
	std::vector<int> bulletVelX;
	std::vector<int> kamiX;
	std::vector<int> kamiY;
	std::vector<FlyingBlock> kamikazes = blocks->getKamikazes();
	for (int i = 0; i < bullets.size(); i++) {
		bulletX.push_back(bullets[i]->getX());
		bulletY.push_back(bullets[i]->getY());
		bulletVelX.push_back(bullets[i]->getXVel());
	}
	for (int i = 0; i < kamikazes.size(); i++) {
		if (kamikazes[i].getRelX() > 0 && kamikazes[i].getRelX() < SCREEN_WIDTH && kamikazes[i].getRelY() > 0 && kamikazes[i].getRelY() < SCREEN_HEIGHT) {
			kamiX.push_back(kamikazes[i].getRelX() + kamikazes[i].BLOCK_WIDTH/2);
			kamiY.push_back(kamikazes[i].getRelY() + kamikazes[i].BLOCK_HEIGHT/2);
		}
	}
	en->move(playerX, playerY, bulletX, bulletY, bulletVelX, kamiX, kamiY);
}

int main() {
	if (!init()) {
		std::cout <<  "Failed to initialize!" << std::endl;
		close();
		return 1;
	}
	
	//Start the player on the left side of the screen
	player = new Player(SCREEN_WIDTH/4 - Player::PLAYER_WIDTH/2, SCREEN_HEIGHT/2 - Player::PLAYER_HEIGHT/2, gRenderer);
	blocks = new MapBlocks(LEVEL_WIDTH, LEVEL_HEIGHT, gRenderer);
	game_over = new GameOver();
	cave_system = new CaveSystem();

	//start enemy on left side behind player
	Enemy* en = new Enemy(100, SCREEN_HEIGHT/2, 125, 53, 200, 200, gRenderer);
	
	Bullet* newBullet;
	std::string fps;//for onscreen fps


	SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_Event e;
	bool gameon = true;

	while(gameon) {

		// Scroll to the side, unless the end of the level has been reached
		time_since_horiz_scroll = SDL_GetTicks() - last_horiz_scroll;
		camX += (double) (SCROLL_SPEED * time_since_horiz_scroll) / 1000;
		if (camX > LEVEL_WIDTH - SCREEN_WIDTH) {
			camX = LEVEL_WIDTH - SCREEN_WIDTH;
		}
		last_horiz_scroll = SDL_GetTicks();

		while(SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				gameon = false;
			}
			// Game can end by either pressing on '7' on the numpad or on top row of numbers
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && (e.key.keysym.sym == SDLK_7 || e.key.keysym.sym == SDLK_KP_7))
			{
				game_over->isGameOver = true;
			}
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_SPACE) {
				newBullet = player->handleForwardFiring();
				if (newBullet != nullptr) {
					bullets.push_back(newBullet);
				}
			}
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_b) {
				newBullet = player->handleBackwardFiring();
				if (newBullet != nullptr) {
					bullets.push_back(newBullet);
				}
			}
			else {
				player->handleEvent(e);
			}
			if(game_over->isGameOver)
			{
				game_over->handleEvent(e, player, blocks,gRenderer);
			}
		}


		// Move player
		player->move(SCREEN_WIDTH, SCREEN_HEIGHT, LEVEL_HEIGHT, camY);

		//move enemy
		moveEnemy(en);
		newBullet = en->handleFiring();
		if (newBullet != nullptr) {
			bullets.push_back(newBullet);
		}
		
		//move the bullets
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i]->move();
		}
		
		//Move Blocks and check collisions
		blocks->moveBlocks(camX, camY);
		blocks->checkCollision(player);
		blocks->checkCollision(en);
		for (int i = bullets.size() - 1; i >= 0; i--) {
			// If the bullet leaves the screen or hits something, it is destroyed
			bool destroyed;
			if (bullets[i]->getX() > SCREEN_WIDTH || bullets[i]->getX() < 0) {
				destroyed = true;
			}
			else {
				destroyed = blocks->checkCollision(bullets[i]);
			}
			if (destroyed) {
				bullets.erase(bullets.begin() + i);
			}
		}

		if((int) camX % 5500 == 0)
		{
			std::cout << "Creating Cave System" << std::endl;
			cave_system = new CaveSystem(camX, camY, SCREEN_WIDTH);
			cave_system->isEnabled = true;
		}
		
		if(cave_system->isEnabled)
		{
			cave_system->moveCaveBlocks(camX, camY);
			cave_system->checkCollision(player);
		}

		// Clear the screen
		SDL_RenderClear(gRenderer);

		
		// Draw the player
		player->render(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
		// Draw the enemy
		en->renderEnemy(gRenderer);
		blocks->render(SCREEN_WIDTH, SCREEN_HEIGHT, gRenderer);
		if (cave_system->isEnabled)
			cave_system->render(SCREEN_WIDTH, SCREEN_HEIGHT, gRenderer);

		//draw the bullets
		for (int i = 0; i < bullets.size(); i++) {
			bullets[i]->renderBullet(gRenderer);
		}

		framecount++;
		fps_cur_time=SDL_GetTicks();
		if (fps_cur_time - fps_last_time > 1000) {
			fps= std::to_string(framecount / ((fps_cur_time - fps_last_time) / 1000.0));
			fps +=" fps";
			// reset
			fps_last_time = fps_cur_time;
			framecount = 0;
		}
		Text text(gRenderer, "sprites/comic.ttf", 14, fps, {255,255,255,255});
		text.render(gRenderer,20,20);


		if(game_over->isGameOver)
		{
			game_over->stopGame(player, blocks);
			game_over->render(gRenderer);
		}
		


		SDL_RenderPresent(gRenderer);
	}

	// Out of game loop, clean up
	close();
}
