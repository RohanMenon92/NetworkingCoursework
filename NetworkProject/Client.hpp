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
#include <MultiThreading.hpp>

class Client
{
private:
	//variables
	sf::Font fontText;

	std::unique_ptr<Game> game;

	sf::Event sfEvent;

	std::string username;
	bool isConnected;

	bool dontExecuteClient;
	//sf::Thread mThread;
	sf::Clock mClock;

	sf::UdpSocket renderSocket;
	sf::TcpSocket controlSocket;

	sf::Clock dtClock;
	float dt;

	// Udp packet number Recieved
	unsigned long long udpPacketNumberReceive;
	// loss packets
	unsigned long long lossPacket;
	// time OutTimer
	sf::Time timeOutTimer;

	bool isAttackPressed = false;
	bool isBlockPressed = false;
	bool isForwardPressed = false;

	sf::Vector2f mousePos;

	// ip of the seveur
	sf::IpAddress serverIP = ServerConfiguration::HostIPAddress;

	//initialization
	void ConnectToServer();

	//Functions
	void SetupClient();
	void ReceiveInput();
	void ReceiveUdpPacket();
	void ReceiveTcpPacket();
	void ClientTick(sf::Time);
	void UpdatePlayer(PlayerMessage playerMessage);
	void SpawnPlayer(PlayerMessage playerMessage);
	void UpdateBullet(BulletMessage bulletMessage);
	void SpawnBullet(BulletMessage bulletMessage);

public:
	int playerNumber;
	std::map<std::string, Player*> players;
	std::map<std::string, Bullet*> bullets;

	//constructor/destructors
	Client(std::string newUsername);
	virtual ~Client();
	void Init();
	void InitGame(sf::Vector2f startPos, float startTime);
};
#endif