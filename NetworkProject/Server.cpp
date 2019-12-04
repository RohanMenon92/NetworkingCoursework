#include "Server.hpp"

Server::Server()
	: mThread(&Server::ExecutionThread, this)
	, m_isRunning(true)
	, mWaitingThreadEnd(false)
	, m_clients()
	, renderGame()
	, m_numberOfPlayers(0)
	, m_maximumPlayersCapacity(4)
{
	m_gameTcpListener.setBlocking(false);
	//renderGame = new RenderGame();
}

Server::~Server()
{
	mWaitingThreadEnd = true;
	mThread.wait();
	//for (auto accountEntry : m_accounts)
	//	delete accountEntry.second;

	//for (World* world : m_worlds)
	//	delete world;
}

void Server::ExecutionThread()
{
	//setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
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
		sf::sleep(sf::milliseconds(200));
	}
}

void Server::ServerTick(sf::Time dt) {
	UpdateNumberOfPlayers();
	UpdateTimeoutPlayers(dt);
	SendUpdate();
}

void Server::Init()
{
	// Init the login server.
	//m_loginServer.init();

	// Bind to the game ports.
	sf::Socket::Status listenStatus = m_gameTcpListener.listen(ServerConfiguration::GameTCPPort);

	if (listenStatus != sf::Socket::Done) {
		std::cout << "[GAME_SERVER] [ERROR] Lisener did not get done:" << listenStatus << std::endl;
		return;
	}

	m_gameTcpListener.setBlocking(false);

	m_gameUdpSocket.bind(ServerConfiguration::GameUDPPort);
	m_gameUdpSocket.setBlocking(false);

	Multithreading::outputMutex.lock();
	std::cout << "[GAME_SERVER] Has been inited 2!" << std::endl;
	Multithreading::outputMutex.unlock();

	mThread.launch();

	renderGame.run();
}

bool Server::IsRunning() const {
	return m_isRunning;
}

void Server::HandleIncomingConnections()
{
	ClientRef* clientRef = new ClientRef();

	std::cout << "[GAME_SERVER] In Handle incoming connections!" << std::endl;

	if (m_gameTcpListener.accept(clientRef->gameTcpSocket) == sf::TcpListener::Done)
	{
		sf::Vector2f spawnPosition = renderGame.OnSocketConnect(clientRef);

		std::cout << "[GAME_SERVER] In TCP Listener Accept! ip:" << clientRef->gameTcpSocket.getRemoteAddress() << std::endl;

		clientRef->ingame = true;
		clientRef->ip = clientRef->gameTcpSocket.getRemoteAddress();
		clientRef->udpPort = ServerConfiguration::GameUDPPort;

		sf::Packet packet;
		packet << NetworkValues::CONNECT;
		packet << spawnPosition.x;
		packet << spawnPosition.y;
		clientRef->gameTcpSocket.send(packet);
	}
}

//void Server::login(sf::Time dt) {
//	PendingSocket* pendingSocket = new PendingSocket();
//	pendingSocket->tcpSocket = std::make_shared<sf::TcpSocket>();
//	pendingSocket->tcpSocket->setBlocking(false);
//
//	if (m_gameTcpListener.accept(*pendingSocket->tcpSocket.get()) == sf::TcpListener::Status::Done)
//	{
//		// We still don't know to who belongs this socket.
//		// We need to wait for the CONNECT packet.
//		m_pendingTcpSockets.push_back(pendingSocket);
//
//		Multithreading::outputMutex.lock();
//		std::cout << "[GAME_SERVER] New pending socket from (" << pendingSocket->tcpSocket->getRemoteAddress().toString() << ")." << std::endl;
//		Multithreading::outputMutex.unlock();
//	}
//	else
//	{
//		delete pendingSocket;
//	}
//
//	// Check if we received any CONNECT packet on the pending sockets and update their timeout.
//	for (auto pendingSocketItr(m_pendingTcpSockets.begin()); pendingSocketItr != m_pendingTcpSockets.end();)
//	{
//		PendingSocket* pendingSocket = (*pendingSocketItr);
//		pendingSocket->timeout += dt;
//
//		// Check timeout for pending socket.
//		if (pendingSocket->timeout >= ServerConfiguration::PendingSocketTimeout)
//		{
//			Multithreading::outputMutex.lock();
//			std::cout << "[GAME_SERVER] Pending socket from (" << pendingSocket->tcpSocket->getRemoteAddress().toString() << ") timed out." << std::endl;
//			Multithreading::outputMutex.unlock();
//
//			delete pendingSocket;
//			m_pendingTcpSockets.erase(pendingSocketItr);
//			continue;
//		}
//
//		// Check if received packet.
//		sf::Packet packet;
//		if (pendingSocket->tcpSocket->receive(packet) == sf::TcpSocket::Status::Done)
//		{
//			unsigned int networkCode(0);
//			packet >> networkCode;
//
//			if (networkCode != NetworkValues::CONNECT)
//			{
//				Multithreading::outputMutex.lock();
//				std::cout << "[GAME_SERVER] Unknown network code : '" << networkCode << "'." << std::endl;
//				Multithreading::outputMutex.unlock();
//				continue;
//			}
//
//			// Extract username and random token.
//			std::string username(""), token("");
//			packet >> username >> token;
//
//			// Check if username exists.
//			//if (m_accounts.find(username) == m_accounts.end())
//			//{
//			//	// CONNECTION_FAIL_UNKNOWN_USER
//			//	sf::Packet answer;
//			//	answer << NetworkValues::CONNECTION_FAIL_UNKNOWN_USER;
//			//	pendingSocket->tcpSocket->send(answer);
//
//			//	Multithreading::outputMutex.lock();
//			//	std::cout << "[GAME_SERVER] CONNECTION_FAIL_UNKNOWN_USER from (" << pendingSocket->tcpSocket->getRemoteAddress().toString() << ")." << std::endl;
//			//	Multithreading::outputMutex.unlock();
//
//			//	continue;
//			//}
//
//			//// Check if the username and random token given are the same as ours.
//			//if (token == m_accounts.at(username)->token)
//			//{
//			//	// OKAY
//			//	sf::Packet answer;
//			//	answer << NetworkValues::OKAY;
//			//	pendingSocket->tcpSocket->send(answer);
//
//			//	// Affect the tcp socket to the client.
//			//	m_accounts.at(username)->linkedClient->gameTcp = pendingSocket->tcpSocket;
//			//	m_accounts.at(username)->linkedClient->gameTcpConnected = true;
//
//			//	// Reset timeout.
//			//	m_accounts.at(username)->linkedClient->timeout = sf::Time::Zero;
//
//			//	Multithreading::outputMutex.lock();
//			//	std::cout << "[GAME_SERVER] Game TCP connected from (" << pendingSocket->tcpSocket->getRemoteAddress().toString() << ") for '" << username << "'." << std::endl;
//			//	Multithreading::outputMutex.unlock();
//
//			//	// Delete the pending socket.
//			//	delete pendingSocket;
//			//	m_pendingTcpSockets.erase(pendingSocketItr);
//			//	continue;
//			//}
//			//else
//			//{
//			//	// CONNECTION_FAIL_WRONG_TOKEN
//			//	sf::Packet answer;
//			//	answer << NetworkValues::CONNECTION_FAIL_WRONG_TOKEN;
//			//	pendingSocket->tcpSocket->send(answer);
//
//			//	Multithreading::outputMutex.lock();
//			//	std::cout << "[GAME_SERVER] CONNECTION_FAIL_WRONG_TOKEN from (" << pendingSocket->tcpSocket->getRemoteAddress().toString() << ")." << std::endl;
//			//	Multithreading::outputMutex.unlock();
//			//}
//		}
//
//		// Next pending socket.
//		pendingSocketItr++;
//	}
//}

void Server::ReceiveInputFromSocket()
{
	ReceiveInputThroughTCP();
	//ReceiveInputThroughUDP();
}

void Server::SendUDPUpdateToClient(ClientRef* client, sf::UdpSocket& socket) {
	//  SEND UDP UPDATE TO CLIENT
}

void Server::SendUpdate() {
	for (ClientRef* clientRef : m_clients)
	{
		if (clientRef->ingame)
		{
			// Multithreading worlds.
			// Can't multithread network stuff.
			SendUDPUpdateToClient(clientRef, m_gameUdpSocket);
			//for (World* world : m_worlds)
			//	world->sendUpdate(client, m_gameUdpSocket);
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
	Multithreading::outputMutex.lock();
	std::cerr << "[GAME_SERVER] Disconnecting everybody..." << std::endl;
	std::cerr << "[GAME_SERVER] Stopping server..." << std::endl;
	Multithreading::outputMutex.unlock();

	//// Disconnect everyone.
	//while (itr != m_accounts.end())
	//{
	//	if (itr->second->linkedClient->ingame)
	//	{
	//		disconnectPlayer(itr->first, "Server closed");
	//	}
	//}

	m_isRunning = false;

	Multithreading::outputMutex.lock();
	std::cerr << "[GAME_SERVER] Server stopped." << std::endl;
	Multithreading::outputMutex.unlock();
}

void Server::DisconnectPlayer(ClientRef* clientRef, std::string reason)
{

	// Dissociate the account and the client.
	try
	{
		auto clientItr = std::find_if(m_clients.begin(), m_clients.end(), [clientRef](ClientRef* client) {
			return client == clientRef;
		});

		clientRef->ingame = false;

		//// Erase the client.
		delete clientRef;
		m_clients.erase(clientItr);
	}
	catch (std::exception& e)
	{
		Multithreading::outputMutex.lock();
		std::cerr << "[GAME_SERVER][ERROR] " << e.what() << std::endl;
		Multithreading::outputMutex.unlock();
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

void Server::ReceiveInputThroughTCP()
{
	// Check if we received any TCP packet.
	for (auto clientItr(m_clients.begin()); clientItr != m_clients.end();)
	{
		// Alias.
		ClientRef* client = (*clientItr);
		PlayerBox* playerBox = client->playerBox;

		// Timeout only if in game.
		if (client->ingame)
		{
			bool deletedClient(false);

			// Check if we received a packet.
			sf::Packet packet;

			while (client->gameTcpPtr->receive(packet) == sf::TcpSocket::Status::Done)
			{
				// Reset timeout.
				client->timeout = sf::Time::Zero;

				// Extract network code.
				unsigned int networkCode;
				packet >> networkCode;

				switch (networkCode)
				{
				case NetworkValues::CONNECT:
				{
					std::string username;
					packet >> username;

					client->clientName = username;

					renderGame.OnPlayerConnect(username, client->playerBox);
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
					// Get Packet Data here, mose position and key pressed
					std::string username;
					float mousePositionX;
					float mousePositionY;
					bool isForwardPressed;
					bool isAttackPressed;
					bool isBlockPressed;

					packet >> username >> mousePositionX >> mousePositionY >> isForwardPressed >> isAttackPressed >> isBlockPressed;

					client->clientName = username;

					if (!client->isAttackPressed && isAttackPressed) {
						// Call On Attack Pressed
						playerBox->OnAttackPressed();
					}
					else if (client->isAttackPressed && !isAttackPressed) {
						// Call On Attack Unpressed
						playerBox->OnAttackUnpressed();
					}

					if (!client->isBlockPressed && isBlockPressed) {
						// Call On Attack Pressed
						playerBox->OnBlockPressed();
					}
					else if (client->isBlockPressed && !isBlockPressed) {
						// Call On Attack Unpressed
						playerBox->OnBlockUnpressed();
					}

					if (!client->isForwardPressed && isForwardPressed) {
						// Call On Attack Pressed
						playerBox->OnForwardPressed();
					}
					else if (client->isForwardPressed && !isForwardPressed) {
						// Call On Attack Unpressed
						playerBox->OnForwardUnpressed();
					}
					
					playerBox->aimAt = {mousePositionX, mousePositionY};
				}
				break;
				default:
					break;
				}
			}

			// Do not increment if we deleted a client previously.
			if (!deletedClient)
				clientItr++;
		}
		else
		{
			clientItr++;
		}
	}
}

//void Server::ReceiveInputThroughUDP()
//{
	//// Check if we received any UDP packet.
	//sf::Packet packet;
	//sf::IpAddress ip;
	//short unsigned int port;

	//while (m_gameUdpSocket.receive(packet, ip, port) == sf::UdpSocket::Status::Done)
	//{
	//	// Treat the packet.
	//	// Extract the network code.
	//	unsigned int networkCode;
	//	packet >> networkCode;

	//	// Evaluate the network code.
	//	switch (networkCode)
	//	{
	//	case NetworkValues::CONNECT:
	//	{
	//		//// Extract username and token.
	//		//std::string username(""), token("");
	//		//packet >> username >> token;

	//		//// Check if username exists.
	//		//if (m_accounts.find(username) != m_accounts.end())
	//		//{
	//		//	// Ignore packet if client not connected.
	//		//	if (m_accounts.at(username)->linkedClient == nullptr)
	//		//		continue;

	//		//	// Ignore packet if already in game.
	//		//	if (m_accounts.at(username)->linkedClient->ingame)
	//		//		continue;

	//		//	// Check if the username and token given are the same as ours.
	//		//	if (token == m_accounts.at(username)->token)
	//		//	{
	//		//		// OKAY
	//		//		sf::Packet answer;
	//		//		answer << NetworkValues::OKAY;
	//		//		m_accounts.at(username)->linkedClient->gameTcp->send(answer);

	//		//		// Affect the tcp socket to the client.
	//		//		m_accounts.at(username)->linkedClient->ip = ip;
	//		//		m_accounts.at(username)->linkedClient->udpPort = port;
	//		//		m_accounts.at(username)->linkedClient->gameUdpConnected = true;
	//		//		m_accounts.at(username)->linkedClient->ingame = true;

	//		//		// Reset timeout.
	//		//		m_accounts.at(username)->linkedClient->timeout = sf::Time::Zero;

	//		//		// Replie with the list of the players.
	//		//		answer.clear();

	//		//		answer << getNumberOfPlayers();
	//		//		for (auto accountEntry : m_accounts)
	//		//		{
	//		//			if (accountEntry.second->linkedClient != nullptr)
	//		//				answer << accountEntry.second->linkedClient->username;
	//		//		}

	//		//		m_accounts.at(username)->linkedClient->gameTcp->send(answer);

	//		//		Multithreading::outputMutex.lock();
	//		//		std::cout << "[GAME_SERVER] Game UDP connected from (" << ip.toString() << ") for '" << username << "'." << std::endl;
	//		//		std::cout << "[GAME_SERVER] '" << username << "' from (" << ip.toString() << ") is now in game !" << std::endl;
	//		//		Multithreading::outputMutex.unlock();

	//		//		// Notify everyone the player connected.
	//		//		notifyPlayerConnected(username);

	//		//		// Select a world to put the player in.
	//		//		/// /!\ TODO ?

	//		//		// Notify the world.
	//		//		m_worlds[0]->playerConnected(m_accounts.at(username)->linkedClient, this);

	//		//		// Change the client's data.
	//		//		m_accounts.at(username)->linkedClient->currentWorld = m_worlds[0]->getId();

	//		//		// Notify player in witch world he has been transferred.
	//		//		answer.clear();
	//		//		answer << NetworkValues::NOTIFY << NetworkValues::PLAYER_MOVED_TO_WORLD << m_worlds[0]->getId() << m_worlds[0]->getMapId();
	//		//		m_accounts.at(username)->linkedClient->gameTcp->send(answer);

	//		//		Multithreading::outputMutex.lock();
	//		//		std::cout << "[GAME_SERVER] '" << username << "' has been transferred to world #" << m_accounts.at(username)->linkedClient->currentWorld << "." << std::endl;
	//		//		Multithreading::outputMutex.unlock();
	//		//	}
	//		//}
	//	}
	//	break;
	//	case NetworkValues::INPUT:
	//	{
	//		//// Extract username and token.
	//		//std::string username(""), token("");
	//		//packet >> username >> token;

	//		//// Check if username exists.
	//		//if (m_accounts.find(username) != m_accounts.end())
	//		//{
	//		//	// Ignore packet if client not connected.
	//		//	if (m_accounts.at(username)->linkedClient == nullptr)
	//		//		continue;

	//		//	// Ignore packet if not already in game.
	//		//	if (!m_accounts.at(username)->linkedClient->ingame)
	//		//		continue;

	//		//	// Check if the username and token given are the same as ours.
	//		//	if (token == m_accounts.at(username)->token)
	//		//	{
	//		//		// Alias.
	//		//		Client* client = m_accounts.at(username)->linkedClient;

	//		//		// Reset timeout.
	//		//		m_accounts.at(username)->linkedClient->timeout = sf::Time::Zero;

	//		//		// Extract udp packet id.
	//		//		unsigned long long udpPacketId(0);
	//		//		packet >> udpPacketId;

	//		//		// Skip packet if we already received newer inputs.
	//		//		if (udpPacketId < client->lastPacketIdReceived)
	//		//			break;

	//		//		// Count how many packets were lost.
	//		//		if (udpPacketId - client->lastPacketIdReceived > 1)
	//		//		{
	//		//			client->lostPackets += (udpPacketId - 1) - client->lastPacketIdReceived;
	//		//			Multithreading::outputMutex.lock();
	//		//			std::cout << "[PACKET_LOSS] " << client->username << ": " << client->lostPackets << " / " << client->lastPacketIdReceived
	//		//				<< " (" << (client->lostPackets * 100.f) / (float)(client->lastPacketIdReceived) << "%)" << std::endl;
	//		//			Multithreading::outputMutex.unlock();
	//		//		}

	//		//		// Update the last packet id.
	//		//		client->lastPacketIdReceived = udpPacketId;

	//		//		// Extract client input.
	//		//		packet >> client->inputs.isMoveUpKeyPressed
	//		//			>> client->inputs.isMoveDownKeyPressed
	//		//			>> client->inputs.isMoveLeftKeyPressed
	//		//			>> client->inputs.isMoveRightKeyPressed
	//		//			>> client->inputs.mouseX
	//		//			>> client->inputs.mouseY
	//		//			>> client->inputs.isAAttackKeyPressed
	//		//			>> client->inputs.isEAttackKeyPressed
	//		//			>> client->inputs.isMouseLeftButtonPressed
	//		//			>> client->inputs.isMouseRightButtonPressed;
	//		//	}
	//		//}
	//	}
	//	break;
	//	default:
	//		break;
	//	}

	//	// Clear the packet.
	//	packet.clear();
	//}
//}

void Server::NotifyPlayerConnected(std::string username)
{
	// Prepare the packet.
	sf::Packet packet;
	packet << NetworkValues::NOTIFY << NetworkValues::PLAYER_CONNECTED << username;

	// Send the notification to all the in-game clients.
	for (ClientRef* client : m_clients)
	{
		if (client->ingame)
			client->gameTcpPtr->send(packet);
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
			client->gameTcpPtr->send(packet);
	}
}

//void Server::switchClientToWorld(Client* client, int worldId)
//{
//	// The client needs to be in game.
//	if (client->ingame)
//	{
//		// In which world is it currently ?
//		int currentWorldId = client->currentWorld;
//
//		if (currentWorldId != -1)
//		{
//			// Find the current world and notify it the player is leaving.
//			auto worldItr = std::find_if(m_worlds.begin(), m_worlds.end(), [&](const World* world) {
//				return world->getId() == currentWorldId;
//			});
//
//			if (worldItr != m_worlds.end())
//				(*worldItr)->playerDisconnected(client, this);
//		}
//
//		// Switch the player in the new world.
//		// Check the destination world is valid.
//		auto worldItr = std::find_if(m_worlds.begin(), m_worlds.end(), [&](const World* world) {
//			return world->getId() == worldId;
//		});
//
//		if (worldItr != m_worlds.end())
//		{
//			(*worldItr)->playerConnected(client, this);
//			int oldWorldId = client->currentWorld;
//			client->currentWorld = worldId;
//
//			// Notify the client.
//			sf::Packet notification;
//			notification << NetworkValues::NOTIFY << NetworkValues::PLAYER_MOVED_TO_WORLD << worldId << (*worldItr)->getMapId();
//			client->gameTcp->send(notification);
//
//			// Log.
//			Multithreading::outputMutex.lock();
//			std::cout << "[GAME_SERVER] Transferred '" << client->username << "' from world #" << oldWorldId << " to world #" << worldId << "." << std::endl;
//			Multithreading::outputMutex.unlock();
//		}
//	}
//}

//bool Server::checkAccountExists(const std::string& username)
//{
//	Multithreading::databaseMutex.lock();
//	bool result = m_database.checkAccountExists(username);
//	Multithreading::databaseMutex.unlock();
//
//	return result;
//}
//
//void Server::createAccount(const std::string& username, const std::string& password)
//{
//	Multithreading::databaseMutex.lock();
//	m_database.createAccount(username, password);
//	Multithreading::databaseMutex.unlock();
//}

//PlayerData Server::getPlayerData(const std::string& username)
//{
//	Multithreading::databaseMutex.lock();
//	PlayerData playerData = m_database.getPlayerData(username);
//	Multithreading::databaseMutex.unlock();
//
//	return playerData;
//}

//void Server::writePlayerData(const PlayerData& playerData)
//{
//	Multithreading::databaseMutex.lock();
//	m_database.writePlayerData(playerData);
//	Multithreading::databaseMutex.unlock();
//}
