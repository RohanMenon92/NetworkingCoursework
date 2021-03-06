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

	sf::IpAddress serverIP;

	std::string username;
	bool isConnected;


	bool dontExecuteClient;
	//sf::Thread mThread;
	sf::Clock mClock;

	sf::UdpSocket renderUDPSocket;
	sf::TcpSocket controlTCPSocket;

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
	Client(std::string newUsername, std::string hostIP);
	virtual ~Client();

	void InitGame(sf::Vector2f startPos, float startTime);
};
#endif