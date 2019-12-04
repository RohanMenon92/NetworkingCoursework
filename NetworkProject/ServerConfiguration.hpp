#ifndef SERVERCONFIGURATION_HPP_INCLUDED
#define SERVERCONFIGURATION_HPP_INCLUDED

#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include <PlayerBox.hpp>

struct ClientRef
{
	std::shared_ptr<sf::TcpSocket> gameTcpPtr;
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

	PlayerBox* playerBox;
	sf::Vector2f targetPosition;

	std::string clientName;

	unsigned long long lastPacketIdSent = 0;
	sf::Time lastPacketTime;
	unsigned int lostPackets = 0;
};

namespace ServerConfiguration
{
	const unsigned int Version(002);
	const float TickPerSec(60.f);
	const sf::Time Ticktime = sf::seconds(1.f / TickPerSec);

	const sf::Time InformationServerTimeout = sf::seconds(30.f);
	const sf::Time PendingSocketTimeout = sf::seconds(5.f);
	const sf::Time GameServerTimeout = sf::seconds(5.f);

	const unsigned int InformationPort(22626);
	const unsigned int GameTCPPort(22624);
	const unsigned int ClientTCPPort(22625);
	const unsigned int GameUDPPort(22623);
} // namespace ServerConfiguration.

#endif // SERVERCONFIGURATION_HPP_INCLUDED
