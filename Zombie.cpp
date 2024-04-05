#include <cstdlib>
#include <ctime>

#include "Zombie.h"
#include "TextureHolder.h"

using namespace std;

void Zombie::spawn(float startX, float startY, int type, int seed)
{
	switch (type)
	{
	case 0:
		//Bloater
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/bloater.png"));

		m_Speed = BLOATER_SPEED;
		m_Health = BOLTER_HEALTH;

		m_HitBox.setSize(Vector2f(BOLTER_HITBOX_WIDTH, BOLTER_HITBOX_HEIGHT));

		break;

	case 1:
		//Chaser
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/chaser.png"));

		m_Speed = CHASER_SPEED;
		m_Health = CHASER_HEALTH;

		m_HitBox.setSize(Vector2f(CHASER_HITBOX_WIDTH, CHASER_HITBOX_HEIGHT));

		break;

	case 2:
		//Crawler
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/crawler.png"));

		m_Speed = CRAWLER_SPEED;
		m_Health = CRAWLER_HEALTH;

		m_HitBox.setSize(Vector2f(CRAWLER_HITBOX_WIDTH, CRAWLER_HITBOX_HEIGHT));

		break;

	default:
		break;
	}

	//Modify the speed to make the zombie unique
	//Every zombie is unique. Create a speed modifier
	srand((int)time(0) * seed);

	//random 71-100
	float modifier = (rand() % MAX_VARRIANCE) + OFFSET;

	//Express this as a fraction of 1
	modifier /= 100; // vaule ~(.7,1)
	m_Speed *= modifier;

	//Initialize its location
	m_Position.x = startX;
	m_Position.y = startY;

	//Set its origin to its center
	m_Sprite.setOrigin(
		m_Sprite.getLocalBounds().height / 2,
		m_Sprite.getLocalBounds().width / 2);

	//Set its position
	m_Sprite.setPosition(m_Position);

	//Setting up hitbox
	m_HitBox.setOrigin(m_Sprite.getOrigin());
	m_HitBox.setFillColor(Color::Transparent);
	m_HitBox.setOutlineThickness(-3);
	m_HitBox.setOutlineColor(Color::Magenta);
}

bool Zombie::hit()
{
	m_Health--;

	if (m_Health <= 0)
	{
		//dead
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture("graphics/blood.png"));

		return true;
	}

	return false;
}

bool Zombie::isAlive()
{
	return m_Alive;
}

FloatRect Zombie::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

Sprite Zombie::getSprite()
{
	return m_Sprite;
}

RectangleShape Zombie::getHitBox()
{
	return m_HitBox;
}

void Zombie::update(float elapsedTime, Vector2f playerLocation)
{
	float playerX = playerLocation.x;
	float playerY = playerLocation.y;

	//Update the zombie position variables
	if (playerX > m_Position.x)
	{
		m_Position.x += m_Speed * elapsedTime;
	}

	if (playerY > m_Position.y)
	{
		m_Position.y += m_Speed * elapsedTime;
	}

	if (playerX < m_Position.x)
	{
		m_Position.x -= m_Speed * elapsedTime;
	}

	if (playerY < m_Position.y)
	{
		m_Position.y -= m_Speed * elapsedTime;
	}

	//Move the sprite
	m_Sprite.setPosition(m_Position);

	//Move the hitbox
	m_HitBox.setPosition(m_Position);

	//Face the sprite in the correct direction
	float angle = (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180) / 3.141;
	m_Sprite.setRotation(angle);

}