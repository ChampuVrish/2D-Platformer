// 2D.cpp : Defines the entry point for the application.
//
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

using namespace std;

struct SDLState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height, logW, logH;
};


bool initialize(SDLState& state);
void cleanup(SDLState& state);


int main(int argc, char* argv[])
{

	SDLState state;
	state.width = 1600;
	state.height = 900;
	state.logW = 640;
	state.logH = 320;

	if (!initialize(state)) {
		return 1;
	}

	//Load Game Assets
	SDL_Texture* idletex = IMG_LoadTexture(state.renderer, "Data/Idle.png");
	SDL_SetTextureScaleMode(idletex, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);





	//Setup Game State
	const bool* keys = SDL_GetKeyboardState(nullptr);
	float playerX = 100;
	const float floor = state.logH;
	uint64_t prevTime = SDL_GetTicks();
	bool flipHorizontal = false;




	//Start The Game Loop
	bool running = true;

	while (running)
	{
		uint64_t nowTime = SDL_GetTicks();
		float deltaTime = (nowTime - prevTime) / 1000.0f; // Convert Form Milliseconds to Seconds

		SDL_Event event{ 0 };
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_QUIT:
				{
				running = false;
				break;
				}

				case SDL_EVENT_WINDOW_RESIZED:
				{
					state.width = event.window.data1;
					state.height = event.window.data2;
					break;
				}
			}

		}


		//Handle Movement Input

		float moveamount = 0;
		if (keys[SDL_SCANCODE_A]) 
		{
			flipHorizontal = true;
			moveamount += -75.0f;
		}

		if (keys[SDL_SCANCODE_D])
		{
			flipHorizontal = false;
			moveamount += 75.0f;
		}

		playerX += moveamount * deltaTime; // Apply deltaTime for consistent movement speed

		// Perform Drawing Operations

		SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
		SDL_RenderClear(state.renderer);

		SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
		
		SDL_FRect gameArea{
		0.0f,
		0.0f,
		640.0f,
		320.0f
		};


		SDL_RenderFillRect(state.renderer, &gameArea);


		const float spritesize = 32;

		SDL_FRect src{
			.x = 0,
			.y = 0,
			.w = 150,
			.h = 150
		};

		SDL_FRect dst{
			.x = playerX,
			.y = floor - spritesize,
			.w = spritesize,
			.h = spritesize
		};

		SDL_RenderTextureRotated(state.renderer, idletex, &src, &dst,0,nullptr,flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);



		//Swap the Buffers and Present the Rendered Frame
		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;

	}
	SDL_DestroyTexture(idletex);
	cleanup(state);
	return 0;
}







//Window Check INITIALIZE
bool initialize(SDLState &state)
{
	bool initsuccess = true;
	if (!SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to initialize SDL", nullptr);
		initsuccess = false;
	}

	// Create the Window

	state.window = SDL_CreateWindow("2D Game", state.width, state.height, SDL_WINDOW_RESIZABLE);

	if (!state.window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create window", nullptr);
		cleanup(state);
		initsuccess = false;
	}

	//Create the Renderer
	state.renderer = SDL_CreateRenderer(state.window, nullptr);
	if (!state.renderer)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to create renderer", nullptr);
		cleanup(state);
		initsuccess = false;
	}

	//Configure Presentation Mode
	state.logW = 640;
	state.logH = 320;
	SDL_SetRenderLogicalPresentation(state.renderer, state.logW, state.logH, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	return initsuccess;
}

void cleanup(SDLState &state)
{
SDL_DestroyRenderer(state.renderer);
SDL_DestroyWindow(state.window);
SDL_Quit();
}