#include "ZombieArena.h"
#include "Zombie.h"

Zombie* createHorde(int numZombies, IntRect arena)
{
	Zombie* zombies = new Zombie[numZombies];

	int maxY = arena.height - 20;
	int minY = arena.top + 20;
	int maxX = arena.width - 20;
	int minX = arena.left + 20;

	//random spawn pos and zombie type
	for (int i = 0; i < numZombies; i++)
	{
		//Which side should the zombie spawn
		srand((int)time(0) * i);
		int side = (rand() % 4);
		float x, y;

		switch (side)
		{
		case 0:
			//left
			x = minX;
			y = (rand() % maxY) + minY;
			break;

		case 1:
			//right
			x = maxX;
			y = (rand() % maxY) + minY;
			break;

		case 2:
			//top
			x = (rand() % maxX) + minX;
			y = minY;
			break;

		case 3:
			//bottom
			x = (rand() % maxX) + minX;
			y = maxY;
		default:
			break;
		}
		
		//Random zombie type
		srand((int)time(0) * i * 2);
		int type = rand() % 3;

		//Spawn the new zombie into the array
		zombies[i].spawn(x, y, type, i);
	}

	return zombies;
}