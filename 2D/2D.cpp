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
	const bool* keys;

	SDLState() : keys(SDL_GetKeyboardState(nullptr))
	{

	}
};


const size_t LAYER_IDX_LEVEL = 0;
const size_t LAYER_IDX_CHARACTERS = 1;
const int MAP_ROWS = 5;
const int MAP_COLS = 50;
const int TILE_SIZE = 32;


struct GameState
{
	std::array<std::vector<GameObject>, 2> layers;
	int playerIdx;

	GameState() {
		playerIdx = 0;
	}
};

//Game Assets And Animation Data
struct Resources
{
	const int ANIM_PLAYER_IDLE = 0;
	const int ANIM_PLAYER_RUNNING = 1;
	std::vector<Animation> playerAnims;

	//TEXTURE MAPPING

	std::vector<SDL_Texture*> textures;
	SDL_Texture* idletex, * runtex, * grasstex, * bricktex, * metaltex, * groundtex;

	SDL_Texture* loadTextures(SDL_Renderer* renderer, const std::string& filepath)
	{
		//Load Textures
		SDL_Texture* tex = IMG_LoadTexture(renderer, filepath.c_str());
		SDL_SetTextureScaleMode(tex, SDL_ScaleMode::SDL_SCALEMODE_NEAREST);
		textures.push_back(tex);
		return tex;

	}

	void load(SDLState& state)
	{
		playerAnims.resize(5);

		//IDLE ANIMATION
		playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 4.0f);
		idletex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\idle.png");

		//RUNNING ANIMATION
		playerAnims[ANIM_PLAYER_RUNNING] = Animation(4, 0.5f);

		//Level Textures
		runtex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\RUN.png");
		grasstex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\GRASS.png");
		bricktex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\BRICK.png");
		metaltex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\METAL.png");
		groundtex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\GROUND.png");


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
void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime);
void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
void createTiles(const SDLState& state, GameState& gs, const Resources& res);



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
	createTiles(state, gs, res);
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
		for (auto& layer : gs.layers)
		{
			for (GameObject& obj : layer)
			{
				update(state, gs, res, obj, deltaTime);
				//Update Animation
				if (obj.currentAnimation != -1)
				{
					obj.animations[obj.currentAnimation].step(deltaTime);
				}
			}
		}

		// Perform Drawing Operations

		SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
		SDL_RenderClear(state.renderer);

		SDL_SetRenderDrawColor(state.renderer, 10, 20, 40, 255);

		SDL_FRect gameArea{
		0.0f,
		0.0f,
		640.0f,
		320.0f
		};


		SDL_RenderFillRect(state.renderer, &gameArea);

		//Draw all Objects
		for (auto& layer : gs.layers)
		{
			for (GameObject& obj : layer)
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
bool initialize(SDLState& state)
{
	bool initsuccess = true;
	if (!SDL_Init(SDL_INIT_VIDEO))
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

void cleanup(SDLState& state)
{
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
}


void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float deltaTime)
{
	const float frameWidth = 147;
	const float frameHeight = 145;

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

void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime)
{
	if (obj.type == ObjectType::player)
	{
		// Update player-specific logic......For example, handle input, movement, etc.
		float currdirection = 0;

		//Left Movement
		if (state.keys[SDL_SCANCODE_A])
		{
			currdirection += -1;
		}

		//Right Movement
		if (state.keys[SDL_SCANCODE_D])
		{
			currdirection += 1;
		}

		if (currdirection != 0)
		{
			obj.direction = currdirection;
		}

		switch (obj.data.player.state)
		{
		case PlayerState::idle:
		{
			if (currdirection)
			{
				obj.data.player.state = PlayerState::running;
				obj.texture = res.runtex;
				obj.currentAnimation = res.ANIM_PLAYER_RUNNING;
			}
			else
			{
				//Deaccelerate Velocity when Idle
				if (obj.velocity.x)
				{
					const float factor = obj.velocity.x > 0 ? -1.5f : 1.5f;
					float amount = factor * obj.acceleration.x * deltaTime;
					if (std::abs(obj.velocity.x) < std::abs(amount))
					{
						obj.velocity.x = 0;
					}
					else
					{
						obj.velocity.x += amount;
					}
				}
			}
			break;
		}
		case PlayerState::running:
		{
			if (!currdirection)
			{
				obj.data.player.state = PlayerState::idle;
				obj.texture = res.idletex;
				obj.currentAnimation = res.ANIM_PLAYER_IDLE;
			}
			break;
		}
		}

		//Add Acceleration Based on Direction
		obj.velocity += currdirection * obj.acceleration * deltaTime;

		//Limiting The speed of the player to MaxSpeedX
		if (std::abs(obj.velocity.x) > obj.MaxSpeedX)
		{
			obj.velocity.x = obj.MaxSpeedX * currdirection;
		}

		//Add velocity to Position
		obj.position += obj.velocity * deltaTime;
	}
}

void createTiles(const SDLState& state, GameState& gs, const Resources& res)
{
	/*

		1 - Ground
		2 - Metal
		3 - Brick
		4 - Player
		5 - Grass
		6 - Enemy

	*/

	short map[MAP_ROWS][MAP_COLS] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,4,0,0,0,0,3,3,3,3,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,1,1,1,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};

	const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type)
		{
			GameObject o;
			o.type = type;
			o.position = glm::vec2(
				c * TILE_SIZE,
				state.logH - (MAP_ROWS - r) * TILE_SIZE
			);
			o.texture = tex;
			return o;
		};

	for (int r = 0;r < MAP_ROWS;r++) {
		for (int c = 0;c < MAP_COLS;c++) {
			switch (map[r][c])
			{
				case 1: //Ground
				{
					GameObject ground = createObject(r, c, res.groundtex, ObjectType::level);
					gs.layers[LAYER_IDX_LEVEL].push_back(ground);
					break;
				}

				case 2: //Metal
				{
					GameObject metal = createObject(r, c, res.metaltex, ObjectType::level);
					gs.layers[LAYER_IDX_LEVEL].push_back(metal);
					break;
				}

				case 3: //Brick
				{
					GameObject brick = createObject(r, c, res.bricktex, ObjectType::level);
					gs.layers[LAYER_IDX_LEVEL].push_back(brick);
					break;
				}

				case 4: //Player
				{
				//Our Player
				GameObject player = createObject(r, c, res.idletex, ObjectType::player);
				//Postion
				player.position = glm::vec2(
					c * TILE_SIZE,
					state.logH - (MAP_ROWS - r) * TILE_SIZE
				);
				player.data.player = PlayerData();
				player.acceleration = glm::vec2(120.0f, 0.0f);
				player.MaxSpeedX = 80.0f;
				player.animations = res.playerAnims;
				player.currentAnimation = res.ANIM_PLAYER_IDLE;
				gs.layers[LAYER_IDX_CHARACTERS].push_back(player);
				break;
				}

				case 5: //Grass
				{
					GameObject grass = createObject(r, c, res.grasstex, ObjectType::level);
					gs.layers[LAYER_IDX_LEVEL].push_back(grass);
					break;
				}
			}
		}
	}
}