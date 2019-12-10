#include "Client.hpp"

Client::Client(std::string newUsername)
	: isConnected(false)
	, dontExecuteClient(false)
	, controlTCPSocket()
	, renderUDPSocket()
	, udpPacketNumberReceive(0)
	, lossPacket(0)
	, timeOutTimer(sf::Time::Zero)
	, players()
	, bullets()
	, username(newUsername)
{
	fontText.loadFromFile("Antonio.ttf");

	game.reset(new Game());
	
	// Bind rendering udp socket to any port, 
	// TCP will notify server of udp port to store it 
	// if needed to listen on server
	renderUDPSocket.bind(sf::Socket::AnyPort);
	renderUDPSocket.setBlocking(false);

	// Initially blocking
	controlTCPSocket.setBlocking(true);

	ConnectToServer();
	SetupClient();
	//mThread.launch();
}

Client::~Client()
{
	// Send Disconnection to Server
	sf::Packet packet;
	packet << NetworkValues::DISCONNECT << username;

	controlTCPSocket.send(packet);

	dontExecuteClient = true;
}

void Client::Init()
{
}

void Client::InitGame(sf::Vector2f startPos, float startTime)
{
	// Bind UDP Socket
	isConnected = true;
	controlTCPSocket.setBlocking(false);

	// Create game elements and start receving control
	game->initWindow();
	game->gameTime = startTime;
}

void Client::ConnectToServer()
{
	// Connect to TCP
	sf::TcpSocket::Status status = controlTCPSocket.connect(serverIP, ServerConfiguration::GameTCPPort, sf::seconds(5.f));

	if (status != sf::Socket::Done)
	{
		std::cout << "[GAME_CLIENT] WAS UNABLE TO CONNECT TO SERVER " << status << std::endl;
	}

	// Wait for initial connect packet

	sf::Packet connectPacket;
	// Listen and recieve generated position
	if (controlTCPSocket.receive(connectPacket) == sf::TcpSocket::Status::Done) {

		unsigned int netCode(0);
		connectPacket >> netCode;

		if (netCode != NetworkValues::CONNECT) {
			std::cout << "[GAME_CLIENT] [ERROR] Did not recieve Connect packet! " << std::endl;
			return;
		}

		// On Connect packet, extract server defined spawn position and player number
		sf::Vector2f startPos;
		float serverTime;
		connectPacket >> startPos.x >> startPos.y >> this->playerNumber >> serverTime;

		std::cout << "[GAME_CLIENT] Recieve Connect packet! Sending username " << std::endl;

		sf::Packet packet;
		packet << NetworkValues::CONNECT << username << renderUDPSocket.getLocalPort() << serverTime;
		controlTCPSocket.send(packet);

		// Start the game based on server set positions and initial serverTime
		InitGame(startPos, serverTime);
	}
}

void Client::SetupClient()
{
	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 60.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!dontExecuteClient)
	{
		if (game->window->isOpen())
		{
			game->UpdateSFMLEvents();
			game->ProcessInput();
			if (isConnected) {
				sf::Time dt = dtClock.restart();
				// Increment game time
				game->Update(dt, players, bullets);
				game->Render(players, bullets);
			}
		}

		ReceiveInput();
		if (isConnected) {
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
		//sf::sleep(sf::milliseconds(100));
	}
}

void Client::ReceiveInput()
{
	if (isConnected) {
		//std::cout << "[GAME_CLIENT] Receiving Input " << std::endl;
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
	unsigned short int remotePort(ServerConfiguration::ServerUDPPort);

	if (renderUDPSocket.receive(packet, ip, remotePort) == sf::Socket::Status::Done)
	{
		timeOutTimer = sf::Time::Zero;
		unsigned int netCode(0);
		packet >> netCode;

		unsigned long long number;
		packet >> number;
		//std::cout << "[Client RECIEVE UDP SOCKET] Number received : " << number << " Number Already received:" << udpPacketNumberReceive << std::endl;

		if (number >= udpPacketNumberReceive) {
			udpPacketNumberReceive = number;
			switch (netCode)
			{
			case NetworkValues::RENDER_BULLET:
			{
				//std::cout << "[Client RECIEVE UDP SOCKET] RENDER BULLET FROM UDP " << std::endl;

				//if (number - udpPacketNumberReceive > 1)
				//{
				//	lossPacket += (number - 1) - udpPacketNumberReceive;
				//	std::cout << "[PACKET_LOSS] " << lossPacket << " / " << udpPacketNumberReceive
				//		<< " (" << (lossPacket * 100.f) / (float)(udpPacketNumberReceive) << "%)" << std::endl;
				//}

				BulletMessage incomingBulletMessage;

				packet >> incomingBulletMessage.playerNumber
					>> incomingBulletMessage.bulletID 
					>> incomingBulletMessage.position.x >> incomingBulletMessage.position.y 
					>> incomingBulletMessage.time
					>> incomingBulletMessage.isDead;

				//std::cout << " [CLIENT GAME UPDATE BULLET ] " << incomingBulletMessage.bulletID << std::endl;
				UpdateBullet(incomingBulletMessage);
			}
			break;
			case NetworkValues::RENDER_PLAYER: 
			{

				//std::cout << "[Client RECIEVE UDP SOCKET] RENDER PLAYER FROM UDP " << std::endl;

				//if (number - udpPacketNumberReceive > 1)
				//{
				//	lossPacket += (number - 1) - udpPacketNumberReceive;
				//	std::cout << "[PACKET_LOSS] " << lossPacket << " / " << udpPacketNumberReceive
				//		<< " (" << (lossPacket * 100.f) / (float)(udpPacketNumberReceive) << "%)" << std::endl;
				//}

				PlayerMessage incomingPlayerMessage;

				packet >> incomingPlayerMessage.playerNumber 
					>> incomingPlayerMessage.playerID
					>> incomingPlayerMessage.position.x >> incomingPlayerMessage.position.y
					>> incomingPlayerMessage.aimAt.x >> incomingPlayerMessage.aimAt.y
					>> incomingPlayerMessage.health
					>> incomingPlayerMessage.isAttacking
					>> incomingPlayerMessage.isBlocking
					>> incomingPlayerMessage.time
					>> incomingPlayerMessage.isDead;

				UpdatePlayer(incomingPlayerMessage);
			}

			break;
			}
		}
	}
}

void Client::UpdateBullet(BulletMessage bulletMessage)
{
	if (bullets.count(bulletMessage.bulletID) == 0) {
		SpawnBullet(bulletMessage);
	} else {
		Bullet* bullet = bullets.find(bulletMessage.bulletID)->second;
		bullet->playerNumber = bullet->playerNumber;
		bullet->AddMessage(bulletMessage);		
		bullet->isDead = bulletMessage.isDead;
	}
}

void Client::UpdatePlayer(PlayerMessage playerMessage)
{
	//std::cout << "[CLIENT UPDATE PLAYER] ABOUT TO ITERATE " << std::endl;

	if (players.count(playerMessage.playerID) == 0) {
		SpawnPlayer(playerMessage);
	}
	else {
		Player* player = players.find(playerMessage.playerID)->second;
		player->playerNumber = playerMessage.playerNumber;
		player->AddMessage(playerMessage);
		player->SetClientLookTo(playerMessage.aimAt);
		player->isAttacking = playerMessage.isAttacking;
		player->isBlocking = playerMessage.isBlocking;
		player->health = playerMessage.health;
		player->isDead = playerMessage.isDead;
		player->usernameHolder.setFont(fontText);
		player->healthHolder.setFont(fontText);
	}
}

void Client::SpawnPlayer(PlayerMessage playerMessage)
{
	Player* player = new Player(playerMessage.playerNumber);

	player->playerID = playerMessage.playerID;
	player->shape.setPosition(playerMessage.position);
	player->AddMessage(playerMessage);
	player->health = playerMessage.health;
	player->isAttacking = playerMessage.isAttacking;
	player->isBlocking = playerMessage.isBlocking;
	player->isDead = playerMessage.isDead;

	players.insert(std::make_pair(playerMessage.playerID, player));
}

void Client::SpawnBullet(BulletMessage bulletMessage)
{
	Bullet* bullet = new Bullet(bulletMessage.playerNumber);
	bullet->playerNumber = bulletMessage.playerNumber;
	bullet->bulletID = bulletMessage.bulletID;
	bullet->shape.setPosition(bulletMessage.position);
	bullet->AddMessage(bulletMessage);
	bullet->isDead = bulletMessage.isDead;

	bullets.insert(std::make_pair(bulletMessage.bulletID, bullet));
}


void Client::ClientTick(sf::Time)
{
	//std::cout << "IN Client Tick" << std::endl;
	if (!isConnected) {
		return;
	}
	// Send tick to server tcp
	sf::Packet packet;

	ControlMessage controlMessage;

	controlMessage.mousePos = mousePos;
	controlMessage.isForwardPressed = isForwardPressed;
	controlMessage.isAttackPressed = isAttackPressed;
	controlMessage.isBlockPressed = isBlockPressed;
	controlMessage.atTime = game->gameTime;

	// Send control packet to move object on server
	packet << NetworkValues::CONTROL << controlMessage.mousePos.x << controlMessage.mousePos.y
		<< controlMessage.isForwardPressed 
		<< controlMessage.isAttackPressed 
		<< controlMessage.isBlockPressed 
		<< controlMessage.atTime;

	// Game time can be used to look at histories for further checks

	controlTCPSocket.send(packet);
}
