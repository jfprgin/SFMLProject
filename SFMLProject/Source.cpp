#include "SFML/Graphics.hpp"

sf::Vector2f viewSize(1024, 768);
sf::VideoMode vm(viewSize.x, viewSize.y);
sf::RenderWindow window(vm, "Hello SFMLGame!!!", sf::Style::Default);

sf::Texture skyTexture;
sf::Sprite skySprite;

sf::Texture bgTexture;
sf::Sprite bgSprite;

sf::Texture heroTexture;
sf::Sprite heroSprite;

sf::Vector2f playerPosition;
bool playerMoving = false;

void init() {
	// Load sky Texture
	skyTexture.loadFromFile("Assets/graphics/sky.png");
	// Set and  Attacha Texture to Sprite
	skySprite.setTexture(skyTexture);

	bgTexture.loadFromFile("Assets/graphics/bg.png");
	bgSprite.setTexture(bgTexture);

	heroTexture.loadFromFile("Assets/graphics/hero.png");
	heroSprite.setTexture(heroTexture);
	heroSprite.setPosition(sf::Vector2f(viewSize.x / 2, viewSize.y / 2));
	heroSprite.setOrigin(heroTexture.getSize().x / 2, heroTexture.getSize().y / 2);
}

void draw() {
	window.draw(skySprite);
	window.draw(bgSprite);
	window.draw(heroSprite);
}

void updateInput() {
	sf::Event event;

	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Right)
			{
				playerMoving = true;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Right)
			{
				playerMoving = false;
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
	if (playerMoving)
	{
		heroSprite.move(50.0f * dt, 0);
	}
}

int main() {
	////init game objects
	//sf::RectangleShape rect(sf::Vector2f(500.0f, 300.0f));
	//rect.setFillColor(sf::Color::Yellow);
	//rect.setPosition(viewSize.x / 2, viewSize.y / 2);
	//rect.setOrigin(sf::Vector2f(rect.getSize().x / 2, rect.getSize().y / 2));

	//sf::CircleShape circle(100);
	//circle.setFillColor(sf::Color::Green);
	//circle.setPosition(viewSize.x / 2, viewSize.y / 2);
	//circle.setOrigin(sf::Vector2f(circle.getRadius(), circle.getRadius()));
	//
	//sf::ConvexShape triangle;
	//triangle.setPointCount(3);
	//triangle.setPoint(0, sf::Vector2f(-100, 0));
	//triangle.setPoint(1, sf::Vector2f(0, -100));
	//triangle.setPoint(2, sf::Vector2f(100, 0));
	//triangle.setFillColor(sf::Color(128, 0, 128, 255));
	//triangle.setPosition(viewSize.x / 2, viewSize.y / 2);
	sf::Clock clock;
	init();

	while (window.isOpen()) {
		//Update input
		updateInput();

		//Update Game
		sf::Time dt = clock.restart();
		update(dt.asSeconds());

		window.clear(sf::Color::Red);
		draw();

		window.display();
	}

	return 0;
}