#ifndef __MY_GAME_H__
#define __MY_GAME_H__

#include <iostream>
#include <vector>
#include <string>
#include "SDL_ttf.h"
#include "SDL.h"

static struct GameData {
    int player1Y = 0;
    int player2Y = 0;
    int ballX = 0;
    int ballY = 0;
} game_data;
 
static struct ScoreData {
	int player1Score = 0;
	int player2Score = 0;
} score_data;//only need one instance

class particle {
public:
	double x;
	double y;

	double vel_x;
	double vel_y;

	double accel_x;
	double accel_y;

	double life;

	int size;
	SDL_Color color;

	particle(double x, double y, double vel_x, double vel_y, double accel_x, double accel_y, int size, SDL_Color color);;
};
class MyGame {

private:
	bool isPlayer1; //booleans to see which player is what and therefore what paddle is to be controlled
	bool isPlayer2;
	bool PlayerSelected; //once player selected then it be true?
	SDL_Surface* image; //background?
	SDL_Rect player1 = { 200, 0, 20, 60 };
	SDL_Rect player2 = { 580, 0, 20, 60 };
	SDL_Rect ball = { 190, 200, 10, 10 };
	double t;

	std::vector<particle*>particles;
	//Allows acess to TTF_Font. SDL_rendertext
	TTF_Font* font;

    public:
	MyGame(TTF_Font* newFont, SDL_Surface* image); //text
        std::vector<std::string> messages;

        void on_receive(std::string message, std::vector<std::string>& args);
        void send(std::string message); 
		void input(SDL_Event& event); 
        void update();
        void render(SDL_Renderer* renderer);

		double get_random()
		{
			return rand() * 1.0 / RAND_MAX; //RANDOM FUNC x should b 0 < x < 1
		}

		bool PlayerSelect(SDL_Event& event)
		{
			if(!PlayerSelected) //if player NOT selected
			{
				
				switch (event.key.keysym.sym) {
				case SDLK_1:
	
					isPlayer2 = false;
					PlayerSelected = true;
					isPlayer1 = true;

					std::cout << "IS PLAYER 1 +" <<PlayerSelected<< std::endl;
					return isPlayer1;
					break;
					//=====================================
				case SDLK_2:

					isPlayer1 = false;
					PlayerSelected = true;
					isPlayer2 = true;

					std::cout << "IS PLAYER 2 +" << PlayerSelected << std::endl;
					return isPlayer2;
					break;
				}
			}
		}

};

#endif