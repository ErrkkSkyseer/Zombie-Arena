#include "Bullet.h"

Bullet::Bullet()
{
	m_BulletShape.setSize(sf::Vector2f(2, 2));
}

void Bullet::shoot(float startX, float startY, float targetX, float targetY)
{
	//Keep track of the bullet
	m_InFlight = true;
	m_Position.x = startX;
	m_Position.y = startY;

	//Calculate the gradient of the flight path
	float gradient = (startX - targetX) / (startY - targetY);

	//Any gradient less than 0 need to be negative
	if (gradient < 0)
	{
		gradient *= -1;
	}

	//Calculate the ratio between x and y
	float ratioXY = m_BulletSpeed / (1 + gradient);

	//Set the "speed" horizontally and vertically
	m_BulletDistanceY = ratioXY;
	m_BulletDistanceX = ratioXY * gradient;

	//Point the bullet in the right direction
	if (targetX < startX)
	{
		m_BulletDistanceX *= -1;
	}

	if (targetY < startY)
	{
		m_BulletDistanceY *= -1;
	}

	//Set a max range of 100 pixels
	float range = 1000.0f;
	m_MinX = startX - range;
	m_MaxX = startX + range;
	m_MinY = startY - range;
	m_MaxY = startY + range;

	//Position the bullet ready to be drawn
	m_BulletShape.setPosition(m_Position);
}

void Bullet::stop()
{
	m_InFlight = false;
}

bool Bullet::isInFlight()
{
	return m_InFlight;
}

FloatRect Bullet::getPosition()
{
	return m_BulletShape.getGlobalBounds();
;}

RectangleShape Bullet::getShape()
{
	return m_BulletShape;
}

void Bullet::update(float elaspedTime)
{
	//Update the bullet position variables
	m_Position.x += m_BulletDistanceX * elaspedTime;
	m_Position.y += m_BulletDistanceY * elaspedTime;

	//Move the bullet
	m_BulletShape.setPosition(m_Position);

	//Has the bullet gone out of range?
	if (m_Position.x < m_MinX || m_Position.x > m_MaxX ||
		m_Position.y < m_MinY || m_Position.y > m_MaxY)
	{
		m_InFlight = false;
	}
}