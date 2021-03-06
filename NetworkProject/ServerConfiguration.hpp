#ifndef SERVERCONFIGURATION_HPP_INCLUDED
#define SERVERCONFIGURATION_HPP_INCLUDED

#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include <Player.hpp>

struct ClientRef
{
	int playerNumber;

	float latencyTime;

	sf::TcpSocket gameTcpSocket;
	sf::UdpSocket gameUdpSocket;

	sf::IpAddress ip;
	unsigned int udpPort;

	sf::Time timeout = sf::Time::Zero;

	bool gameTcpConnected = false;
	bool gameUdpConnected = false;
	bool ingame = false;

	bool isAttackPressed = false;
	bool isBlockPressed = false;
	bool isForwardPressed = false;

	Player* playerBox;
	sf::Vector2f targetPosition;

	std::string clientName;

	unsigned long long lastPacketIdSent = 0;
	sf::Time lastPacketTime;
	unsigned int lostPackets = 0;
};

namespace ServerConfiguration
{
	const sf::Time Ticktime = sf::seconds(1.f / GameConstants::TickPerSec);

	const sf::Time InformationServerTimeout = sf::seconds(30.f);
	const sf::Time PendingSocketTimeout = sf::seconds(5.f);
	const sf::Time GameServerTimeout = sf::seconds(5.f);

	const unsigned int InformationPort(22626);
	const unsigned int GameTCPPort(21101); // Connects TCP from server port
	const unsigned int ClientTCPPort(21102); // Connects TCP from client port
	const unsigned int ServerUDPPort(21103); // Connects UDP from server
	const unsigned int GameClientUDPPort(21104); // Connevts UDP from client
	const std::string LocalIPAddress = "127.0.0.1";
} // namespace ServerConfiguration.

#endif // SERVERCONFIGURATION_HPP_INCLUDED
