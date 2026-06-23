// ZombieArena - main game loop, plus createBackground() and createHorde()
#include <ctime>      // time() used by createBackground / createHorde
#include <cstdlib>    // rand(), srand()
// Zombie

#include <SFML/Graphics.hpp>
#include <sstream>
#include <iostream>
#include "player.h"
#include "zombie.h"
#include "bullet.h"
using namespace sf;
using namespace std;

int createBackground(VertexArray &rVA, IntRect arena);
Zombie *createHorde(int numZombies, IntRect arena);

int main(){
	// *** Setup (Declaration & Initialisation) ***
	VideoMode vm(1920, 1080);
	RenderWindow window(vm,"Zombie Shooter", Style::Fullscreen);

	View mainView(FloatRect(0, 0, 1920, 1080));
	View hudView(FloatRect(0, 0, 1920, 1080));

	enum class State{GAME_OVER, LEVELING_UP, PLAYING, PAUSED};

	State state = State::GAME_OVER;
    IntRect arena;
    Vector2f resolution;
    resolution.x = 1920;
    resolution.y = 1080;

	RectangleShape bar;
	bar.setSize(Vector2f(500, 500));
	bar.setPosition(0, 0);
	bar.setFillColor(Color::Red);

	Vector2f mouseWorldPosition; // relation to world coordinates
	Vector2i mouseScreenPosition; // relation to screen coordinates

	Texture textureBackground;
	textureBackground.loadFromFile("background_sheet.png");

	Texture textureGameOver;
	textureGameOver.loadFromFile("background.png");
	Sprite spriteGameOver;
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	VertexArray background;

	window.setMouseCursorVisible(false);

	Texture textureCrosshair;
	textureCrosshair.loadFromFile("crosshair.png");

	Sprite spriteCrosshair;
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	Font font;
    font.loadFromFile("zombiecontrol.ttf");

    //Paused Text
    Text pausedText;
    pausedText.setFont(font);
    pausedText.setString("Press Enter \nto Continue");
    pausedText.setCharacterSize(155);
    pausedText.setFillColor(Color::White);
	pausedText.setPosition(400, 400);

    //Game Over Text
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Press Enter to Play");
    gameOverText.setCharacterSize(125);
    gameOverText.setFillColor(Color::White);

	FloatRect textRect = gameOverText.getLocalBounds();
	gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	gameOverText.setPosition(1960 / 2.0f, 1080 / 2.0f);

	// Levelling Up
    Text levelUpText;
    levelUpText.setFont(font);
    levelUpText.setCharacterSize(80);
    levelUpText.setFillColor(Color::White);
	levelUpText.setPosition(150, 250);

	stringstream levelUpStream;
	levelUpStream <<
		"1- Increase rate of fire" <<
		"\n2- Increase clip size (next reload)" <<
		"\n3- Increase max health";

	levelUpText.setString(levelUpStream.str());

	// Score
	Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(55);
    scoreText.setFillColor(Color::White);
	scoreText.setPosition(20, 20);

	int score = 0;
	stringstream s;
	s << "Score: " << score;
	scoreText.setString(s.str());

	// High Score
	Text hiScoreText;
    hiScoreText.setFont(font);
    hiScoreText.setCharacterSize(55);
    hiScoreText.setFillColor(Color::White);
	hiScoreText.setPosition(1400, 0);

	int hiScore = 0;
	stringstream hs;
	hs << "High Score: " << hiScore;
	hiScoreText.setString(hs.str());

	// Zombies Remaining
    Text zombiesRemainingText;
    zombiesRemainingText.setFont(font);
    zombiesRemainingText.setString("Zombies: 100");
    zombiesRemainingText.setCharacterSize(55);
    zombiesRemainingText.setFillColor(Color::White);
	zombiesRemainingText.setPosition(1500, 980);

	// Zombies Remaining
    Text waveNumberText;
    waveNumberText.setFont(font);
    waveNumberText.setString("Wave: 0");
    waveNumberText.setCharacterSize(55);
    waveNumberText.setFillColor(Color::White);
	waveNumberText.setPosition(1250, 980);

	// Health Bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);
	healthBar.setSize(Vector2f(300, 70));

	// Create an Instance of Player
	Player player;

	int numZombies;
	int numZombiesAlive;
	Zombie *zombies = nullptr;

	int waves = 1;

	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;

	Time gameTimeTotal;
	Time lastPressed;

    Clock clock;

	// Game Loop
	while(window.isOpen()){
		// ------------ Event Handling Loop ------------
		Event event;
		while(window.pollEvent(event)){							// Event handle
			if(event.type == Event::Closed){
				window.close();
			}

			if(event.type == Event::KeyPressed){
				if(event.key.code == Keyboard::Return && state == State::GAME_OVER){
					state = State::LEVELING_UP;
				}

				else if(event.key.code == Keyboard::Return && state == State::PLAYING){
					state = State::PAUSED;
				}

				else if(event.key.code == Keyboard::Return && state == State::PAUSED){
					state = State::PLAYING;
				}
			}
		}
		// --------- End of Event Handling Loop ---------

		// -------------- To Quit the Game --------------
		if(Keyboard::isKeyPressed(Keyboard::Escape)){ 			// Keyboard handle (Escape Key)
			window.close();
		}

		// -------- Handle W A S D keys while playing --------
		if(state == State::PLAYING){
			if(Keyboard::isKeyPressed(Keyboard::W)){
				player.moveUp();
			}else{
				player.stopUp();
			}

			if(Keyboard::isKeyPressed(Keyboard::A)){
				player.moveLeft();
			}else{
				player.stopLeft();
			}

			if(Keyboard::isKeyPressed(Keyboard::S)){
				player.moveDown();
			}else{
				player.stopDown();
			}

		    if(Keyboard::isKeyPressed(Keyboard::D)){
				player.moveRight();
			}else{
				player.stopRight();
			}
			// --------------------------- Fire a Bullet ---------------------------
			if(Mouse::isButtonPressed(Mouse::Left)){
				if(gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate){
					bullets[currentBullet].shoot(
						player.getCenter().x, // startX
						player.getCenter().y, // startY
						mouseWorldPosition.x, // targetX
						mouseWorldPosition.y  // targetY
					);

					currentBullet++;
					if(currentBullet > 99){
						currentBullet = 0;
					}
					lastPressed = gameTimeTotal;
				}
			}
		}

		// -------- Handle the LEVELING up state --------
		if(state == State::LEVELING_UP){
			if(event.key.code == Keyboard::Num1){
				state = State::PLAYING;
			}

			if(event.key.code == Keyboard::Num2){
				state = State::PLAYING;
			}

			if(event.key.code == Keyboard::Num3){
				state = State::PLAYING;
			}

			if(state == State::PLAYING){
				arena.width = 500 * waves;
            	arena.height = 500 * waves;
            	arena.left = 0;
            	arena.top = 0;
            	int tileSize = 50;

               	createBackground(background, arena);

            	// Spawn the player
            	player.spawn(arena, resolution, tileSize);

            	numZombies = 10;
            	numZombiesAlive = 10;
            	zombies = createHorde(numZombies, arena);

            	clock.restart();
			}
		}// ------------- End of LEVELING up -------------

		// ----------------- Update Part ----------------
		if(state == State::PLAYING){
			Time dt = clock.restart();
			float dtAsSeconds = dt.asSeconds();
			mouseScreenPosition = Mouse::getPosition();

			// Update total game time
			gameTimeTotal += dt;

			//Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			spriteCrosshair.setPosition(mouseWorldPosition);

			// Update Player
			player.update(dtAsSeconds, Mouse::getPosition());

			// Update Zombies
			Vector2f playerLocation(player.getCenter());
			for(int i = 0; i < numZombies; i++){
				if(zombies[i].isAlive()){
					zombies[i].update(dtAsSeconds, playerLocation);
				}
			}

			// Update any bullets that are InFlight
			for(int i=0; i<100; i++){
				if(bullets[i].isInFlight()){
					bullets[i].update(dtAsSeconds);
				}
			}
			// Collision between bullets and zombies
			for(int i=0; i<100; i++){
				for(int j=0; j<numZombies; j++){
					if(bullets[i].isInFlight() && zombies[j].isAlive()){
						if(bullets[i].getPosition().intersects(zombies[j].getPosition())){
							// Stop the bullet
							bullets[i].stop();

							if(zombies[j].hit()){
								score += 10;
								if(score >= hiScore){
									hiScore = score;
								}
								numZombiesAlive--;
								// When all zombies are dead
								if(numZombiesAlive == 0){
									state = State::LEVELING_UP;
								}
							}
						}
					}
				}
			} // End of zombie and bullet collision

			// Update Score HUD
			// Update the score text
			s.str("");
			s << "Score: " << score;
			scoreText.setString(s.str());

			// Update the high score text
			hs.str("");
			hs << "High Score: " << hiScore;
			hiScoreText.setString(hs.str());

			mainView.setCenter(player.getCenter());
		}
		// -------------- End of Update Part -------------


		// -------------- Draw the Scene --------------
		window.clear();
		if(state == State::PLAYING){
			window.setView(mainView);
			window.draw(background, &textureBackground);
			window.draw(player.getSprite());

			for(int i = 0; i < numZombies; i++){
				window.draw(zombies[i].getSprite());
			}

			for(int i=0; i<100; i++){
				if(bullets[i].isInFlight()){
					window.draw(bullets[i].getShape());
				}
			}

			window.draw(spriteCrosshair);
			window.setView(hudView);
			window.draw(healthBar);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(zombiesRemainingText);
			window.draw(waveNumberText);
		}

		if(state == State::LEVELING_UP){
			window.draw(levelUpText);
		}

		if(state == State::PAUSED){
			window.draw(pausedText);
		}

		if(state == State::GAME_OVER){
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}
		window.display();

	} // End of Game Loop

	return 0;
}

// Function to create scrolling background decoration
int createBackground(VertexArray &rVA, IntRect arena){
	const int TILE_SIZE = 50;
	const int TILE_TYPES = 3;
	const int VERTS_IN_QUAD = 4;

	rVA.setPrimitiveType(Quads);
	int worldWidth = arena.width/TILE_SIZE;
	int worldHeight = arena.height/TILE_SIZE;
	rVA.resize(worldWidth * worldHeight * VERTS_IN_QUAD);

	int currentVertex = 0;
	for (int w = 0; w < worldWidth; w++){ // Columns of the map
		for (int h = 0; h < worldHeight; h++){ // Rows of the map
			// Set the positions of 1 corner tile (top-left, top-right, bottom-right, bottom-left) of the entire arena
			rVA[currentVertex + 0].position = Vector2f(w * TILE_SIZE, h * TILE_SIZE);
			rVA[currentVertex + 1].position = Vector2f(w * TILE_SIZE + TILE_SIZE, h * TILE_SIZE);
			rVA[currentVertex + 2].position = Vector2f(w * TILE_SIZE + TILE_SIZE, h * TILE_SIZE + TILE_SIZE);
			rVA[currentVertex + 3].position = Vector2f(w * TILE_SIZE, h * TILE_SIZE + TILE_SIZE);

			// Filling Wall Tiles || Border check
			if(w == 0 || w == worldWidth-1 || h == 0 || h == worldHeight-1){
				rVA[currentVertex+0].texCoords = Vector2f(0, TILE_SIZE * TILE_TYPES);
				rVA[currentVertex+1].texCoords = Vector2f(TILE_SIZE, TILE_SIZE * TILE_TYPES);
				rVA[currentVertex+2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE * TILE_TYPES + TILE_SIZE);
				rVA[currentVertex+3].texCoords = Vector2f(0, TILE_SIZE * TILE_TYPES + TILE_SIZE);
			}else{ // Rest of the random flooring of the arena
				srand((int)time(0)+ h * w - h);
				int mOrG = (rand() % TILE_TYPES);
				int verticalOffset = mOrG * TILE_SIZE;
				rVA[currentVertex+0].texCoords = Vector2f(0, verticalOffset);
				rVA[currentVertex+1].texCoords = Vector2f(TILE_SIZE, verticalOffset);
				rVA[currentVertex+2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + verticalOffset);
				rVA[currentVertex+3].texCoords = Vector2f(0, TILE_SIZE + verticalOffset);
			}
			currentVertex = currentVertex + VERTS_IN_QUAD;
		} // h loop end
	} // w loop end
	return TILE_SIZE;
} //function end

Zombie *createHorde(int numZombies, IntRect arena){

	Zombie *zombies = new Zombie[numZombies];

	int minX = arena.left + 25;
	int maxX = arena.width - 25;
	int minY = arena.top + 25;
	int maxY = arena.height - 25;

	for (int i = 0; i < numZombies; i++){
		// Which side should the zombie spawn
		srand((int)time(0) * i);
		int side = (rand() % 4);
		float x, y;

		switch(side){
			case 0:  // left
				x = minX;
				y = (rand() % maxY) + minY;
				break;
			case 1:  // right
				x = maxX;
				y = (rand() % maxY) + minY;
				break;
			case 2:  // top
				x = (rand() % maxX) + minX;
				y = minY;
				break;
			case 3:  // bottom
				x = (rand() % maxX) + minX;
				y = maxY;
				break;
		}
		// Select the type of Zombie (Bloater, chaser, or crawler)
		srand((int)time(0) * i * 2);
		int type = (rand() % 3);

		//Spawn the Zombie
		zombies[i].spawn(x, y, type, i);

	} // loop end

	return zombies;
} // function end
