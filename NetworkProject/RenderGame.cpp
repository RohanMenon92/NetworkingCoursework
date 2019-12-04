#include "RenderGame.hpp"

RenderGame::RenderGame()
	:playerBoxes()
	,bullets()
{
	this->InitWindow();
}

void RenderGame::run() {
	while (mWindow->isOpen())
	{
		ProcessPlayerInput();
		UpdateSFMLEvents();
		Update(dtClock.restart());
		Render();
	}
}

sf::Vector2f RenderGame::OnSocketConnect(ClientRef* clientRef)
{
	PlayerBox playerBoxInstance;
	clientRef->playerBox = &playerBoxInstance;

	float randomSpawnX = ((float)rand() / (float)RAND_MAX) * (mWindow->getSize().x - 0) + 0;
	float randomSpawnY = ((float)rand() / (float)RAND_MAX) * (mWindow->getSize().x - 0) + 0;

	//float randomSpawnX = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0 - mWindow->getSize().x)));
	//float randomSpawnY = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0 - mWindow->getSize().y)));

	sf::Vector2f spawnPosition = { randomSpawnX, randomSpawnY };
	playerBoxInstance.shape.setPosition(spawnPosition);
	playerBoxes.push_back(PlayerBox(playerBoxInstance));

	std::cout << "[RENDER GAME] ON SOCKET CONNECT X:" << randomSpawnX << " Y:" << randomSpawnY << std::endl;

	return spawnPosition;
}

void RenderGame::OnPlayerConnect(std::string username, PlayerBox* playerBox)
{
	playerBox->playerID = username;
	std::cout << "PLAYER HAS CONNECTED :" + username << std::endl;
}

void RenderGame::OnPlayerAimMove(std::string username, sf::Vector2f position)
{
}

void RenderGame::OnPlayerDisconnect(std::string username)
{
}

void RenderGame::InitWindow()
{
	//creates an SFML window using options from window.ini file.
	std::ifstream ifs("Config/window.ini");
	sf::VideoMode window_bounds(800, 600);
	std::string title = "Render Game Screen";
	unsigned framerate_limit = 120;
	bool vertical_sync_enabled = false;

	if (ifs.is_open()) {
		std::getline(ifs, title);
		ifs >> window_bounds.width >> window_bounds.height;
		ifs >> framerate_limit;
		ifs >> vertical_sync_enabled;
	}
	ifs.close();

	this->mWindow = new sf::RenderWindow(window_bounds, title);
	this->mWindow->setFramerateLimit(framerate_limit);
	this->mWindow->setVerticalSyncEnabled(vertical_sync_enabled);
}

void RenderGame::ProcessPlayerInput()
{
	//std::cout << "[RENDER GAME] PROCESS SERVER INPUT" << std::endl;

}

void RenderGame::UpdateSFMLEvents()
{
	while (mWindow->pollEvent(sfEvent))
	{
		if (sfEvent.type == sf::Event::Closed)
			mWindow->close();
	}
}

void RenderGame::Update(sf::Time dt)
{
	//std::cout << "[RENDER GAME] UPDATE CALL DT:" << dt.asSeconds() << std::endl;

}

void RenderGame::Render()
{
	//std::cout << "[RENDER GAME] RENDER " << std::endl;

	mWindow->clear();

	// Render Player Boxes
	for (PlayerBox playerBox : playerBoxes) {
		mWindow->draw(playerBox.shape);
	}
	// Render Player Bullets
	for (Bullet bullet : bullets) {
		mWindow->draw(bullet.shape);
	}

	mWindow->display();
}
