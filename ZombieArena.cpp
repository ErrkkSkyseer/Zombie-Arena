#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

using namespace sf;

int main()
{
	//Here is the instance of TextureHolder
	TextureHolder holder;

	//The gane will always be in on of four states
	enum class State
	{
		PAUSED,
		LEVELING_UP,
		GAME_OVER,
		PLAYING
	};

	//Start with the GAME_OVR state;
	State state = State::GAME_OVER;

	//Get the screen resolution and create SFML window
	Vector2f resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

	//Create an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	//DA CLOCK
	Clock clock;

	//How long has the PLAYING state been active
	Time gameTimeTotal;

	//Where is the mouse in relation to world/screen coordinates
	Vector2f mouseWorldPosition;
	Vector2i mouseScreenPosition;

	//Create an instance of the Player class
	Player player;

	//The boundaries of the arena
	IntRect arena;

	//Create the background
	VertexArray background;
	//Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	//Prepare for a horde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = nullptr;

	//Prepare the bullets
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletSpare = 24;
	int bulletInClip = 6;
	int clipSize = 6;
	float fireRate = 1;
	Time lastPressed; //When was the fire button last pressed?

	//Hide the mouse pointer and replace it with crosshair
	window.setMouseCursorVisible(false);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	//Create a couple of pickups
	Pickup healthPickup(1);
	Pickup ammoPickup(2);

	//About the game
	int score = 0;
	int hiScore = 0;

	/*
	UI -----
	*/

	//For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	//Create a view for the HUD
	View hudView(FloatRect(0, 0, resolution.x, resolution.y));

	//Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	//Load the font
	Font font;
	font.loadFromFile("fonts/zombiecontrol.ttf");

	//Paused UI
	Text pausedText;
	pausedText.setFont(font);
	pausedText.setCharacterSize(155);
	pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);
	pausedText.setString("Press Enter \nto continue");

	//Game Over UI
	Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setCharacterSize(125);
	gameOverText.setFillColor(Color::White);
	gameOverText.setPosition(250, 850);
	gameOverText.setString("Press enter to play");

	//Leveling up
	Text levelUpText;
	levelUpText.setFont(font);
	levelUpText.setCharacterSize(80);
	levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);
	std::stringstream levelUpStream;
	levelUpStream << "1- Increased rate of fire"
		<< "\n2- Increased clip size(next reload)"
		<< "\n3- Increased max heath"
		<< "\n4- Increased run speed"
		<< "\n5- More and better health pickups"
		<< "\n6- More and better ammo pickups";
	levelUpText.setString(levelUpStream.str());

	//Ammo
	Text ammoText;
	ammoText.setFont(font);
	ammoText.setCharacterSize(55);
	ammoText.setFillColor(Color::White);
	ammoText.setPosition(200, 980);

	//Score
	Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(55);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 0);

	//Load the high score from a text file
	std::ifstream inputFile("gamedata/score.txt");
	if (inputFile.is_open())
	{
		// >> Reads the data
		inputFile >> hiScore; // hiScore reads the data from inputFile
		inputFile.close();
	}

	//HighScore
	Text hiScoreText;
	hiScoreText.setFont(font);
	hiScoreText.setCharacterSize(55);
	hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);
	std::stringstream s;
	s << "Hi Score:" << hiScore;
	hiScoreText.setString(s.str());

	//Zombies remaining
	Text zombiesRemainingText;
	zombiesRemainingText.setFont(font);
	zombiesRemainingText.setCharacterSize(55);
	zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(1500, 980);
	zombiesRemainingText.setString("Zombies: 100");

	//Wave number
	int wave = 0;
	Text waveNumberText;
	waveNumberText.setFont(font);
	waveNumberText.setCharacterSize(55);
	waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);
	waveNumberText.setString("Wave: 0");

	//health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);

	//When did we last update the HUD?
	int framesSinceHUDUpdate = 0;

	//How often (in frame should we update the HUD)
	int fpsMeasurementFrameInterval = 1000;

	//Prepare the sound
	SoundBuffer hitBuffer;
	SoundBuffer splatBuffer;
	SoundBuffer shootBuffer;
	SoundBuffer reloadBuffer;
	SoundBuffer failedBuffer;
	SoundBuffer powerUpBuffer;
	SoundBuffer pickUpBuffer;

	hitBuffer.loadFromFile("sound/hit.wav");
	splatBuffer.loadFromFile("sound/splat.wav");
	shootBuffer.loadFromFile("sound/shoot.wav");
	reloadBuffer.loadFromFile("sound/reload.wav");
	failedBuffer.loadFromFile("sound/reload_failed.wav");
	powerUpBuffer.loadFromFile("sound/powerup.wav");
	pickUpBuffer.loadFromFile("sound/pickup.wav");
	
	Sound hit;
	Sound splat;
	Sound shoot;
	Sound reload;
	Sound failed;
	Sound powerUp;
	Sound pickUp;
	
	hit.setBuffer(hitBuffer);
	splat.setBuffer(splatBuffer);
	shoot.setBuffer(shootBuffer);
	reload.setBuffer(reloadBuffer);
	failed.setBuffer(failedBuffer);
	powerUp.setBuffer(powerUpBuffer);
	pickUp.setBuffer(pickUpBuffer);

	//The main game loop
	while (window.isOpen())
	{

		/*
		Handle Input -----
		*/

		#pragma region Handle Input

		//Handle events by polling
		Event myEvent;
		while (window.pollEvent(myEvent))
		{
			if (myEvent.type == Event::KeyPressed)
			{
				//Pause a game while playing
				if (myEvent.key.code == Keyboard::Return &&
					state == State::PLAYING)
				{
					state = State::PAUSED;
				}
				//Resume while Paused
				else if (myEvent.key.code == Keyboard::Return && 
					state == State::PAUSED)
				{
					state = State::PLAYING;
					//Reset the clock so there isn't a frame jump
					clock.restart();
				}
				//Start a new game while in GAME_OVER state
				else if (myEvent.key.code == Keyboard::Return &&
					state == State::GAME_OVER)
				{
					state = State::LEVELING_UP;

					wave = 0;
					score = 0;

					//prepare the gun and ammo for next game
					currentBullet = 0;
					bulletSpare = 24;
					bulletInClip = 6;
					clipSize = 6;
					fireRate = 1;

					//Reset player's stats
					player.resetPlayerStats();
				}

				if (state == State::PLAYING)
				{
					//Reloading
					if (myEvent.key.code == Keyboard::R)
					{
						if (bulletSpare >= clipSize)
						{
							//Reload normally
							bulletInClip = clipSize;
							bulletSpare -= clipSize;

							reload.play();
						}
						else if (bulletSpare > 0)
						{
							//only few bullet left
							bulletInClip = bulletSpare;
							bulletSpare = 0;

							reload.play();
						}
						else
						{
							failed.play();
						}
					}
				}
			}
		}//End event polling
		
		//Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		//Handle WASD while playing
		if (state == State::PLAYING)
		{
			//Handle the pressing and releasing of the WASD keys
			if (Keyboard::isKeyPressed(Keyboard::W))
			{
				player.moveUp();
			}
			else
			{
				player.stopUp();
			}

			if (Keyboard::isKeyPressed(Keyboard::A))
			{
				player.moveLeft();
			}
			else
			{
				player.stopLeft();
			}

			if (Keyboard::isKeyPressed(Keyboard::S))
			{
				player.moveDown();
			}
			else
			{
				player.stopDown();
			}

			if (Keyboard::isKeyPressed(Keyboard::D))
			{
				player.moveRight();
			}
			else
			{
				player.stopRight();
			}

			//Fire a bullet
			if (Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate &&
					bulletInClip > 0)
				{
					//Pass the center of the player and the center of the cross-hair to the bullet function
					bullets[currentBullet].shoot(
						player.getCenter().x, player.getCenter().y,
						mouseWorldPosition.x, mouseWorldPosition.y);

					currentBullet++;
					if (currentBullet > 99)
					{
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;

					shoot.play();

					bulletInClip--;
				}
			}//End fire a bullet
		}//End WASD while playing

		//Handle the LEVELING_UP state
		if (state == State::LEVELING_UP)
		{
			//Handle the player leveling up
			if (myEvent.key.code == Keyboard::Num1)
			{
				//Increase fire rate
				fireRate++;
				state = State::PLAYING;
			}

			if (myEvent.key.code == Keyboard::Num2)
			{
				//Increase clip size
				clipSize += clipSize;
				state = State::PLAYING;
			}

			if (myEvent.key.code == Keyboard::Num3)
			{
				//Increase health
				player.upgradehealth();
				state = State::PLAYING;
			}

			if (myEvent.key.code == Keyboard::Num4)
			{
				//Increase speed
				player.upgradeSpeed();
				state = State::PLAYING;
			}

			if (myEvent.key.code == Keyboard::Num5)
			{
				//Upgrade pickup
				healthPickup.upgrade();
				state = State::PLAYING;
			}

			if (myEvent.key.code == Keyboard::Num6)
			{
				//Upgrade pickup
				ammoPickup.upgrade();
				state = State::PLAYING;
			}

			if (state == State::PLAYING)
			{
				//Increase the wave number
				wave++;

				//Prepare the level
				arena.width = 500 * wave;		
				arena.height = 500 * wave;		
				arena.left = 0;
				arena.top = 0;

				//Pass the vertex array by reference to the "createBackground()" function
				int tileSize = createBackground(background, arena);

				//Spawn the player in the middle of the arena
				player.spawn(arena, resolution, tileSize);

				//Configure the pickups
				healthPickup.setArena(arena);
				ammoPickup.setArena(arena);

				//Create a horde of zombies
				numZombies = 5 * wave;

				//Delete the previously allocated memory if exists
				delete[] zombies;
				zombies = createHorde(numZombies, arena);
				numZombiesAlive = numZombies;

				//Play the powerUp sound
				powerUp.play();

				//Reset the clock so there isn't a frame jump
				clock.restart();
			}
		}//End LEVELING_UP

		#pragma endregion

		/*
		Update -----
		*/

		#pragma region Update

		if (state == State::PLAYING)
		{
			//Update the delta time
			Time dt = clock.restart();

			//Update the total game time
			gameTimeTotal += dt;

			//Make a decimal fraction of 1 from delta time
			float dtAsSeconds = dt.asSeconds();

			//Where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			//Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			//Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			//Update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			//Make a note of the player new position
			Vector2f playerPosition(player.getCenter());

			//Make the view center around the player
			mainView.setCenter(player.getCenter());

			//Loop through each Zombie and update them
			for (int i = 0; i < numZombies; i++)
			{
				if (zombies[i].isAlive())
				{
					zombies[i].update(dt.asSeconds(), playerPosition);
				}
			}

			//Update any bullet that are in-flight
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					bullets[i].update(dtAsSeconds);
				}
			}

			//Update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			//Collision detection -----
			//have any zombies been shot?
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numZombies; j++)
				{
					if (bullets[i].isInFlight() &&
						zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getHitBox().getGlobalBounds()))
						{
							//Stop the bullet
							bullets[i].stop();

							//Register the hit and see if it was a kill
							if (zombies[j].hit())
							{
								//get a score when kill
								score += 10;

								if (score >= hiScore)
								{
									hiScore = score;
								}

								numZombiesAlive--;

								// When all the zombies are dead
								if (numZombiesAlive == 0)
								{
									state = State::LEVELING_UP;
								}

								splat.play();
							}
						}
					}
				}
			}//End Bullet-Zombie Collision detection

			//Have  any zombies touched the player
			for (int i = 0; i < numZombies; i++)
			{
				if (player.getHitBox().getGlobalBounds().intersects(zombies[i].getHitBox().getGlobalBounds()) &&
					zombies[i].isAlive())
				{
					if (player.hit(gameTimeTotal))
					{
						hit.play();
					}

					if (player.getHealth() <= 0)
					{
						state = State::GAME_OVER;

						std::ofstream outputFile("gamedata/score.txt");
						// << write the data
						outputFile << hiScore; // write "hiScore" data in output file
						outputFile.close();
					}
				}
			}//End Player-Zombie Collision Detection

			//Has the player touched the pickups
			if (player.getPosition().intersects(healthPickup.getPosition()) &&
				healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt());

				pickUp.play();
			}

			if (player.getPosition().intersects(ammoPickup.getPosition()) &&
				ammoPickup.isSpawned())
			{
				bulletSpare += ammoPickup.gotIt();

				reload.play();
			}
			//End Player-Pickups Collision Detection

			//Update HUD -----
			//Size up the health bar
			healthBar.setSize(Vector2f(player.getHealth() * 3, 50));

			//Increment the number of frame since the previous update
			framesSinceHUDUpdate++;

			//re-calculate every fpsMrasurementFrameInterval frame
			if (framesSinceHUDUpdate > fpsMeasurementFrameInterval)
			{
				//Update game HUD text
				std::stringstream ssAmmo;
				std::stringstream ssScore;
				std::stringstream ssHiScore;
				std::stringstream ssWave;
				std::stringstream ssZombiesAlive;

				//Update the ammo text
				ssAmmo << bulletInClip << "/" << bulletSpare;
				ammoText.setString(ssAmmo.str());

				//Update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				//Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				//Update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceHUDUpdate = 0;
			}//End HUD update

		}//End updating the scene

		#pragma endregion

		/*
		Draw -----
		*/

		#pragma region Draw

		if (state == State::PLAYING)
		{
			window.clear();

			//Set the mainView to be displayed in the window and draw everything related to it
			window.setView(mainView);

			//Draw the background
			window.draw(background, &textureBackground);

			//Draw the zombies
			for (int i = 0; i < numZombies; i++)
			{
				window.draw(zombies[i].getSprite());

				//Draw its hitbox
				window.draw(zombies[i].getHitBox());
			}

			//Dtaw the active bullet
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight())
				{
					window.draw(bullets[i].getShape());
				}
			}

			//Draw the player
			window.draw(player.getSprite());
			//Draw player's hitbox
			window.draw(player.getHitBox());

			//Draw the pickups if currently spawned
			if (ammoPickup.isSpawned())
			{
				window.draw(ammoPickup.getSprite());
			}

			if (healthPickup.isSpawned())
			{
				window.draw(healthPickup.getSprite());
			}

			//Draw the corsshair
			window.draw(spriteCrosshair);

			//Switch to HUD view
			window.setView(hudView);

			//Draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}

		window.display();

		#pragma endregion

	}//End game loop

	//Delete the previously allocated memory
	delete[] zombies;

	return 0;
}