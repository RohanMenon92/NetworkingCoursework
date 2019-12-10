#ifndef RENDERGAME_H
#define RENDERGAME_H

#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>

#include<ServerConfiguration.hpp>
#include<PlayerBox.hpp>
#include<Bullet.hpp>

class RenderGame
{
public:
	sf::Font fontText;

	sf::RenderWindow *mWindow;
	sf::Event sfEvent;

	sf::Clock dtClock;
	float dt;

	std::vector<Player*> players;
	std::vector<Bullet*> bullets;

public:
	RenderGame();
	void run();
	sf::Vector2f OnSocketConnect(ClientRef* clientRef);
	void OnPlayerConnect(std::string username, Player* playerBox);
	void OnPlayerDisconnect(std::string username);

protected:
	void InitWindow();

private:
	void ProcessPlayerInput();
	void UpdateSFMLEvents();
	float GetRotationAim(sf::Vector2f currPos, sf::Vector2f aimPos);
	void Update(sf::Time dt);
	void Render();
	sf::Vector2f Interpolate2f(const sf::Vector2f& pointA, const sf::Vector2f& pointB, float factor);

	float InterpolateFloat(const float & pointA, const float & pointB, float factor);

private:
	static const sf::Time	TimePerFrame;
};
#endif