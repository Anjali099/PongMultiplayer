#include "SDL_net.h"
#include "SDL_ttf.h"
#include "MyGame.h"
#include "SDL_mixer.h"
#include <SDL_image.h>

using namespace std;

const char* IP_NAME = "localhost";
const Uint16 PORT = 55555;

bool is_running = true;

MyGame* game;

static int on_receive(void* socket_ptr) {
	TCPsocket socket = (TCPsocket)socket_ptr;

	const int message_length = 1024;
 
	char message[message_length];
	int received;

	// TODO: while(), rather than do
	do {
		received = SDLNet_TCP_Recv(socket, message, message_length);
		message[received] = '\0';

		char* pch = strtok(message, ",");

		// get the command, which is the first string in the message
		string cmd(pch);

		// then get the arguments to the command
		vector<string> args;

		while (pch != NULL) {
			pch = strtok(NULL, ",");

			if (pch != NULL) {
				args.push_back(string(pch));
			}
		}

		game->on_receive(cmd, args);

		if (cmd == "exit") {
			break;
		}

	} while (received > 0 && is_running);

	return 0;
}

static int on_send(void* socket_ptr) {
	TCPsocket socket = (TCPsocket)socket_ptr;

	while (is_running) {
		if (game->messages.size() > 0) {
			string message = "CLIENT_DATA";

			for (auto m : game->messages) {
				message += "," + m;
			}

			game->messages.clear();

			cout << "Sending_TCP: " << message << endl;

			SDLNet_TCP_Send(socket, message.c_str(), message.length());
		}

		SDL_Delay(1);
	}

	return 0;
}

void loop(SDL_Renderer* renderer) {
	SDL_Event event;

	while (is_running) {
		// input
		while (SDL_PollEvent(&event)) {
			if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
				game->input(event);
			

				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					is_running = false;
					break;

				default:
					break;
				}
			}

			if (event.type == SDL_QUIT) {
				is_running = false;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		game->update();

		game->render(renderer);

		SDL_RenderPresent(renderer);

		SDL_Delay(17);
	}
}

int run_game() {
	SDL_Window* window = SDL_CreateWindow(
		"Multiplayer Pong Client",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600,
		SDL_WINDOW_SHOWN
	);

	if (nullptr == window) {
		std::cout << "Failed to create window" << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (nullptr == renderer) {
		std::cout << "Failed to create renderer" << SDL_GetError() << std::endl;
		return -1;
	}

	loop(renderer);

	return 0;
}

int main(int argc, char** argv) {

	// Initialize SDL
	if (SDL_Init(0) == -1) {
		printf("SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}


	// Initialize SDL_net
	if (SDLNet_Init() == -1) {
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
		exit(2);
	}


	IPaddress ip;

	// Resolve host (ip name + port) into an IPaddress type
	if (SDLNet_ResolveHost(&ip, IP_NAME, PORT) == -1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(3);
	}

	// Open the connection to the server
	TCPsocket socket = SDLNet_TCP_Open(&ip);

	if (!socket) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(4);
	}

	//Initialise SDL_TTF (FONT)
	if (TTF_Init() == -1) {
		printf("SDL_Init: %s\n", SDL_GetError());
		exit(5);
	}

	//Get Images
	auto image = IMG_Load("assets/Background.png");
	if (image != nullptr) {
		std::cout << "Image is loaded" << std::endl;
	}
	else
	{
		std::cout << "Image is not loaded" << std::endl;
	}

	//Load Font
	TTF_Font* font = TTF_OpenFont("assets/fonts/arial.ttf", 14);

	if (font != nullptr) {
		std::cout << "Font is loaded" << std::endl;
	}
	else {
		std::cout << "Font is not found" << std::endl;
		exit(6);
	}
	// audio

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}
	else {
		std::cout << "Audio is opened" << std::endl;
	}

	// load sample.wav in to sample
	Mix_Chunk* sound = Mix_LoadWAV("assets/audio/space.wav");

	if (sound != nullptr) {
		std::cout << "Sound is loaded" << std::endl;
	}
	else {
		std::cout << "Sound is not loaded" << std::endl;
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
	}
	// set the sample's volume to 1/2
// Mix_Chunk *sample;
	int previous_volume;
	previous_volume = Mix_VolumeChunk(sound, MIX_MAX_VOLUME / 2);
	printf("previous_volume: %d\n", previous_volume);
	 //plays audio
	if (Mix_PlayChannel(-1, sound, 0) == -1) {
	 printf("Mix_PlayChannel: %s\n", Mix_GetError());
	}


	//Any assets being render go before this line
	game = new MyGame(font,image);
	//multi threaded
	SDL_CreateThread(on_receive, "ConnectionReceiveThread", (void*)socket);
	SDL_CreateThread(on_send, "ConnectionSendThread", (void*)socket);

	run_game();

	delete game;

	//Release font resrouce back to OS
	TTF_CloseFont(font);

	//Close Audio
	Mix_CloseAudio();

	// Close connection to the server
	SDLNet_TCP_Close(socket);

	// Shutdown SDL_net
	SDLNet_Quit();

	// Shutdown SDL
	SDL_Quit();

	return 0;
}