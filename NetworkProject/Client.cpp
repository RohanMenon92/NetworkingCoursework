#include "Client.hpp"

Client::Client(std::string newUsername)
	: mThread(&Client::ExecutionThread, this)
	, isConnected(false)
	, mWaitingThreadEnd(false)
	, controlSocket()
	, renderSocket()
	, udpPacketNumberReceive(0)
	, lossPacket(0)
	, timeOutTimer(sf::Time::Zero)
	, username(newUsername)
{
	renderSocket.bind(ServerConfiguration::GameClientUDPPort);

	// Initially blocking
	controlSocket.setBlocking(true);
	renderSocket.setBlocking(false);

	mThread.launch();
}

Client::~Client()
{
	// Send Disconnection to Server
	sf::Packet packet;
	packet << NetworkValues::DISCONNECT << username;

	controlSocket.send(packet);

	mWaitingThreadEnd = true;
	mThread.wait();
}

void Client::Init()
{
	ConnectToServer();
}

void Client::InitGame(sf::Vector2f startPos)
{
	// Bind UDP Socket

	// Create game elements and start receving control
	game.reset(new Game(players, bullets));
	game->initWindow();
	game->RunGame(startPos);

	isConnected = true;
	controlSocket.setBlocking(false);
}

void Client::ConnectToServer()
{
	//std::cout << "[GAME_CLIENT] IN CONNECT TO SERVER " << std::endl;
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
			//std::cout << "[GAME_CLIENT] [ERROR] Did not recieve Connect packet! " << std::endl;
			return;
		}

		sf::Vector2f startPos;
		connectPacket >> startPos.x >> startPos.y;

		std::cout << "[GAME_CLIENT] Recieve Connect packet! Sending username " << std::endl;

		packet << NetworkValues::CONNECT << username << renderSocket.getLocalPort();
		controlSocket.send(packet);
		// Start the game based on server set positions
		InitGame(startPos);
	}
}

void Client::ExecutionThread()
{
	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 40.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{
		if (isConnected) {
			ReceiveInput();
			ReceiveUdpPacket();
			ReceiveTcpPacket();
		}

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

		// Sleep to prevent client from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void Client::ReceiveInput()
{
	if (isConnected) {
		std::cout << "[GAME_CLIENT] Receiving Input " << std::endl;
		// Recieve Input from Game
		mousePos = game->mousePos;
		isAttackPressed = game->isAttackPressed;
		isForwardPressed = game->isForwardPressed;
		isBlockPressed = game->isBlockPressed;
	}
}

void Client::ReceiveTcpPacket()
{
	//sf::Packet packet;
	//while (controlSocket.receive(packet) == sf::Socket::Status::Done)
	//{
	//	timeOutTimer = sf::Time::Zero;

	//	// Extract network code.
	//	unsigned int networkCode;
	//	packet >> networkCode;

	//	//switch (networkCode)
	//	//{
	//	//	case NetworkValues::SPAWN_PLAYER:
	//	//	{
	//	//		std::string username;
	//	//		float posX;
	//	//		float posY;
	//	//		float velocityX;
	//	//		float velocityY;
	//	//		packet >> username >> posX >> posY >> velocityX >> velocityY;

	//	//		game.SpawnPlayer(username, { posX, posY }, { velocityX, velocityY });
	//	//	}
	//	//	break;
	//	//	case NetworkValues::SPAWN_BULLET:
	//	//	{
	//	//		std::string bulletID;
	//	//		float posX;
	//	//		float posY;
	//	//		float velocityX;
	//	//		float velocityY;
	//	//		packet >> bulletID >> posX >> posY >> velocityX >> velocityY;

	//	//		game.SpawnBullet(username, { posX, posY }, { velocityX, velocityY });
	//	//	}
	//	//	break;
	//	//	case NetworkValues::DESPAWN_PLAYER:
	//	//	{
	//	//		std::string username;
	//	//		packet >> username;

	//	//		game.DespawnPlayer(username);
	//	//	}
	//	//	break;
	//	//	case NetworkValues::DESPAWN_BULLET:
	//	//	{
	//	//		std::string bulletID;
	//	//		packet >> bulletID;

	//	//		game.DespawnPlayer(bulletID);
	//	//	}
	//	//	break;
	//	//}
	//}
}

void Client::ReceiveUdpPacket()
{
	if (!isConnected) {
		return;
	}

	timeOutTimer += sf::seconds(1 / 60.f);
	sf::Packet packet;

	packet.clear();
	sf::IpAddress ip = ServerConfiguration::HostIPAddress;
	unsigned short int remotePort(ServerConfiguration::GameUDPPort);

	while (renderSocket.receive(packet, ip, remotePort) == sf::Socket::Status::Done)
	{
		std::cout << "[Client RECIEVE UDP SOCKET] IP:" << ip << "  remotePort:" << remotePort << std::endl;
		timeOutTimer = sf::Time::Zero;
		unsigned int netCode(0);
		packet >> netCode;

		unsigned long long number;
		packet >> number;

		switch (netCode)
		{
		case NetworkValues::RENDER_BULLET:
			std::cout << "[Client RECIEVE UDP SOCKET] RENDER BULLET FROM UDP " << std::endl;

			if (number - udpPacketNumberReceive > 1)
			{
				lossPacket += (number - 1) - udpPacketNumberReceive;
				std::cout << "[PACKET_LOSS] " << lossPacket << " / " << udpPacketNumberReceive
					<< " (" << (lossPacket * 100.f) / (float)(udpPacketNumberReceive) << "%)" << std::endl;
			}
			if (number > udpPacketNumberReceive)
			{
				udpPacketNumberReceive = number;

				std::string bulletID;
				float posX;
				float posY;
				float velocityX;
				float velocityY;
				packet >> bulletID >> posX >> posY >> velocityX >> velocityY;

				std::cout << " [CLIENT GAME UPDATE BULLET ] ";
				sf::Vector2f pos(posX, posY);
				sf::Vector2f velocity(velocityX, velocityY);
				UpdateBullet(bulletID, pos, velocity);
			}
			break;
		case NetworkValues::RENDER_PLAYER:
			std::cout << "[Client RECIEVE UDP SOCKET] RENDER PLAYER FROM UDP " << std::endl;

			if (number - udpPacketNumberReceive > 1)
			{
				lossPacket += (number - 1) - udpPacketNumberReceive;
				std::cout << "[PACKET_LOSS] " << lossPacket << " / " << udpPacketNumberReceive
					<< " (" << (lossPacket * 100.f) / (float)(udpPacketNumberReceive) << "%)" << std::endl;
			}
			if (number > udpPacketNumberReceive)
			{
				std::string username;
				float posX;
				float posY;
				float velocityX;
				float velocityY;
				float aimX;
				float aimY;
				float health;
				bool isAttacking;
				bool isBlocking;
				packet >> username >>
					posX >> posY >> velocityX >> velocityY >> aimX >> aimY >>
					health >> isAttacking >> isBlocking;

				sf::Vector2f pos(posX, posY);
				sf::Vector2f velocity(velocityX, velocityY);
				sf::Vector2f aim(aimX, aimY);

				UpdatePlayer(username, pos, velocity, aim, health, isAttacking, isBlocking);
			}
			break;
		}
	}
}

void Client::UpdateBullet(std::string bulletID, sf::Vector2f pos, sf::Vector2f velocity)
{
	bool bulletFound = false;
	for (Bullet* bullet : bullets) {
		if (bulletID == bullet->bulletID) {
			bulletFound = true;
			bullet->shape.setPosition(pos);
			bullet->velocity = velocity;
		}
	}

	if (!bulletFound) {
		SpawnBullet(bulletID, pos, velocity);
	}
}

void Client::UpdatePlayer(std::string playerID, sf::Vector2f pos, sf::Vector2f velocity, sf::Vector2f aimAt, float health, bool isAttacking, bool isBlocking)
{
	bool playerBoxFound = false;
	for (Player* playerBox : players) {
		if (playerID == playerBox->playerID) {
			playerBoxFound = true;
			playerBox->shape.setPosition(pos);
			playerBox->velocity = velocity;
			playerBox->isAttacking = isAttacking;
			playerBox->isBlocking = isBlocking;
			playerBox->aimAt = aimAt;
			playerBox->health = health;
		}
	}

	if (!playerBoxFound) {
		SpawnPlayer(playerID, pos, velocity, health);
	}
}

Player* Client::SpawnPlayer(std::string playerID, sf::Vector2f pos, sf::Vector2f velocity, float health)
{
	Multithreading::renderMutex.lock();
	std::cout << "Player has been created " << playerID << std::endl;

	std::unique_ptr<Player> player(new Player());

	player->playerID = playerID;
	player->shape.setPosition(pos);
	player->velocity = velocity;
	player->health = health;

	players.push_back(player.get());
	Multithreading::renderMutex.unlock();
	return players.back();
	//pendingPlayerBoxes.insert(std::pair<std::string, PlayerBox*>(playerID, playerBox));
}

Bullet* Client::SpawnBullet(std::string bulletID, sf::Vector2f pos, sf::Vector2f velocity)
{
	Multithreading::renderMutex.lock();
	std::cout << "Bullet has been created " << bulletID << std::endl;

	std::unique_ptr<Bullet> bullet(new Bullet());

	bullet->bulletID = bulletID;
	bullet->shape.setPosition(pos);
	bullet->velocity = velocity;
	bullets.push_back(bullet.get());
	Multithreading::renderMutex.unlock();
	return bullets.back();
	//pendingBullets.insert(std::pair<std::string, Bullet*>(bulletID, bullet));
}


void Client::ClientTick(sf::Time)
{
	if (!isConnected) {
		return;
	}
	// Send tick to server tcp
	sf::Packet packet;

	// Send control packet to move object on server
	packet << NetworkValues::CONTROL << mousePos.x << mousePos.y
		<< isForwardPressed << isAttackPressed << isBlockPressed;

	controlSocket.send(packet);
}
