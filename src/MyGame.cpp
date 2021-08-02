#include "MyGame.h"
#include <string> //=======
#include "SDL_ttf.h"

particle::particle(double x, double y, double vel_x, double vel_y, double accel_x, double accel_y, int size, SDL_Color color) {
	this->x = x;
	this->y = y;
	this->vel_x = vel_x;
	this->vel_y = vel_y;
	this->accel_x = accel_x;
	this->accel_y = accel_y;
	this->size = size;
	this->color = color;
	this->life = 1.0 * 10;
}
MyGame::MyGame(TTF_Font* newFont, SDL_Surface* image) {
	this->font = newFont;
	this->image = image;
	t = 0.0;

	int x = 0;
	int y = 0;
	isPlayer1 = false;
	isPlayer2 = false;
	PlayerSelected = false;
	srand(5000);
	for (int i = 0; i < 1000; i++) {
		double vel_x = get_random() - 0.5;
		double vel_y = -1 * get_random();

		double accel_x = (get_random() - 0.5) * 0.005;
		double accel_y = -0.21 * get_random();

		double x = get_random() * 10;
		particles.push_back(new particle(400, 600, vel_x, vel_y, accel_x, accel_y, 1, { 235, 30, 25, 255 }));


		x++;
		if (x == 800) {
			y++;
		}
	}
	std::cout << "number of particles:" << particles.size() << std::endl;
	std::cout << "Select a player with with the keys: 1 or 2" << std::endl;
}

void MyGame::on_receive(std::string cmd, std::vector<std::string>& args) {
	if (cmd == "GAME_DATA") {
		// we should have exactly 4 arguments
		if (args.size() == 4) {
			game_data.player1Y = stoi(args.at(0));
			game_data.player2Y = stoi(args.at(1));
			game_data.ballX = stoi(args.at(2));
			game_data.ballY = stoi(args.at(3));
		}
	}
	// Get player score from server cmds
   //Store each players score
	else if (cmd == "SCORES") {
		if (args.size() == 2) {
			score_data.player1Score = stoi(args.at(0));
			score_data.player2Score = stoi(args.at(1));

			std::cout << score_data.player1Score << std::endl;
		}
		else {
			std::cout << "Received: " << cmd << std::endl;
		}
	}
}
	void MyGame::send(std::string message) {
		messages.push_back(message);
	}



	void MyGame::input(SDL_Event& event) {
		
		PlayerSelect(event);
		if (PlayerSelected && isPlayer1 == true) {
			switch (event.key.keysym.sym) {
			case SDLK_w:
				send(event.type == SDL_KEYDOWN ? "W_DOWN" : "W_UP");
				break;
				//=====================================
			case SDLK_s:
				send(event.type == SDL_KEYDOWN ? "S_DOWN" : "S_UP");
				break;
			}
		}
		if (PlayerSelected && isPlayer2 == true) {
			switch (event.key.keysym.sym) {
			case SDLK_i:
				send(event.type == SDL_KEYDOWN ? "I_DOWN" : "I_UP");
				break;
				//=====================================
			case SDLK_k:
				send(event.type == SDL_KEYDOWN ? "K_DOWN" : "K_UP");
				break;
			}
		}
	}
	


	void MyGame::update() {
		player1.y = game_data.player1Y;
		player2.y = game_data.player2Y;
		ball.y = game_data.ballY;
		ball.x = game_data.ballX;
		t += 0.016; //time

		for (auto p : particles) {
			p->x += p->vel_x;
			p->y += p->vel_y;

			p->vel_x += p->accel_x; // adding accleration to my velocity
			p->vel_y += p->accel_y;

			p->life -= 0.016;

			if (p->life <= 0.0) {
				p->color.a = 0;
			}
			else {
				// [0..10] -> [0..1] -> [0..255]
				p->color.a = (Uint8)((p->life / 10.0) * 255); // changes alpha channel every 10 seconds
			}

		}
		
		/*	for (int i = 0; i < 100; i++) {
				double vel_x = get_random() - 0.5;
				double vel_y = -1 * get_random();

				double accel_x = (get_random() - 0.5) * 0.005;
				double accel_y = -0.21 * get_random();

				double x = get_random() * 10;
				particles.push_back(new particle(400, 600, vel_x, vel_y, accel_x, accel_y, 1, { 235, 30, 25, 255 }));

			}*/
	}
void MyGame::render(SDL_Renderer* renderer) {
	//Background

	SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_Rect bgRect = { 0, 0, 800, 600 };
	SDL_RenderCopy(renderer, backgroundTexture, NULL, &bgRect);
	SDL_DestroyTexture(backgroundTexture);


	SDL_SetRenderDrawColor(renderer, 106, 90, 205,255);
    SDL_RenderDrawRect(renderer, &player1);
	SDL_RenderFillRect(renderer, &player1);
	SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
	SDL_RenderDrawRect(renderer, &player2);
	SDL_RenderFillRect(renderer, &player2);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, &ball);
	SDL_RenderFillRect(renderer, &ball);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD); // need this so i can change the alpha channels
	//----Particle----
	for (auto p:particles) {
		SDL_Rect rect = { (int)p->x,(int)p->y,p->size * 2,p->size * 2 };
		SDL_SetRenderDrawColor(renderer, p->color.r, p->color.g, p->color.b, p->color.a); //setting the colour of the particle
		SDL_RenderFillRect(renderer, &rect);
	}

	//SDL_Color color = {255,192,203,255};
	//particle p(100,100,50,color);



	//Player 1 score text
	SDL_Color text_color = { 255, 165, 0, 255 };

	std::string scorePH = "player 1 score : ";

	std::string playerscoreString = std::to_string(score_data.player1Score);

	std::string combinedP1Score = scorePH + playerscoreString;

	SDL_Surface* text_surface = TTF_RenderText_Blended(font, combinedP1Score.c_str(), text_color);

	if (text_surface != nullptr) {
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		SDL_FreeSurface(text_surface);

		if (text_texture != nullptr) {
			int w, h;
			SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);

			SDL_Rect dst = { 100, 50, w, h };

			// NULL to draw the entire texture
			SDL_RenderCopy(renderer, text_texture, NULL, &dst);

			//TODO kill text
		}

		//Player 2 score text
		SDL_Color text_color2 = { 106, 90, 205,255 };

		std::string scorePH2 = "player 2 score : ";

		std::string player2scoreString = std::to_string(score_data.player2Score);

		std::string combinedP2Score = scorePH2 + player2scoreString;

		SDL_Surface* text_surface2 = TTF_RenderText_Blended(font, combinedP2Score.c_str(), text_color2);

		if (text_surface2 != nullptr) {
			SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface2);
			SDL_FreeSurface(text_surface2);

			if (text_texture != nullptr) {
				int w, h;
				SDL_QueryTexture(text_texture, NULL, NULL, &w, &h);

				SDL_Rect dst = { 600, 50, w, h };

				// NULL to draw the entire texture
				SDL_RenderCopy(renderer, text_texture, NULL, &dst);

				//TODO kill text
			}


		}
	}
}
