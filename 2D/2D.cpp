#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "gameObject.h"
#include <array>
#include <format>

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
	std::vector<GameObject> backgroundTiles;
	std::vector<GameObject> foregroundTiles;
	std::vector<GameObject> bullets;
	int playerIdx;
	SDL_FRect mapViewport;
	float bg2Scroll, bg3Scroll, bg4Scroll, bgfog;
	bool debugMode;

	GameState(const SDLState& state) {
		playerIdx = -1;
		mapViewport = { 0, 0, static_cast<float>(state.logW), static_cast<float>(state.logH) };
		bg2Scroll = bg3Scroll = bg4Scroll = bgfog = 0;
		debugMode = false;
	}

	GameObject& player() {
		return layers[LAYER_IDX_CHARACTERS][playerIdx];
	}
};

//Game Assets And Animation Data
struct Resources
{
	const int ANIM_PLAYER_IDLE = 0;
	const int ANIM_PLAYER_RUNNING = 1;
	const int ANIM_PLAYER_JUMPING = 2;
	const int ANIM_PLAYER_SLIDE = 3;
	const int ANIM_PLAYER_SHOOT = 4;
	const int ANIM_PLAYER_SLIDE_SHOOT = 5;
	const int ANIM_PLAYER_JUMP_SHOOT = 6;
	std::vector<Animation> playerAnims;

	const int ANIM_BULLET_MOVING = 0;
	const int ANIM_BULLET_HIT = 1;
	std::vector<Animation> bulletAnims;

	const int ANIM_ENEMY_IDLE = 0;
	const int ANIM_ENEMY_HIT = 1;
	const int ANIM_ENEMY_DEATH = 2;
	std::vector<Animation> enemyAnims;

	//TEXTURE MAPPING

	std::vector<SDL_Texture*> textures;
	SDL_Texture* idletex, * runtex, * jumptex, * slidetex, * grasstex, * bricktex, * metaltex, * groundtex, * BGtex,
		* Bg2tex, * Bg3tex, * Bg4tex, * fogtex, * bullettex, * bulletHittex,
		* shoottex, * runShoottex, * slideshoottex, * jumpShoottex,
		* enemytex, * enemyHittex, * enemydietex;

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
		playerAnims.resize(7);
		//IDLE ANIMATION
		playerAnims[ANIM_PLAYER_IDLE] = Animation(8, 3.3f);
		idletex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\idle.png");

		//RUNNING ANIMATION
		playerAnims[ANIM_PLAYER_RUNNING] = Animation(4, 0.5f);
		runtex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\RUN.png");

		//JUMPING ANIMATION
		playerAnims[ANIM_PLAYER_JUMPING] = Animation(1, 1.0f);
		jumptex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\jump.png");

		//SLIDING ANIMATION
		playerAnims[ANIM_PLAYER_SLIDE] = Animation(1, 1.0f);
		slidetex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\slide.png");

		//Level Textures
		grasstex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Tiles\\GRASS.png");
		bricktex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Tiles\\BRICK.png");
		metaltex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Tiles\\METAL.png");
		groundtex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Tiles\\GROUND.png");
		//Backrounds
		BGtex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Backgrounds\\BG1.png");
		Bg2tex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Backgrounds\\BG2.png");
		Bg3tex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Backgrounds\\BG3.png");
		Bg4tex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Backgrounds\\BG4.png");
		fogtex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Backgrounds\\fog.png");

		//Shooting

		playerAnims[ANIM_PLAYER_SHOOT] = Animation(4, 0.3f);
		shoottex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\shoot.png");
		runShoottex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\runShoot.png");
		playerAnims[ANIM_PLAYER_SLIDE_SHOOT] = Animation(1, 0.6f);
		slideshoottex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\slideShoot.png");
		playerAnims[ANIM_PLAYER_JUMP_SHOOT] = Animation(2, 0.3f);
		jumpShoottex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Player\\jumpShoot.png");

		//Bullets

		bulletAnims.resize(2);
		bulletAnims[ANIM_BULLET_MOVING] = Animation(1, 0.05f);
		bullettex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Bullets\\bullet.png");
		bulletAnims[ANIM_BULLET_HIT] = Animation(1, 0.5f);

		//Enemy
		enemyAnims.resize(3);
		enemyAnims[ANIM_ENEMY_IDLE] = Animation(5, 1.0f);
		enemytex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Enemy\\Enemyidle.png");
		enemyAnims[ANIM_ENEMY_HIT] = Animation(1, 0.7f);
		enemyHittex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Enemy\\EnemyHit.png");
		enemyAnims[ANIM_ENEMY_DEATH] = Animation(8, 2.0f);
		enemydietex = loadTextures(state.renderer, "E:\\AyushS\\2DGameDev\\2D\\Assets\\Enemy\\EnemyDeath.png");

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
	void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float width, float height, float deltaTime);
	void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime);
	void createTiles(const SDLState& state, GameState& gs, const Resources& res);
	void checkCollision(const SDLState& state, GameState& gs, Resources& res, GameObject& a, GameObject& b, float deltaTime);
	void handlekeyinput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown);
	void drawParallax(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime);


	int main(int argc, char* argv[])
	{

		SDLState state;
		state.width = 1600;
		state.height = 900;
		state.logW = 640;
		state.logH = 360;

		if (!initialize(state)) {
			return 1;
		}


		//Load Game Assets

		Resources res;
		res.load(state);


		//Setup Game Data
		GameState gs(state);
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
				case SDL_EVENT_KEY_DOWN:
				{
					handlekeyinput(state, gs, gs.player(), event.key.scancode, true);
					break;
				}
				case SDL_EVENT_KEY_UP:
				{
					handlekeyinput(state, gs, gs.player(), event.key.scancode, false);
					if (event.key.scancode == SDL_SCANCODE_BACKSLASH) {
						gs.debugMode = !gs.debugMode;
					}
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

			//Update Bullets
			for (GameObject& bullet : gs.bullets)
			{
				update(state, gs, res, bullet, deltaTime);
				//Update Animation
				if (bullet.currentAnimation != -1)
				{
					bullet.animations[bullet.currentAnimation].step(deltaTime);
				}
			}

			//Calculate Map Viewport Based on Player Position
			gs.mapViewport.x = (gs.player().position.x) - gs.mapViewport.w / 3;


			// Perform Drawing Operations
			SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
			SDL_RenderClear(state.renderer);


			//Draw Background
			SDL_FRect bgDst{
			.x = 0.0f,
			.y = 0.0f,
			.w = 640.0f,
			.h = 360.0f
			};
			SDL_RenderTexture(state.renderer, res.BGtex, nullptr, &bgDst);
			drawParallax(state.renderer, res.Bg4tex, gs.player().velocity.x, gs.bg4Scroll, 0.06f, deltaTime);

			drawParallax(state.renderer, res.Bg3tex, gs.player().velocity.x, gs.bg3Scroll, 0.20f, deltaTime);

			drawParallax(state.renderer, res.Bg2tex, gs.player().velocity.x, gs.bg2Scroll, 0.40f, deltaTime);
			drawParallax(state.renderer, res.fogtex, gs.player().velocity.x, gs.bgfog, 0.13f, deltaTime);

			//Draw Background Tiles
			for (GameObject& obj : gs.backgroundTiles)
			{
				SDL_FRect dst{
					.x = obj.position.x - gs.mapViewport.x,
					.y = obj.position.y,
					.w = static_cast<float>(obj.texture->w),
					.h = static_cast<float>(obj.texture->w)
				};
				SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);

			}


			//Draw all Objects
			for (std::vector<GameObject>& layer : gs.layers)
			{
				for (GameObject& obj : layer)
				{
					if (obj.type == ObjectType::enemy)
					{
						drawObject(state, gs, obj, 410.0f, 493.0f, deltaTime);
					}
					else
					{
						drawObject(state, gs, obj, 147.0f, 145.0f, deltaTime);
					}
				}
			}

			//Draw Bullets
			for (GameObject& bullet : gs.bullets)
			{
				if (bullet.data.bullet.state != bulletState::inactive)
				{
					drawObject(state, gs, bullet, 32.0f, 32.0f, deltaTime);
				}
			};
			

			//Draw foreground Tiles
			for (GameObject& obj : gs.foregroundTiles)
			{
				SDL_FRect dst{
					.x = obj.position.x - gs.mapViewport.x,
					.y = obj.position.y,
					.w = static_cast<float>(obj.texture->w),
					.h = static_cast<float>(obj.texture->w)
				};
				SDL_RenderTexture(state.renderer, obj.texture, nullptr, &dst);

			}


			//Show Debug Info
			if (gs.debugMode) {
				SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
				SDL_RenderDebugText(state.renderer, 5, 5,
					std::format("DEBUG MODE      State: {}  BulletCount: {}  OnGround: {}", static_cast<int>(gs.player().data.player.state), gs.bullets.size(), gs.player().grounded).c_str());
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

		//VSync
		SDL_SetRenderVSync(state.renderer, true);

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


	void drawObject(const SDLState& state, GameState& gs, GameObject& obj, float width, float height, float deltaTime)
	{

		const float spriteSize = 32.0f;


		float srcX = obj.currentAnimation != -1
			? obj.animations[obj.currentAnimation].currentframe() * width : 0.0f;

		SDL_FRect src{
			.x = srcX,
			.y = 0,
			.w = width,
			.h = height
		};

		SDL_FRect dst{
			.x = obj.position.x - gs.mapViewport.x,
			.y = obj.position.y,
			.w = spriteSize,
			.h = spriteSize
		};

		//Bullet Size
		if (obj.type == ObjectType::bullet) {
			SDL_FRect dst{
			dst.w = 2,
			dst.h = 2
			};
		}

		// Enemy size
		if (obj.type == ObjectType::enemy)
		{
			dst.w = 40.0f;
			dst.h = 40.0f;
			// Lift enemy up by the extra height.
			dst.y -= (40.0f - 32.0f);
		}


		SDL_FlipMode flipmode = obj.direction == -1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		SDL_RenderTextureRotated(state.renderer, obj.texture, &src, &dst, 0, nullptr, flipmode);

		if (gs.debugMode) {
			SDL_FRect rectA{
			.x = obj.position.x + obj.collider.x - gs.mapViewport.x,
			.y = obj.position.y + obj.collider.y,
			.w = obj.collider.w,
			.h = obj.collider.h
			};

			SDL_FRect sensor{
			.x = (obj.position.x + obj.collider.x + 4.0f) - gs.mapViewport.x,
			.y = obj.position.y + obj.collider.y + obj.collider.h,
			.w = obj.collider.w - 8.0f,
			.h = 2.0f
			};

			//Collision Display
			SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(state.renderer, 0, 255, 0, 100);
			SDL_RenderRect(state.renderer, &rectA);

			//Sensor Diplay
			SDL_SetRenderDrawColor(state.renderer, 0, 255, 0, 255);
			if (obj.grounded)
			{
				SDL_SetRenderDrawColor(state.renderer, 0, 0, 255, 255); // Blue
			}
			else
			{
				SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 255); // red
			}

			SDL_RenderRect(state.renderer, &sensor);

		}
	}

	void update(const SDLState& state, GameState& gs, Resources& res, GameObject& obj, float deltaTime)
	{
		if (obj.dynamic && !obj.grounded) {
			//Apply Gravity
			obj.velocity += glm::vec2(0, 500) * deltaTime;
		}

		float currdirection = 0;

		if (obj.type == ObjectType::player)
		{
			// Update player-specific logic......For example, handle input, movement, etc.

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

			Timer& weaponTimer = obj.data.player.weaponTimer;
			weaponTimer.step(deltaTime / 3); //Rate Of Shooting
			const auto handleShooting = [&state, &gs, &res, &obj, &weaponTimer](SDL_Texture* tex, SDL_Texture* shootTex, int animIndex, int shootAnimIndex)
				{
					if (state.keys[SDL_SCANCODE_J])
					{
						//Set Shooting Animation And Textures
						obj.texture = shootTex;
						obj.currentAnimation = shootAnimIndex;
						if (weaponTimer.isTimeout())
						{
							weaponTimer.reset();

							//Spawn Bullets
							GameObject bullet;
							bullet.data.bullet = bulletData();
							bullet.type = ObjectType::bullet;
							bullet.direction = gs.player().direction;
							bullet.texture = res.bullettex;
							bullet.currentAnimation = res.ANIM_BULLET_MOVING;
							const float bulletSize = 2.5f;
							bullet.collider = SDL_FRect{
								.x = 13,
								.y = 14,
								.w = bulletSize,
								.h = bulletSize
							};
							bullet.MaxSpeedX = 1000.0f;
							const int yVariation = 50; //Recoil
							const float yVelocity = SDL_rand(yVariation) - yVariation / 2.0f;
							bullet.velocity = glm::vec2(obj.velocity.x + 500.0f * obj.direction, yVelocity);
							bullet.animations = res.bulletAnims;

							//Adjust bullet Start
							const float left = -20;
							const float right = 40;
							const float t = (obj.direction + 1) / 2.0f;
							const float xOffset = left + right * t;
							bullet.position = glm::vec2(obj.position.x + xOffset,
								obj.position.y - 2.67);


							bool foundInactive = false;
							for (int i = 0;i < gs.bullets.size() && !foundInactive;i++)
							{
								if (gs.bullets[i].data.bullet.state == bulletState::inactive)
								{
									foundInactive = true;
									gs.bullets[i] = bullet;
								}
							}
							if (!foundInactive) {
								gs.bullets.push_back(bullet);
							}
						}
					}
					else
					{
						obj.texture = tex;
						obj.currentAnimation = animIndex;
					}
				};

			switch (obj.data.player.state)
			{
			case PlayerState::idle:
			{
				if (currdirection)
				{
					obj.data.player.state = PlayerState::running;
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
				handleShooting(res.idletex, res.shoottex, res.ANIM_PLAYER_IDLE, res.ANIM_PLAYER_SHOOT);
				break;
			}
			case PlayerState::running:
			{
				if (!currdirection)
				{
					obj.data.player.state = PlayerState::idle;

				}

				//Sliding Animation
				if (obj.velocity.x * currdirection < 0 && obj.grounded)
				{
					handleShooting(res.slidetex, res.slideshoottex, res.ANIM_PLAYER_SLIDE, res.ANIM_PLAYER_SLIDE_SHOOT);
				}

				else
					//Running Animation
				{
					handleShooting(res.runtex, res.runShoottex, res.ANIM_PLAYER_RUNNING, res.ANIM_PLAYER_RUNNING);
				}
				break;
			}

			case PlayerState::jumping:
			{
				handleShooting(res.jumptex, res.jumpShoottex, res.ANIM_PLAYER_JUMPING, res.ANIM_PLAYER_JUMP_SHOOT);
				break;
			}
			}

		}
		else if (obj.type == ObjectType::bullet)
		{
			switch (obj.data.bullet.state)
			{
			case bulletState::moving:
			{
				if (obj.position.x - gs.mapViewport.x < 0 || obj.position.x - gs.mapViewport.x > state.logW
					|| obj.position.y - gs.mapViewport.y < 0 || obj.position.y - gs.mapViewport.y > state.logH)
				{
					obj.data.bullet.state = bulletState::inactive;
				}
				break;
			}

			case bulletState::colliding:
			{
				if (obj.animations[obj.currentAnimation].isDone())
				{
					obj.data.bullet.state = bulletState::inactive;
				}
			}
			}
		}

		if (currdirection)
		{
			obj.direction = currdirection;
		}
		//Add Acceleration Based on Direction
		obj.velocity += currdirection * obj.acceleration * deltaTime;
		if (std::abs(obj.velocity.x) > obj.MaxSpeedX)
		{
			obj.velocity.x = obj.MaxSpeedX * currdirection;
		}

		//Add velocity to Position
		obj.position += obj.velocity * deltaTime;

		//Handle Collision Detection
		bool foundGround = false;
		for (auto& layer : gs.layers)
		{
			for (GameObject& objB : layer)
			{
				if (&obj != &objB) {
					checkCollision(state, gs, res, obj, objB, deltaTime);

					//Grounded Sensor
					if (objB.type == ObjectType::level)
					{
						SDL_FRect sensor{
						.x = obj.position.x + obj.collider.x + 4,
						.y = obj.position.y + obj.collider.y + obj.collider.h,
						.w = obj.collider.w - 8,
						.h = 1
						};
						SDL_FRect rectB{
							.x = objB.position.x + objB.collider.x,
							.y = objB.position.y + objB.collider.y,
							.w = objB.collider.w,
							.h = objB.collider.h
						};
						SDL_FRect rectC{ 0 };

						if (SDL_GetRectIntersectionFloat(&sensor, &rectB, &rectC))
						{
							foundGround = true;
						}
					}
				}
			}
		}

		if (obj.grounded != foundGround)
		{
			//Switching Grounded State
			obj.grounded = foundGround;
			if (foundGround && obj.type == ObjectType::player)
			{
				obj.data.player.state = PlayerState::running;
			}
		}
	}

	void CollisionResponse(const SDLState& state, GameState& gs, Resources& res, const SDL_FRect& rectA,
		const SDL_FRect& rectB, const SDL_FRect& rectC,
		GameObject& objA, GameObject& objB, float deltaTime)
	{
		const auto genericResponse = [&]()
			{
				if (rectC.w < rectC.h)
				{
					//Horizontal Collision
					if (objA.velocity.x > 0) {  // Right Collision
						objA.position.x -= rectC.w;
					}
					else if (objA.velocity.x < 0) {
						objA.position.x += rectC.w;	//Left Collision
					}
					objA.velocity.x = 0;
				}
				else
				{
					//Verticle Collision
					if (objA.velocity.y > 0) {  // Down Collision
						objA.position.y -= rectC.h;
					}
					else if (objA.velocity.y < 0) {
						objA.position.y += rectC.h;	//Up Collision
					}
					objA.velocity.y = 0;
				}
			};

		//Object We Are Checking
		if (objA.type == ObjectType::player)
		{
			//What we are Colliding With
			switch (objB.type)
			{
			case ObjectType::level:
			{
				genericResponse();
				break;
			}
			}
		}
		else if (objA.type == ObjectType::bullet)
		{
			switch (objA.data.bullet.state)
			{
			case bulletState::moving:
			{
				genericResponse();
				objA.velocity *= 0;
				objA.data.bullet.state = bulletState::moving;
				objA.texture = res.bulletHittex;
				objA.currentAnimation = res.ANIM_BULLET_HIT;
				break;
			}
			}
		}
	}

	void checkCollision(const SDLState& state, GameState& gs, Resources& res, GameObject& a, GameObject& b, float deltaTime)
	{
		SDL_FRect rectA{
			.x = a.position.x + a.collider.x,
			.y = a.position.y + a.collider.y,
			.w = a.collider.w,
			.h = a.collider.h
		};

		SDL_FRect rectB{
			.x = b.position.x + b.collider.x,
			.y = b.position.y + b.collider.y,
			.w = b.collider.w,
			.h = b.collider.h
		};

		SDL_FRect rectC{ 0 };

		if (SDL_GetRectIntersectionFloat(&rectA, &rectB, &rectC))
		{
			//Intersection found
			CollisionResponse(state, gs, res, rectA, rectB, rectC, a, b, deltaTime);
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
			0,0,0,4,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,6,0,0,0,0,2,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,2,1,1,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,1,2,1,2,1,1,2,1,1,2,1,1,2,2,1,1,2,2,1,1,2,1,2,2,1,1,2,1,2,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};

		short foreground[MAP_ROWS][MAP_COLS] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};

		short background[MAP_ROWS][MAP_COLS] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		};

		const auto loadMap = [&state, &gs, &res](short layer[MAP_ROWS][MAP_COLS])
			{
				const auto createObject = [&state](int r, int c, SDL_Texture* tex, ObjectType type)
					{
						GameObject o;
						o.type = type;
						o.position = glm::vec2(
							c * TILE_SIZE,
							state.logH - (MAP_ROWS - r) * TILE_SIZE
						);
						o.texture = tex;
						o.collider = { .x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE };
						return o;
					};

				for (int r = 0;r < MAP_ROWS;r++) {
					for (int c = 0;c < MAP_COLS;c++) {
						switch (layer[r][c])
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
							gs.backgroundTiles.push_back(brick);
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
							player.dynamic = true;
							player.collider = {
								.x = 9,
								.y = 2,
								.w = 14,
								.h = 30
							};
							gs.layers[LAYER_IDX_CHARACTERS].push_back(player);
							gs.playerIdx = gs.layers[LAYER_IDX_CHARACTERS].size() - 1;
							break;
						}

						case 5: //Grass
						{
							GameObject grass = createObject(r, c, res.grasstex, ObjectType::level);
							gs.foregroundTiles.push_back(grass);
							break;
						}
						case 6: //Enemy
						{
							GameObject enemy = createObject(r, c, res.enemytex, ObjectType::enemy);
							enemy.currentAnimation = res.ANIM_ENEMY_IDLE;
							enemy.animations = res.enemyAnims;
							enemy.collider = SDL_FRect{
								.x = 10,
								.y = -4,
								.w = 22,
								.h = 36
							};
							gs.layers[LAYER_IDX_CHARACTERS].push_back(enemy);
							break;
						}
						}
					}
				}
			};
		loadMap(map);
		loadMap(background);
		loadMap(foreground);
		assert(gs.playerIdx != -1);
	}

	void handlekeyinput(const SDLState& state, GameState& gs, GameObject& obj, SDL_Scancode key, bool keyDown)
	{
		const float JUMP_FORCE = -220.0f;

		if (obj.type == ObjectType::player)
		{
			switch (obj.data.player.state)
			{
			case PlayerState::idle:
			{
				if (key == SDL_SCANCODE_K && keyDown)
				{
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y += JUMP_FORCE;
				}
				break;
			}

			case PlayerState::running:
			{
				if (key == SDL_SCANCODE_K && keyDown)
				{
					obj.data.player.state = PlayerState::jumping;
					obj.velocity.y += JUMP_FORCE;
				}
				break;
			}
			}
		}
	}

	void drawParallax(SDL_Renderer* renderer, SDL_Texture* texture, float xVelocity, float& scrollPos, float scrollFactor, float deltaTime)
	{

		const float screenW = 640.0f;
		const float screenH = 360.0f;
		const float TexH = -10.0f;

		// Move layer opposite to player's movement
		scrollPos += xVelocity * scrollFactor * deltaTime;

		// Keep scroll between 0 and screenW
		while (scrollPos < 0.0f)
			scrollPos += screenW;

		while (scrollPos >= screenW)
			scrollPos -= screenW;

		// First copy
		SDL_FRect dst1{
			.x = -scrollPos,
			.y = TexH,
			.w = screenW,
			.h = screenH
		};

		// Second copy immediately after it
		SDL_FRect dst2{
			.x = -scrollPos + screenW,
			.y = TexH,
			.w = screenW,
			.h = screenH
		};

		SDL_RenderTexture(renderer, texture, nullptr, &dst1);
		SDL_RenderTexture(renderer, texture, nullptr, &dst2);
	}