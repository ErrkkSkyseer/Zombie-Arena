#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Zombie
{
private:
	//How fast is each zombie type?
	const float BLOATER_SPEED = 40;
	const float CHASER_SPEED = 80;
	const float CRAWLER_SPEED = 20;

	//How tough is each zombie type
	const float BOLTER_HEALTH = 5;
	const float CHASER_HEALTH = 1;
	const float CRAWLER_HEALTH = 3;

	//Zombie's hitbox size
	const float BOLTER_HITBOX_WIDTH = 75;
	const float BOLTER_HITBOX_HEIGHT = 75;
	const float CHASER_HITBOX_WIDTH = 55;
	const float CHASER_HITBOX_HEIGHT = 55;
	const float CRAWLER_HITBOX_WIDTH = 50;
	const float CRAWLER_HITBOX_HEIGHT = 50;

	//Make each zombie vary its speed slightly
	const int MAX_VARRIANCE = 30;
	const int OFFSET = 101 - MAX_VARRIANCE;

	//Position
	Vector2f m_Position;

	//Sprite
	Sprite m_Sprite;

	//Hitbox
	RectangleShape m_HitBox;

	//This zombie's status
	float m_Speed;
	float m_Health;

	//Is it still alive?
	bool m_Alive;

public:

	//Handle when a bullet hit a zombie
	bool hit();

	//find out that the zombie is alive
	bool isAlive();

	//Spawn a new zombie
	void spawn(float startX, float startY, int type, int seed);

	//Return a FloatRect
	FloatRect getPosition();

	//get Sprite
	Sprite getSprite();

	//get the HitBox
	RectangleShape getHitBox();

	//Update
	void update(float elapsedTime, Vector2f playerLocation);
};