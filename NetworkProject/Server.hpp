#ifndef SERVER_HPP_INCLUDED
#define SERVER_HPP_INCLUDED

#pragma once

#include <exception>
#include <iostream>
#include <vector>
#include <sstream>
#include <thread>
#include <map>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <ServerConfiguration.hpp>
#include <NetworkValues.hpp>

#include <RenderGame.hpp>
#include <ResourceHolder.hpp>
#include <ResourceIdentifier.hpp>
#include <PlayerBox.hpp>

/*
	Server class.
	This class manages the main functions of the server.
*/
class Server
{
public:
	// Constructor.
	Server();

	// Destructor.
	virtual ~Server();

	// Inits the server.
	void Init();

	// Returns true if the server is running, false otherwise.
	bool IsRunning() const;

	// Handle Incoming Socket Connections
	void HandleIncomingConnections();

	// Receives the input from the clients.
	void ReceiveInputFromSocket();

	// Receives the input from the clients.
	void ServerTick(sf::Time dt);

	// Sends the update to the clients.
	void SendUpdate();

	// Sends update to a particular client
	void SendUDPUpdateToClient(ClientRef* client, sf::UdpSocket& socket);
	
	// Returns the number of players currently online.
	unsigned int GetNumberOfPlayers() const;

	// Return the maximum players capacity of the server.
	unsigned int GetMaximumPlayersCapacity() const;

	// Returns the list of clients.
	std::vector<ClientRef*>* GetClients();

	// Shuts down the server.
	void Shutdown();

	// Execution thread for listners
	void ExecutionThread();

protected:
	// Disconnects the player for the given reason.
	void DisconnectPlayer(ClientRef* clientRef, std::string reason);

	// Updates the number of players.
	void UpdateNumberOfPlayers();

	// Updates the timeout of the players.
	void UpdateTimeoutPlayers(sf::Time dt);

	// Receives the input from the clients TCP sockets.
	void ReceiveThroughTCP();

	// Receives the input from the clients TCP sockets.
	void ReceiveThroughUDP();

	// Notifies everyone the player connected.
	void NotifyPlayerConnected(std::string username);

	// Notifies everyone the player disconnected.
	void NotifyPlayerDisconnected(std::string username);

// Variables
protected:
	RenderGame renderGame;

	sf::Thread			mThread;
	sf::Clock			mClock;

	bool mWaitingThreadEnd;

	// Running flag.
	bool m_isRunning;

	// Number of players connected and maximum number of players.
	unsigned int m_numberOfPlayers;
	unsigned int m_maximumPlayersCapacity;

	// List of the clients.
	std::vector<ClientRef*> m_clients;

	// The game TCP listener.
	sf::TcpListener m_gameTcpListener;

	// The game UDP socket.
	sf::UdpSocket m_gameUdpSocket;

};
#endif