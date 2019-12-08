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
Game::Game()
	//: players()
	//,bullets()
{

	std::cout << "CONSTUCTOR CALLED" << std::endl;
}

Game::~Game()
{
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

void Game::Update(sf::Time dt, std::map<std::string, Player*>& playerPointer, std::map<std::string, Bullet*>& bulletPointer)
{
	playersPtr = &playerPointer;
	bulletsPtr = &bulletPointer;

	std::cout << "[GAME ] Update Called " << dt.asSeconds() << std::endl;

	for (auto playerPair : (*playersPtr))
	{
		Player* player = playerPair.second;

		player->aimShape.setPosition(Interpolate2f(player->aimShape.getPosition(),
			player->clientLookTo,
			ServerConfiguration::playerAimSpeed * dt.asSeconds()));

		player->shape.setPosition(Interpolate2f(player->shape.getPosition(),
			player->clientMoveTo + player->velocity,
			ServerConfiguration::playerInterpolateSpeed * dt.asSeconds()));

		player->ClientUpdate(dt);
	}

	for (auto bulletPair : (*bulletsPtr))
	{
		Bullet* bullet = bulletPair.second;

		sf::Vector2f bulletPos = bullet->shape.getPosition();

		//Check Out of bounds and reverse for client side iteration
		if (bulletPos.x < 0) {
			bullet->shape.setPosition(window->getSize().x, bulletPos.y);
			bullet->clientMoveTo = sf::Vector2f(window->getSize().x, bulletPos.y);
		}
		else if (bulletPos.x > window->getSize().x) {
			bullet->shape.setPosition(0, bulletPos.y);
			bullet->clientMoveTo = sf::Vector2f(0, bulletPos.y);
		}
		else if (bulletPos.y < 0) {
			bullet->shape.setPosition(bulletPos.x, window->getSize().y);
			bullet->clientMoveTo = sf::Vector2f(bulletPos.x, window->getSize().y);
		}
		else if (bulletPos.y > window->getSize().y) {
			bullet->shape.setPosition(bulletPos.x, 0);
			bullet->clientMoveTo = sf::Vector2f(bulletPos.x, 0);
		}


		bullet->shape.setPosition(Interpolate2f(bullet->shape.getPosition(),
			bullet->clientMoveTo + bullet->velocity,
			ServerConfiguration::bulletInterpolateSpeed * dt.asSeconds()));
	}
	//std::cout << "[GAME ] Update DONE " << std::endl;
}

void Game::Render(std::map<std::string, Player*>& playerPointer, std::map<std::string, Bullet*>& bulletPointer)
{
	playersPtr = &playerPointer;
	bulletsPtr = &bulletPointer;

	window->clear();

	//std::cout << "[GAME ] Render Called " << std::endl;

	// Render Player Boxes
	for (auto playerPair : (*playersPtr))
	{
		Player* player = playerPair.second;
		window->draw(player->GetShape());
		window->draw(player->GetAimShape());
	}

	////// Render Player Bullets
	for (auto bulletPair : (*bulletsPtr))
	{
		Bullet* bullet = bulletPair.second;
		window->draw(bullet->GetShape());
	}

	//std::cout << "[GAME ] Render Done " << std::endl;
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
	//std::cout << "GAME CLIENT IS RUNNING GAME: window Open " << window->isOpen() << " POSITION "
	//	<< spawnPlayerPosition.x << " "  << spawnPlayerPosition.y << std::endl;

	//while (window->isOpen())
	//{
	//	UpdateSFMLEvents();
	//	ProcessInput();
	//	Update(dtClock.restart());
	//	Render();
	//}
}



sf::Vector2f Game::Interpolate2f(const sf::Vector2f & pointA, const sf::Vector2f & pointB, float factor)
{
	if (factor > 1.f)
		factor = 1.f;

	else if (factor < 0.f)
		factor = 0.f;

	return pointA + (pointB - pointA) * factor;
}

float Game::InterpolateFloat(const float & pointA, const float & pointB, float factor)
{
	if (factor > 1.f)
		factor = 1.f;

	else if (factor < 0.f)
		factor = 0.f;

	return pointA + (pointB - pointA) * factor;
}
