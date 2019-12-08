#ifndef GAME_H
#define GAME_H

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>


#include <ServerConfiguration.hpp>
#include <ResourceIdentifier.hpp>
#include <PlayerBox.hpp>
#include <Bullet.hpp>

#include <iostream>
#include <fstream>

class Game
{
private:
	//variables
	sf::Event sfEvent;

	sf::Clock dtClock;
	//Initialization

	std::map<std::string, Player*>* playersPtr;
	std::map<std::string, Bullet*>* bulletsPtr;

public:
	sf::RenderWindow *window;

	sf::Vector2f mousePos;
	bool isAttackPressed = false;
	bool isBlockPressed = false;
	bool isForwardPressed = false;

	//constructor/destructor
	Game();
	virtual ~Game();

	void initWindow();

	void RunGame(sf::Vector2f spawnPlayerPosition);

	sf::Vector2f Interpolate2f(const sf::Vector2f & pointA, const sf::Vector2f & pointB, float factor);

	float InterpolateFloat(const float & pointA, const float & pointB, float factor);

	//Functions
	void ProcessInput();
	void UpdateSFMLEvents();
	void Update(sf::Time dt, std::map<std::string, Player*>& playerPointer, std::map<std::string, Bullet*>& bulletPointer);
	void Render(std::map<std::string, Player*>& playerPointer, std::map<std::string, Bullet*>& bulletPointer);
	void DespawnPlayer(std::string playerID);
	void DespawnBullet(std::string bulletID);

	//void UpdateBullet(std::string bulletID, sf::Vector2f pos, sf::Vector2f velocity);
	//void UpdatePlayer(std::string playerID, sf::Vector2f pos, sf::Vector2f velocity, sf::Vector2f aimAt, float health, bool isAttacking, bool isBlocking);
};

#endif