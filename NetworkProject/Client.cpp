#include "Client.hpp"

Client::Client(std::string newUsername)
	: mThread(&Client::ExecutionThread, this)
	,game()
	,username(newUsername)
{
	mThread.launch();
	ConnectToServer();
}

Client::~Client()
{
	mWaitingThreadEnd = true;
	mThread.wait();
}


void Client::InitGame(sf::Vector2f startPos)
{
	game.RunGame(startPos);
}

void Client::ConnectToServer()
{
	controlSocket.setBlocking(true);

	std::cout << "[GAME_CLIENT] IN CONNECT TO SERVER " << std::endl;
	sf::Packet packet;
	// Connect to TCP
	sf::TcpSocket::Status status = controlSocket.connect(serverIP, ServerConfiguration::GameTCPPort, sf::seconds(5.f));

	if (status != sf::Socket::Done)
	{
		std::cout << "[GAME_CLIENT] WAS UNABLE TO CONNECT TO SERVER " << status << std::endl;
	}

	// Wait for initial connect packet

	sf::Packet connectPacket;
	// Listen and recieve generated position
	if (controlSocket.receive(connectPacket) == sf::TcpSocket::Status::Done) {

		unsigned int netCode(0);
		connectPacket >> netCode;

		// On Connect packet, extract server defined spawn position
		if (netCode != NetworkValues::CONNECT) {
			std::cout << "[GAME_CLIENT] [ERROR] Did not recieve Connect packet! " << std::endl;
			return;
		}

		sf::Vector2f startPos;
		connectPacket >> startPos.x >> startPos.y;

		packet << NetworkValues::CONNECT << username;
		controlSocket.send(packet);
		// Start the game based on server set positions
		InitGame(startPos);
	}
	controlSocket.setBlocking(false);
}

void Client::ExecutionThread()
{
	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{
		ReceiveInput();

		stepTime += stepClock.getElapsedTime();
		stepClock.restart();

		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Fixed update step
		while (stepTime >= stepInterval)
		{
			//mBattleFieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			ClientTick(tickInterval);
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void Client::ReceiveInput()
{
	
}

void Client::ClientTick(sf::Time)
{
}

void Client::Init()
{
}

void Client::Run()
{
}

void Client::UpdateDT()
{
}

void Client::UpdateSFMLEvents()
{
}

void Client::Update()
{
}

void Client::Render()
{
}
