#include<Game.hpp>
//static functions

//initialize functions
void Game::initWindow()
{
	//creates an SFML window using options from window.ini file.
	std::ifstream ifs("Config/window.ini");
	sf::VideoMode window_bounds(800, 600);
	std::string title = "ClientGame";
	unsigned framerate_limit = 120;
	bool vertical_sync_enabled = false;

	if (ifs.is_open()) {
		std::getline(ifs, title);
		ifs >> window_bounds.width >> window_bounds.height;
		ifs >> framerate_limit;
		ifs >> vertical_sync_enabled;
	}
	ifs.close();

	window = new sf::RenderWindow(window_bounds, title);
	window->setFramerateLimit(framerate_limit);
	window->setVerticalSyncEnabled(vertical_sync_enabled);

	std::cout << "initWindow Called" << std::endl;
}

//constructors/destructors
Game::Game(std::vector<Player*>& playerPointer, std::vector<Bullet*>& bulletPointer)
	//: players()
	//,bullets()
{
	players = &playerPointer;
	bullets = &bulletPointer;

	std::cout << "CONSTUCTOR CALLED" << std::endl;
}

void Game::ProcessInput()
{	
	// Mouse Positions
	mousePos = (sf::Vector2f)sf::Mouse::getPosition(*(window));

	sf::Vector2u windowSize = window->getSize();

	//Limit mouse movement within X bounds
	if (mousePos.x + 5.f > windowSize.x) {
		mousePos.x = windowSize.x - 5.f;
	}
	else if (mousePos.x - 5.f < 0) {
		mousePos.x = 5.f;
	}
	else {
		mousePos.x = mousePos.x;
	}

	//Limit mouse movement within Y bounds
	if (mousePos.y + 5.f > windowSize.y) {
		mousePos.y = windowSize.y - 5.f;
	}
	else if (mousePos.y - 5.f / 2 < 0) {
		mousePos.y = 5.f;
	}
	else {
		mousePos.y = mousePos.y;
	}

	isForwardPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
	isAttackPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
	isBlockPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
}

void Game::UpdateSFMLEvents()
{
	while (window->pollEvent(sfEvent))
	{
		if (sfEvent.type == sf::Event::Closed)
			window->close();
	}
}

void Game::Update(sf::Time dt)
{
	// Adding all new player boxes and bullets here
	// To prevent thread from causing issues in iteration

	std::vector<Player*> playersCopy = *players;
	std::vector<Bullet*> bulletsCopy = *bullets;

	for (Player* playerBox : playersCopy) {
		playerBox->ClientUpdate(dt);
	}

	for (Bullet* bullet : bulletsCopy) {
		bullet->ClientUpdate(dt);

		sf::Vector2f bulletPos = bullet->shape.getPosition();
		if (bulletPos.x < 0 || bulletPos.x > window->getSize().x
			|| bulletPos.y < 0 || bulletPos.y > window->getSize().y)
		{
			DespawnBullet(bullet->bulletID);
		}
	}
}

void Game::Render()
{
	//std::cout << "[CLIENT GAME] RENDER " << std::endl;

	std::vector<Player*> playersCopy = *players;
	std::vector<Bullet*> bulletsCopy = *bullets;

	window->clear();


	// Render Player Boxes
	for (Player* playerBox : playersCopy) {
		window->draw(playerBox->shape);
		window->draw(playerBox->aimShape);
	}

	// Render Player Bullets
	for (Bullet* bullet : bulletsCopy) {
		window->draw(bullet->shape);
	}

	window->display();
}

void Game::DespawnPlayer(std::string playerID)
{
	//playerBoxes.erase(playerID);
}

void Game::DespawnBullet(std::string bulletID)
{
	//bullets.erase(bulletID);
}

void Game::RunGame(sf::Vector2f spawnPlayerPosition)
{
	std::cout << "GAME CLIENT IS RUNNING GAME: window Open " << window->isOpen() << " POSITION "
		<< spawnPlayerPosition.x << " "  << spawnPlayerPosition.y << std::endl;

	while (window->isOpen())
	{
		UpdateSFMLEvents();
		ProcessInput();
		Update(dtClock.restart());
		Render();
	}
}