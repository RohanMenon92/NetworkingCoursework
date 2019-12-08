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
	std::unique_ptr<Game> game;

	sf::Event sfEvent;

	std::string username;
	bool isConnected;

	bool mWaitingThreadEnd;
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
	void InitGame(sf::Vector2f startPos);
	void ConnectToServer();

	//Functions
	void ExecutionThread();
	void ReceiveInput();
	void ReceiveUdpPacket();
	void UpdateBullet(std::string bulletID, sf::Vector2f pos, sf::Vector2f velocity);
	void UpdatePlayer(std::string playerID, sf::Vector2f pos, sf::Vector2f velocity, sf::Vector2f aimAt, float health, bool isAttacking, bool isBlocking);
	void ReceiveTcpPacket();
	void ClientTick(sf::Time);
	void SpawnPlayer(std::string playerID, sf::Vector2f pos, sf::Vector2f velocity, float health);
	void SpawnBullet(std::string bulletID, sf::Vector2f pos, sf::Vector2f velocity);

public:
	std::map<std::string, Player*> players;
	std::map<std::string, Bullet*> bullets;

	//constructor/destructors
	Client(std::string newUsername);
	virtual ~Client();
	void Init();
};
#endif