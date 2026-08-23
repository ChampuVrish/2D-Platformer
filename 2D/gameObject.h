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
	sliding,
	prone,
	dead
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

enum class bulletState
{
	moving,colliding,inactive
};

enum class EnemyState
{
	shambling,
	damaged,
	dead
};

struct bulletData
{
	bulletState state;
	bulletData() : state(bulletState::moving)
	{

	}
};

struct EnemyData
{
	EnemyState state;
	Timer damagedTimer;
	int Enemyhealth;
	EnemyData() : state(EnemyState::shambling),damagedTimer(0.5f)
	{
		Enemyhealth = 100;
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
	Timer FlashTimer;
	bool ShouldFlash;
	int SpriteFrame;
	bool isAlive;
	int playerHP;


	GameObject() : data{ .level = LevelData() }, collider{ 0 }, FlashTimer(0.05f)
	{
		type = ObjectType::level;
		direction = 1;
		MaxSpeedX = 0;
		position = velocity = acceleration = glm::vec2(0);
		currentAnimation = -1;
		texture = nullptr;
		dynamic = false;
		grounded = false;
		ShouldFlash = false;
		SpriteFrame = 1;
		isAlive = true;
		playerHP = 1;
	}
};