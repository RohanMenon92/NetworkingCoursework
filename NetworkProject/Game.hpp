#ifndef GAME_H
#define GAME_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window/Mouse.hpp>

#include <ResourceIdentifier.hpp>
#include <PlayerBox.hpp>
#include <Bullet.hpp>

#include <iostream>
#include <fstream>

class Game
{
private:
	//variables
	sf::RenderWindow *window;
	sf::Event sfEvent;

	sf::Clock dtClock;
	
	sf::Vector2f mousePos;
	std::vector<PlayerBox> playerBoxes;
	std::vector<Bullet> bullets;

	//Initialization
	void initWindow();
	
public:
	//constructor/destructor
	Game();
	virtual ~Game();

	void RunGame(sf::Vector2f spawnPlayerPosition);

	//Functions
	void ProcessInput();
	void UpdateSFMLEvents();
	void Update(sf::Time dt);
	void Render();
};

#endif