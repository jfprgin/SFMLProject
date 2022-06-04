#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include <vector>

#include "Hero.h"
#include "Enemy.h"
#include "Rocket.h"


sf::Vector2f viewSize(1024, 768);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Hello SFMLGame!!!", sf::Style::Default);

sf::Texture skyTexture;
sf::Sprite skySprite;

sf::Texture bgTexture;
sf::Sprite bgSprite;

Hero hero;
std::vector<Enemy*> enemies;
std::vector<Rocket*> rockets;

sf::Vector2f playerPosition;
bool playerMoving{ false };

float currentTime;
float prevTime { 0.0f };

int score{0};
bool gameover{ true };

//Text
sf::Font headingFont;
sf::Text headingText;

sf::Font scoreFont;
sf::Text scoreText;

sf::Text tutorialText;

//Audio
sf::Music bgMusic;

sf::SoundBuffer fireBuffer;
sf::SoundBuffer hitBuffer;

sf::Sound fireSound(fireBuffer);
sf::Sound hitSound(hitBuffer);

void init();
void draw();
void updateInput();
void update(float dt);
void spawnEnemy();
void shoot();
bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2);
void reset();

int main() 
{
	sf::Clock clock;
	window.setFramerateLimit(60);
	//init game objects
	init();

	while (window.isOpen()) {
		//Update input
		updateInput();

		//Update Game
		sf::Time dt = clock.restart();
		if (!gameover)
		{
			update(dt.asSeconds());
		}

		window.clear(sf::Color::Red);
		draw();

		window.display();
	}

	return 0;
}

void init()
{
	// Load sky Texture
	skyTexture.loadFromFile("Assets/graphics/sky.png");
	// Set and  Attacha Texture to Sprite
	skySprite.setTexture(skyTexture);

	//Load font
	headingFont.loadFromFile("Assets/fonts/SnackerComic.ttf");
	//Set Heading Text
	headingText.setFont(headingFont);
	headingText.setString("Tiny Bazooka");
	headingText.setCharacterSize(84);
	headingText.setFillColor(sf::Color::Red);

	sf::FloatRect headingBounds = headingText.getLocalBounds();
	headingText.setOrigin(headingBounds.width / 2, headingBounds.height / 2);
	headingText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));
	
	//Load font
	scoreFont.loadFromFile("Assets/fonts/arial.ttf");
	//Set Score Text
	scoreText.setFont(scoreFont);
	scoreText.setString("Score: 0");
	scoreText.setCharacterSize(45);
	scoreText.setFillColor(sf::Color::Red);

	sf::FloatRect scoreBounds = scoreText.getLocalBounds();
	scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
	scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

	//Text Tutorail
	tutorialText.setFont(scoreFont);
	tutorialText.setString("Press Down Arrow to Fire and Start Game, Up Arrow to Jump");
	tutorialText.setCharacterSize(35);
	tutorialText.setFillColor(sf::Color::Red);

	sf::FloatRect tutorialBounds = tutorialText.getGlobalBounds();
	tutorialText.setOrigin(tutorialBounds.width / 2, tutorialBounds.height / 2);
	tutorialText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.20f));

	// Load Background Texture
	bgTexture.loadFromFile("Assets/graphics/bg.png");
	bgSprite.setTexture(bgTexture);

	hero.init("Assets/graphics/hero.png", sf::Vector2f(viewSize.x * 0.25f, viewSize.y * 0.5f), 200);
	srand((int)time(0));

	//Audio
	bgMusic.openFromFile("Assets/audio/bgMusic.ogg");
	bgMusic.play();

	hitBuffer.loadFromFile("Assets/audio/hit.ogg");
	fireBuffer.loadFromFile("Assets/audio/fire.ogg");
}

void draw()
{
	window.draw(skySprite);
	window.draw(bgSprite);
	window.draw(hero.getSprite());

	for (Enemy *enemy : enemies)
	{
		window.draw(enemy->getSprite());
	}

	for (Rocket *rocket : rockets)
	{
		window.draw(rocket->getSprite());
	}

	if (gameover)
	{
		window.draw(headingText);
		window.draw(tutorialText);
	}
	else
	{
		window.draw(scoreText);
	}
}

void updateInput()
{
	sf::Event event;

	// while there are pending events...
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Up)
			{
				hero.jump(750.0f);
			}
		}
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Down)
			{
				if (gameover)
				{
					gameover = false;
					reset();
				}
				else
				{
					shoot();
				}
			}
		}
		if (event.key.code == sf::Keyboard::Escape || event.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}

void update(float dt)
{
	hero.update(dt);
	currentTime += dt;
	//Spawn Enemies
	if (currentTime >= prevTime + 1.125f)
	{
		spawnEnemy();
		prevTime = currentTime;
	}

	//Update Enemies
	for (int i = 0; i < enemies.size(); i++)
	{
		Enemy *enemy = enemies[i];

		enemy->update(dt);

		if (enemy->getSprite().getPosition().x < 0)
		{
			enemies.erase(enemies.begin() + i);
			delete(enemy);
			gameover = true;
		}
	}

	//Update Rockets
	for (int i = 0; i < rockets.size(); i++)
	{
		Rocket *rocket = rockets[i];

		rocket->update(dt);

		if (rocket->getSprite().getPosition().x > viewSize.x)
		{
			rockets.erase(rockets.begin() + i);
			delete(rocket);
		}
	}

	//Check collision between Rocket and Enemies
	for (int i = 0; i < rockets.size(); i++)
	{
		for (int j = 0; j < enemies.size(); j++)
		{
			Rocket* rocket = rockets[i];
			Enemy* enemy = enemies[j];

			if (checkCollision(rocket->getSprite(), enemy->getSprite()))
			{
				hitSound.play();

				score++;
				std::string finalScore = "Score: " + std::to_string(score);
				scoreText.setString(finalScore);
				sf::FloatRect scoreBounds = scoreText.getLocalBounds();
				scoreText.setOrigin(scoreBounds.width / 2, scoreBounds.height / 2);
				scoreText.setPosition(sf::Vector2f(viewSize.x * 0.5f, viewSize.y * 0.10f));

				rockets.erase(rockets.begin() + i);
				enemies.erase(enemies.begin() + j);

				delete(rocket);
				delete(enemy);

				printf("Rocket intersects enemy\n");
			}
		}
	}
}

void spawnEnemy()
{
	int randLoc = rand() % 3;
	sf::Vector2f enemyPos;
	float speed;

	switch (randLoc)
	{
	case 0:
		enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.75f);
		speed = -400;
		break;
	case 1:
		enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.60f);
		speed = -550;
		break;
	case 2:
		enemyPos = sf::Vector2f(viewSize.x, viewSize.y * 0.40f);
		speed = -650;
		break;
	default:
		printf("incorrect y value |\n");
		break;
	}

	Enemy* enemy = new Enemy();
	enemy->init("Assets/graphics/enemy.png", enemyPos, speed);

	enemies.push_back(enemy);
}

void shoot()
{
	Rocket* rocket = new Rocket();

	rocket->init("Assets/graphics/rocket.png", hero.getSprite().getPosition(), 400.0f);

	rockets.push_back(rocket);
	fireSound.play();
}

bool checkCollision(sf::Sprite sprite1, sf::Sprite sprite2)
{
	sf::FloatRect shape1 = sprite1.getGlobalBounds();
	sf::FloatRect shape2 = sprite2.getGlobalBounds();

	if (shape1.intersects(shape2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void reset()
{
	score = 0;
	currentTime = 0.0f;
	prevTime = 0.0;
	scoreText.setString("Score: 0");

	for (Enemy *enemy : enemies)
	{
		delete(enemy);
	}
	for (Rocket *rocket : rockets)
	{
		delete(rocket);
	}

	enemies.clear();
	rockets.clear();
}