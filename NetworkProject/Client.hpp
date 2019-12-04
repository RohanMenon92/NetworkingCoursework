#ifndef CLIENT_H
#define CLIENT_H

#pragma once

#include <iostream>
#include <fstream>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window/Mouse.hpp>

#include <Game.hpp>
#include <NetworkValues.hpp>
#include <ServerConfiguration.hpp>

class Client
{
private:
	//variables
	Game game;
	sf::Event sfEvent;

	std::string username;

	bool mWaitingThreadEnd; 
	sf::Thread mThread;
	sf::Clock mClock;

	sf::UdpSocket renderSocket;
	sf::TcpSocket controlSocket;

	sf::Clock dtClock;
	float dt;

	// ip of the seveur
	sf::IpAddress serverIP = "127.0.0.1";

	//initialization
	void InitGame(sf::Vector2f startPos);
	void ConnectToServer();
public:
	//constructor/destructor
	Client(std::string newUsername);
	virtual ~Client();

	//Functions
	void ExecutionThread();
	void ReceiveInput();
	void ClientTick(sf::Time);
	void Init();
	void Run();
	void UpdateDT();
	void UpdateSFMLEvents();
	void Update();
	void Render();
};
#endif