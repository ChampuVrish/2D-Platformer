#pragma once
#include <glm/glm.hpp>
#include<vector>
#include "Animation.h"
#include <SDL3/SDL.h>

enum class PlayerState
{
	idle,
	running,
	jumping,
	sliding
};

struct PlayerData
{
	PlayerState state;
	Timer weaponTimer;
	PlayerData() : weaponTimer(0.1f)
	{
		state = PlayerState::idle;
	}
};


struct LevelData
{
	
};

struct EnemyData
{

};

enum class bulletState
{
	moving,colliding,inactive
};

struct bulletData
{
	bulletState state;
	bulletData() : state(bulletState::moving)
	{

	}
};

union ObjectData
{
	PlayerData player;
	LevelData level;
	EnemyData enemy;
	bulletData bullet;
};


enum class ObjectType
{
	player,enemy,level,bullet
};


struct GameObject
{
	ObjectType type;
	ObjectData data;
	glm::vec2 position, velocity, acceleration;
	float direction;
	float MaxSpeedX;
	std::vector<Animation> animations;
	int currentAnimation;
	SDL_Texture* texture;
	bool dynamic;
	bool grounded;
	SDL_FRect collider;

	GameObject() : data{ .level = LevelData() }, collider{ 0 }
	{
		type = ObjectType::level;
		direction = 1;
		MaxSpeedX = 0;
		position = velocity = acceleration = glm::vec2(0);
		currentAnimation = -1;
		texture = nullptr;
		dynamic = false;
		grounded = false;
	}
};