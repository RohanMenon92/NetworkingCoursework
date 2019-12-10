#include "Server.hpp"

Server::Server(std::string ipHost)
	: mThread(&Server::ExecutionThread, this)
	, m_isRunning(true)
	, mWaitingThreadEnd(false)
	, m_clients()
	, renderGame()
	, m_gameTcpListener()
	, m_gameUdpSocket()
	, m_numberOfPlayers(0)
	, m_maximumPlayersCapacity(6)
{
}

Server::~Server()
{
	Shutdown();
	mWaitingThreadEnd = true;
	mThread.wait();
}

void Server::ExecutionThread()
{
	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(20.f / 60.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{
		HandleIncomingConnections();
		ReceiveInputFromSocket();

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
			ServerTick(tickInterval);
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void Server::ServerTick(sf::Time dt) {
	UpdateNumberOfPlayers();
	UpdateTimeoutPlayers(dt);
	SendUDPUpdate(dt);
}

void Server::Init()
{
	// Bind to the game ports.
	sf::Socket::Status listenStatus = m_gameTcpListener.listen(ServerConfiguration::GameTCPPort);

	if (listenStatus != sf::Socket::Done) {
		std::cout << "[GAME_SERVER] [ERROR] Lisener did not get done:" << listenStatus << std::endl;
		return;
	}
	m_gameTcpListener.setBlocking(false);

	// Bind to the Udp ports.
	m_gameUdpSocket.bind(ServerConfiguration::ServerUDPPort);
	m_gameUdpSocket.setBlocking(false);

	//Multithreading::outputMutex.lock();
	std::cout << "[GAME_SERVER] Has been inited!" << std::endl;
	//Multithreading::outputMutex.unlock();

	mThread.launch();

	renderGame.run();
}

bool Server::IsRunning() const {
	return m_isRunning;
}

void Server::HandleIncomingConnections()
{
	//std::cout << "[GAME_SERVER] In Handle incoming connections!" << std::endl;

	ClientRef* clientRef = new ClientRef();

	if (m_gameTcpListener.accept(clientRef->gameTcpSocket) == sf::TcpListener::Done)
	{
		std::cout << "[GAME_SERVER] In TCP Listener Accept! Remote Host:" << clientRef->gameTcpSocket.getRemoteAddress() << ":" << clientRef->gameTcpSocket.getRemotePort() << std::endl;

		clientRef->playerNumber = currPlayerNumber;
		clientRef->ingame = true;
		clientRef->gameTcpConnected = true;
		clientRef->gameTcpSocket.setBlocking(false);
		clientRef->ip = clientRef->gameTcpSocket.getRemoteAddress();
		sf::Vector2f spawnPosition = renderGame.OnSocketConnect(clientRef);

		sf::Packet packet;
		// Send spawn position, playerNumber and current server time for synronization
		packet << NetworkValues::CONNECT << 
			spawnPosition.x << spawnPosition.y << currPlayerNumber << serverTime;

		// Increment playerNumber
		currPlayerNumber++;

		std::cout << "[GAME_SERVER] Sent CONNECT packet " << clientRef->gameTcpSocket.getRemoteAddress() << std::endl;
		clientRef->gameTcpSocket.send(packet);
		m_clients.push_back(clientRef);
	} else {
		delete clientRef;
	}
}

void Server::ReceiveInputFromSocket()
{
	ReceiveThroughTCP();
	ReceiveThroughUDP();
	//ReceiveInputThroughUDP();
}

void Server::SendUDPUpdateToClient(ClientRef* client) {
	unsigned long long packetId = client->lastPacketIdSent;

	client->lastPacketIdSent++;
	for (Player* player : renderGame.players) {
		//if DeadSignal sent enough times
		if (player->isDeadSentCount > 0) {

			sf::Packet packet;

			if (player->isDead) {
				// Iterate through timeout and send dead signal
				player->isDeadSentCount--;
			}

			//std::cout << "[SEND DATA] Sending out PLAYER " << player->playerID << " packet:" << client->lastPacketIdSent << std::endl;

			PlayerMessage playerMessage;
			playerMessage.playerNumber = player->playerNumber;
			playerMessage.playerID = player->playerID;
			playerMessage.position = player->shape.getPosition();
			playerMessage.aimAt = player->aimAt;
			playerMessage.health = player->health;
			playerMessage.isAttacking = player->isAttacking;
			playerMessage.isBlocking = player->isBlocking;
			playerMessage.time = serverTime - client->latencyTime;
			playerMessage.isDead = player->isDead;

			packet << NetworkValues::RENDER_PLAYER << packetId
				<< playerMessage.playerNumber
				<< playerMessage.playerID
				<< playerMessage.position.x << playerMessage.position.y
				<< playerMessage.aimAt.x << playerMessage.aimAt.y
				<< playerMessage.health
				<< playerMessage.isAttacking
				<< playerMessage.isBlocking
				<< playerMessage.time
				<< playerMessage.isDead;

			//client->gameTcpSocket.send(packet);
			m_gameUdpSocket.send(packet, client->ip, client->udpPort);
			packet.clear();
		}
	}

	for (Bullet* bullet : renderGame.bullets) {
		// If DeadSignal sent enough times
		if (bullet->isDeadSentCount > 0) {

			sf::Packet packet;

			if (bullet->isDead) {
				// Iterate through timeout and send dead signal
				bullet->isDeadSentCount--;
			}

			//std::cout << "[SEND DATA] Sending out bullet " << bullet->bulletID << "  packetID:" << client->lastPacketIdSent << std::endl;

			BulletMessage bulletMessage;
			bulletMessage.playerNumber = bullet->playerNumber;
			bulletMessage.bulletID = bullet->bulletID;
			bulletMessage.isDead = bullet->isDead;
			bulletMessage.position = bullet->shape.getPosition();
			bulletMessage.time = serverTime - client->latencyTime;

			packet << NetworkValues::RENDER_BULLET << packetId
				<< bulletMessage.playerNumber
				<< bulletMessage.bulletID
				<< bulletMessage.position.x << bulletMessage.position.y
				<< bulletMessage.time
				<< bulletMessage.isDead;

			//client->gameTcpSocket.send(packet);
			m_gameUdpSocket.send(packet, client->ip, client->udpPort);
			packet.clear();
		}
	}
}

void Server::SendUDPUpdate(sf::Time dt) {
	// update gameTime
	serverTime += dt.asSeconds();

	// Iterate and send update
	for (ClientRef* clientRef : m_clients)
	{
		if (clientRef->ingame)
		{
			SendUDPUpdateToClient(clientRef);
		}
	}
}

unsigned int Server::GetNumberOfPlayers() const
{
	return m_numberOfPlayers;
}

unsigned int Server::GetMaximumPlayersCapacity() const
{
	return m_maximumPlayersCapacity;
}

std::vector<ClientRef*>* Server::GetClients()
{
	return &m_clients;
}

void Server::Shutdown()
{
	//Multithreading::outputMutex.lock();
	std::cerr << "[GAME_SERVER] Disconnecting everybody..." << std::endl;
	std::cerr << "[GAME_SERVER] Stopping server..." << std::endl;
	//Multithreading::outputMutex.unlock();

	// Disconnect everyone.
	for (ClientRef* client : m_clients)
	{
		DisconnectPlayer(client, "Server closed");
	}
	m_isRunning = false;

	//Multithreading::outputMutex.lock();
	std::cerr << "[GAME_SERVER] Server stopped." << std::endl;
	//Multithreading::outputMutex.unlock();
}

void Server::DisconnectPlayer(ClientRef* clientRef, std::string reason)
{
	try
	{
		auto clientItr = std::find_if(m_clients.begin(), m_clients.end(), [clientRef](ClientRef* client) {
			return client == clientRef;
		});

		clientRef->ingame = false;
		clientRef->playerBox->isDead = true;

		//// Erase the client.
		delete clientRef;
		m_clients.erase(clientItr);
	}
	catch (std::exception& e)
	{
		//Multithreading::outputMutex.lock();
		std::cerr << "[GAME_SERVER][ERROR] " << e.what() << std::endl;
		//Multithreading::outputMutex.unlock();
	}
}

void Server::UpdateNumberOfPlayers()
{
	// count through number of players
	unsigned int numberOfPlayers(0);

	for (ClientRef* client : m_clients)
	{
			numberOfPlayers++;
	};

	m_numberOfPlayers = numberOfPlayers;
}

void Server::UpdateTimeoutPlayers(sf::Time dt)
{
	// Check each client connected to the game server.
	for (auto clientItr(m_clients.begin()); clientItr != m_clients.end();)
	{
		// Alias.
		ClientRef* client = (*clientItr);

		// Timeout only if in game.
		if (client->ingame)
		{
			// Update timer.
			client->timeout += dt;

			// Check the timeout.
			if (client->timeout >= ServerConfiguration::GameServerTimeout)
			{
				DisconnectPlayer(client, "timed out");
				continue;
			}
			else
			{
				clientItr++;
			}
		}
		else
		{
			clientItr++;
		}
	}
}

void Server::ReceiveThroughTCP()
{
	//std::cout << "[GAME_SERVER] recieve input through tcp " << std::endl;

	// Check if we received any TCP packet.
	for (auto clientItr(m_clients.begin()); clientItr != m_clients.end();)
	{
		ClientRef* client = (*clientItr);
		Player* playerBox = client->playerBox;

		// Timeout only if in game.
		//std::cout << "[GAME_SERVER] IS CLIENT IN GAME " << client->clientName << std::endl;
		if (client->ingame)
		{
			bool deletedClient(false);

			// Check if we received a packet.
			//std::cout << "[GAME_SERVER] CLIENT IS IN GAME RECIEVING " << client->clientName << std::endl;

			sf::Packet packet;

			while (client->gameTcpSocket.receive(packet) == sf::TcpSocket::Status::Done)
			{
				// Reset timeout.
				client->timeout = sf::Time::Zero;

				// Extract network code.
				unsigned int networkCode;
				packet >> networkCode;

				switch (networkCode)
				{
				case NetworkValues::RETURN_CONNECT:
				{
					std::string username;
					unsigned short udpPort;
					float returnedServerTime;
					packet >> username >> udpPort >> returnedServerTime;

					//bool usernameExists;
					//for (auto usernameItr(m_clients.begin()); usernameItr != m_clients.end();)
					//{
					//	ClientRef* uClient = (*usernameItr);

					//	if (uClient->clientName == username) {
					//		usernameExists = true;
					//	}
					//}

					//// Store username as new if it already exists
					//username = username + (usernameExists ? "New" : "");

					client->clientName = username;
					// Store udpPort for client and latency based on roundtrip communication
					client->udpPort = udpPort;
					client->latencyTime = (serverTime - returnedServerTime)/2;
					renderGame.OnPlayerConnect(username, playerBox);
					// Make connection to udp non blocking
					client->gameUdpSocket.setBlocking(false);
				}
				break;
				case NetworkValues::DISCONNECT:
				{
					// Notify everyone the player disconnected.
					NotifyPlayerDisconnected(client->clientName);

					// Disconnect the player.
					DisconnectPlayer(client, "disconnected");

					// Tag we deleted a client.
					deletedClient = true;
					break;
				}
				break;
				case NetworkValues::CONTROL:
				{
					if (playerBox != NULL) {
						// Get Packet Data here, mose position and key pressed
						ControlMessage controlMessage;

						packet >> controlMessage.mousePos.x >> controlMessage.mousePos.y
							>> controlMessage.isForwardPressed
							>> controlMessage.isAttackPressed
							>> controlMessage.isBlockPressed >> 
							controlMessage.atTime;

						//std::cout << "[SERVER] Got [CONTROL] Signal" 
						//	<< " MousePos::" << mousePositionX << " " << mousePositionY
						//	<< " KeyValues::" << isForwardPressed << isAttackPressed << isBlockPressed
						//	<< std::endl;

						// Update client Latency here as well? How? Using atTime?

						// Set looking at position and interaction properties
						playerBox->isMovingForward = controlMessage.isForwardPressed;
						playerBox->isAttacking = controlMessage.isAttackPressed;
						playerBox->isBlocking = controlMessage.isBlockPressed;
						playerBox->SetAimPos(controlMessage.mousePos);
					}
				}
				break;
				default:
					break;
				}
			}
			// Do not increment if we deleted a client previously.
			if (!deletedClient) {
				clientItr++;
			}
		}
		else
		{
			clientItr++;
		}
	}
}

void Server::ReceiveThroughUDP()
{
	// NOTHING HERE AS OF NOW

	// Check if we received any UDP packet.
	//sf::Packet packet;
	//sf::IpAddress ip;
	//short unsigned int port;

	//while (m_gameUdpSocket.receive(packet, ip, port) == sf::UdpSocket::Status::Done)
	//{
		//// Treat the packet.
		//// Extract the network code.
		//unsigned int networkCode;
		//std::string username;
		//packet >> networkCode >> username;

		//bool clientFound;
		//// Alias.
		//ClientRef* client;
		//
		//// Check if user has connected
		//for (auto clientItr(m_clients.begin()); clientItr != m_clients.end();)
		//{
		//	ClientRef* udpClient = (*clientItr);

		//	// Make sure client had connected on TCP
		//	if (udpClient->clientName == username && udpClient->ip == ip) {
		//		client = udpClient;
		//		clientFound = true;
		//	}
		//}

		//if (clientFound) {
		//PlayerBox* playerBox = client->playerBox;

		//// Evaluate the network code.
		//switch (networkCode)
		//{
		//	case NetworkValues::CONNECT:
		//	{

		//	}
		//	break;
		//	default:
		//		break;
		//	}
		//}
		//// Clear the packet.
		//packet.clear();
	//}
}

void Server::NotifyPlayerConnected(std::string username)
{
	// Prepare the packet.
	sf::Packet packet;
	packet << NetworkValues::NOTIFY << NetworkValues::PLAYER_CONNECTED << username;

	// Send the notification to all the in-game clients.
	for (ClientRef* client : m_clients)
	{
		if (client->ingame)
			client->gameTcpSocket.send(packet);
	}
}

void Server::NotifyPlayerDisconnected(std::string username)
{
	// Prepare the packet.
	sf::Packet packet;
	packet << NetworkValues::NOTIFY << NetworkValues::PLAYER_DISCONNECTED << username;

	// Send the notification to all the in-game clients.
	for (ClientRef* client : m_clients)
	{
		if (client->ingame)
			client->gameTcpSocket.send(packet);
	}
}