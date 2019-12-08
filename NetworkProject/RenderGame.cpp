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
	Player* playerBoxInstance = new Player();

	float randomSpawnX = ((float)rand() / (float)RAND_MAX) * (mWindow->getSize().x*0.75f - mWindow->getSize().x*0.25f) + mWindow->getSize().x*0.25f;
	float randomSpawnY = ((float)rand() / (float)RAND_MAX) * (mWindow->getSize().y*0.75f - mWindow->getSize().y*0.25f) + mWindow->getSize().y*0.25f;

	sf::Vector2f spawnPosition(randomSpawnX, randomSpawnY);
	playerBoxInstance->shape.setPosition(spawnPosition);
	playerBoxes.push_back(playerBoxInstance);

	clientRef->playerBox = playerBoxInstance;

	std::cout << "[RENDER GAME] ON SOCKET CONNECT X:" << randomSpawnX << " Y:" << randomSpawnY << std::endl;
	return spawnPosition;
}

void RenderGame::OnPlayerConnect(std::string username, Player* playerBox)
{
	std::cout << "OnPlayerConnect:" << playerBox->GetShape().getPosition().x << " " << playerBox->GetShape().getPosition().y << std::endl;
	playerBox->playerID = username;
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

float RenderGame::GetRotationAim(sf::Vector2f currPos, sf::Vector2f aimPos) {
	sf::Vector2f aimDir = aimPos - currPos;
	sf::Vector2f aimDirNorm = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));

	float PI = 3.14159265f;
	return (atan2(aimDirNorm.y, aimDirNorm.x) * 180 / PI) + 90;
}

void RenderGame::Update(sf::Time dt)
{
	//std::cout << "[RENDER GAME] UPDATE CALL DT:" << dt.asSeconds() << std::endl;

	// Update PlaterBoxes
	for (Player* playerBox : playerBoxes) {
		// Set Position Over Time
		playerBox->aimShape.setPosition(Interpolate2f(playerBox->aimShape.getPosition(), 
			playerBox->aimAt, 
			ServerConfiguration::playerAimSpeed * dt.asSeconds()));

		// Set Rotation Over Time
		playerBox->shape.setRotation(InterpolateFloat(playerBox->shape.getRotation(), 
			GetRotationAim(playerBox->aimShape.getPosition(), playerBox->aimAt), 
			ServerConfiguration::playerRotateSpeed * dt.asSeconds()));

		// Fire Bullet If Possible
		if (playerBox->isAttacking && playerBox->canAttackTimer == 0.f) {
			playerBox->canAttackTimer = ServerConfiguration::PlayerReloadSpeed;

			playerBox->bulletsFired++;
			Bullet* bulletInstance = new Bullet();

			bulletInstance->shape.setPosition(playerBox->shape.getPosition() + (playerBox->heading * 35.f));
			bulletInstance->velocity = playerBox->heading * bulletInstance->maxSpeed;

			bulletInstance->bulletID = playerBox->playerID + std::to_string(playerBox->bulletsFired);
			bullets.push_back(bulletInstance);
		}

		playerBox->Update(dt);
	}

	for (size_t i = 0; i < bullets.size(); i++) {
		sf::Vector2f bulletPos = bullets[i]->shape.getPosition();

		//Check Out of bounds and reverse
		if (bulletPos.x < 0) {
			bullets[i]->shape.setPosition(mWindow->getSize().x, bulletPos.y);
		}
		else if (bulletPos.x > mWindow->getSize().x) {
			bullets[i]->shape.setPosition(0, bulletPos.y);
		}
		else if (bulletPos.y < 0) {
			bullets[i]->shape.setPosition(bulletPos.x, mWindow->getSize().y);
		}
		else if (bulletPos.y > mWindow->getSize().y) {
			bullets[i]->shape.setPosition(bulletPos.x, 0);
		}
		bullets[i]->Update(dt);
	}
}

void RenderGame::Render()
{
	//std::cout << "[RENDER GAME] RENDER " << std::endl;

	mWindow->clear();

	// Render Player Boxes
	for (Player* playerBox : playerBoxes) {
		mWindow->draw(playerBox->GetShape());
		mWindow->draw(playerBox->GetAimShape());
	}
	// Render Player Bullets
	for (Bullet* bullet : bullets) {
		mWindow->draw(bullet->GetShape());
	}
	mWindow->display();
}

sf::Vector2f RenderGame::Interpolate2f(const sf::Vector2f & pointA, const sf::Vector2f & pointB, float factor)
{
	if (factor > 1.f)
		factor = 1.f;

	else if (factor < 0.f)
		factor = 0.f;

	return pointA + (pointB - pointA) * factor;
}

float RenderGame::InterpolateFloat(const float & pointA, const float & pointB, float factor)
{
	if (factor > 1.f)
		factor = 1.f;

	else if (factor < 0.f)
		factor = 0.f;

	return pointA + (pointB - pointA) * factor;
}
