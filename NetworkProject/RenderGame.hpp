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

#include <MusicPlayer.hpp>
#include <SoundPlayer.hpp>

class RenderGame
{
public:
	sf::RenderWindow *mWindow;
	sf::Event sfEvent;

	sf::Clock dtClock;
	float dt;

	std::vector<PlayerBox> playerBoxes;
	std::vector<Bullet> bullets;

public:
	RenderGame();
	void run();
	sf::Vector2f OnSocketConnect(ClientRef* clientRef);
	void OnPlayerConnect(std::string username, PlayerBox* playerBox);
	void OnPlayerAimMove(std::string username, sf::Vector2f position);
	void OnPlayerDisconnect(std::string username);

protected:
	void InitWindow();

private:
	void ProcessPlayerInput();
	void UpdateSFMLEvents();
	void Update(sf::Time dt);
	void Render();

private:
	static const sf::Time	TimePerFrame;
};
#endif