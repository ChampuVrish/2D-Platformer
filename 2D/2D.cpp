#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "gameObject.h"
#include <array>


using namespace std;

struct SDLState
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height, logW, logH;
};


const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 1;


struct GameState
{
	std::array<std::vector<GameObject>, 2> layers;
	int playerIdx;

	GameState() {
		playerIdx = 0; 
	}
};

//Game Assets
struct Resources
{
	const int ANIM_PLAYER_IDLE = 0;
	std::vector<Animation> playerAnims;

	std::vector<SDL_Texture *> textures;
	SDL_Texture* idletex;

	SDL_Texture* loadTextures(SDL_Renderer* renderer, const std::string& filepath)
	{
		//Load Textures

		SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());
		SDL_SetTextureScaleMode(tex, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
		textures.push_back(tex);
		return tex;

	}

	void load(SDLState &state)
	{
		playerAnims.resize(5);
		playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 0.7f);

		idletex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\idle.png");
	}

	void unload()
	{
		for (SDL_Texture* tex : textures)
		{
			SDL_DestroyTexture(tex);
		}
	}
};



bool initialize(SDLState& state);
void cleanup(SDLState& state);
void drawObject(const SDLState& state, GameState& gs,GameObject& obj, float deltaTime);



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

	Resources res;
	res.load(state);


	//Setup Game Data
	GameState gs;
	//Our Player
	GameObject player;
	player.type = ObjectType::player;
	player.texture = res.idletex;
	player.animations = res.playerAnims;
	player.currentAnimation = res.ANIM_PLAYER_IDLE;
	gs.layers[LAYER_IDX_CHARACTERS].push_back(player);



	const bool* keys = SDL_GetKeyboardState(nullptr);
	uint64_t prevTime = SDL_GetTicks();




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

		//update all Objects
		for (auto &layer : gs.layers)
		{
			for (GameObject &obj : layer)
			{
				if (obj.currentAnimation != -1)
				{
					obj.animations[obj.currentAnimation].step(deltaTime);
				}
			}
		}

		// Perform Drawing Operations

		SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
		SDL_RenderClear(state.renderer);

		SDL_SetRenderDrawColor(state.renderer, 50, 50, 50, 255);
		
		SDL_FRect gameArea{
		0.0f,
		0.0f,
		640.0f,
		320.0f
		};


		SDL_RenderFillRect(state.renderer, &gameArea);

		//Draw all Objects
		for (auto &layer : gs.layers)
		{
			for (GameObject &obj : layer)
			{
				drawObject(state, gs, obj, deltaTime);
			}
		}


		//Swap the Buffers and Present the Rendered Frame
		SDL_RenderPresent(state.renderer);
		prevTime = nowTime;

	}

	//Unloading

	res.unload();
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


void drawObject(const SDLState &state,GameState &gs,GameObject &obj,float deltaTime)
{
	const float frameWidth = 147;
	const float frameHeight = 150;

	const float spriteSize = 32;
	float srcX = obj.currentAnimation != -1
		? obj.animations[obj.currentAnimation].currentframe() * frameWidth : 0.0f;

	SDL_FRect src{
		.x = srcX,
		.y = 0,
		.w = frameWidth,
		.h = frameHeight
	};

	SDL_FRect dst{
		.x = obj.position.x,
		.y = obj.position.y,
		.w = spriteSize,
		.h = spriteSize
	};

	SDL_FlipMode flipmode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipmode);
}