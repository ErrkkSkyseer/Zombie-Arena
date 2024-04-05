#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Player
{
private:
	const float START_SPEED = 200.0f;
	const float START_HEALTH = 100.0f;

	//hitBox const
	const float PLAYER_HITBOX_X = 50;
	const float PLAYER_HIYBOX_Y = 50;

	//position
	Vector2f m_Position;

	//hitbox
	RectangleShape m_HitBox;

	//sprite
	Sprite m_Sprite;

	//texture
	Texture m_Texture;

	//screen resolution
	Vector2f m_Resolution;

	//current arena size
	IntRect m_Arena;

	//tile size
	int m_TileSize;

	//player movment direction
	bool m_UpPressed;
	bool m_DownPressed;
	bool m_LeftPressed;
	bool m_RightPressed;

	//player stats
	int m_Health;
	int m_MaxHealth;

	//when was the player last hit
	Time m_LastHit;

	//speed
	float m_Speed;

public:

	Player();

	void spawn(IntRect arena, Vector2f resolution, int tileSize);

	//call at the end of every game
	void resetPlayerStats();

	//handle player getting hit by zombie
	bool hit(Time timeHit);

	//how long ago was the player last hit
	Time getLastHitTime();

	//Where is player
	FloatRect getPosition();

	//Where is the center of player
	Vector2f getCenter();

	//What angle is player facing;
	float getRotation();

	//send a copy of the sprite to the main function
	Sprite getSprite();

	//get hitbox
	RectangleShape getHitBox();

	//player movement functions
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	//player stop movement functions
	void stopLeft();
	void stopRight();
	void stopUp();
	void stopDown();

	//Update
	void update(float elapsedTime, Vector2i mousePosition);

	//upgrade
	void upgradeSpeed();
	void upgradehealth();

	//increase max health
	void increaseHealthLevel(int amount);

	//how much health has the player currently got?
	int getHealth();

};