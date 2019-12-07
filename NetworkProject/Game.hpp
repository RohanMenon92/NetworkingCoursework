#ifndef GAME_H
#define GAME_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <ResourceIdentifier.hpp>
#include <PlayerBox.hpp>
#include <Bullet.hpp>
#include <Multithreading.hpp>

#include <iostream>
#include <fstream>

class Game
{
private:
	//variables
	sf::RenderWindow *window;
	sf::Event sfEvent;

	sf::Clock dtClock;
	//Initialization

	std::vector<Player*>* players;
	std::vector<Bullet*>* bullets;


public:
	sf::Vector2f mousePos;
	bool isAttackPressed = false;
	bool isBlockPressed = false;
	bool isForwardPressed = false;

	//constructor/destructor
	Game(std::vector<Player*>& playerPointer, std::vector<Bullet*>& bulletPointer);
	//virtual ~Game();

	void initWindow();

	void RunGame(sf::Vector2f spawnPlayerPosition);

	//Functions
	void ProcessInput();
	void UpdateSFMLEvents();
	void Update(sf::Time dt);
	void Render();
	void DespawnPlayer(std::string playerID);
	void DespawnBullet(std::string bulletID);

	//void UpdateBullet(std::string bulletID, sf::Vector2f pos, sf::Vector2f velocity);
	//void UpdatePlayer(std::string playerID, sf::Vector2f pos, sf::Vector2f velocity, sf::Vector2f aimAt, float health, bool isAttacking, bool isBlocking);
};

#endif