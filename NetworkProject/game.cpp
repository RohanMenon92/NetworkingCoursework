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
}

//constructors/destructors
Game::Game()
{
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
	gameTime += dt.asSeconds();

	playersPtr = &playerPointer;
	bulletsPtr = &bulletPointer;

	//std::cout << "[GAME ] Update Called " << dt.asSeconds() << std::endl;

	for (auto playerPair : (*playersPtr))
	{
		Player* player = playerPair.second;

		if (!player->isDead) {
			player->aimShape.setPosition(Interpolate2f(player->aimShape.getPosition(),
				player->clientLookTo,
				GameConstants::playerAimSpeed * dt.asSeconds()));

			//std::cout << "[GAME] CurrentPos:" << player->shape.getPosition().x << " " << player->shape.getPosition().y
			//	<< "MOVING PLAYER TO:" << player->PredictPosition(gameTime).x << " " << player->PredictPosition(gameTime).y << std::endl;

			player->shape.setPosition(Interpolate2f(player->shape.getPosition(),
				player->PredictPosition(gameTime),
				GameConstants::playerSpeed * dt.asSeconds()));

			player->ClientUpdate(dt);
		}
	}

	for (auto bulletPair : (*bulletsPtr))
	{
		Bullet* bullet = bulletPair.second;

		if (!bullet->isDead) {
			sf::Vector2f bulletPos = bullet->shape.getPosition();

			//Check Out of bounds and reverse
			//if (bulletPos.x < 0) {
			//	bullet->shape.setPosition(window->getSize().x, bulletPos.y);
			//}
			//else if (bulletPos.x > window->getSize().x) {
			//	bullet->shape.setPosition(0, bulletPos.y);
			//}
			//else if (bulletPos.y < 0) {
			//	bullet->shape.setPosition(bulletPos.x, window->getSize().y);
			//}
			//else if (bulletPos.y > window->getSize().y) {
			//	bullet->shape.setPosition(bulletPos.x, 0);
			//}

			// Update bullet prediction velocity to point
			bullet->shape.setPosition(Interpolate2f(bulletPos,
				bullet->PredictPosition(gameTime),
				GameConstants::bulletSpeed * dt.asSeconds()));
		}
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
		if (!player->isDead) {
			window->draw(player->shape);
			window->draw(player->aimShape);
			window->draw(player->healthHolder);
			window->draw(player->usernameHolder);
		}
	}

	////// Render Player Bullets
	for (auto bulletPair : (*bulletsPtr))
	{
		Bullet* bullet = bulletPair.second;
		if (!bullet->isDead) {
			window->draw(bullet->shape);
		}
	}

	//std::cout << "[GAME ] Render Done " << std::endl;
	window->display();
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
