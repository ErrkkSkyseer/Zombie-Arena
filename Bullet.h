#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Bullet
{
private:
	//Where is the bullet?
	Vector2f m_Position;

	//Where each bullet looks like
	RectangleShape m_BulletShape;

	//Is this bullet currently whizzing through the air
	bool m_InFlight = false;

	//How fast does bullet travel?
	float m_BulletSpeed = 1000.0f;

	//How fast doest a bullet travel horizontally and vertically each frame?
	//These vaules will be derived from m_BulletSpeed
	float m_BulletDistanceX;
	float m_BulletDistanceY;

	//Some boundaries so the bullet doesn't fly forever
	float m_MaxX;
	float m_MinX;
	float m_MaxY;
	float m_MinY;

public:
	Bullet();

	//Stop the bullet
	void stop();

	//Returns the calue of m_InFlight
	bool isInFlight();

	//Launch a new bullet
	void shoot(float startX, float startY, float targetX, float targetY);

	//Tell the calling code where the bullet is in the world
	FloatRect getPosition();

	//return the actual shape for drawing
	RectangleShape getShape();

	//Update the bullet each frame
	void update(float elapsedTime);
};