#include "RenderGame.hpp"

RenderGame::RenderGame()
	:players()
	,bullets()
{
	fontText.loadFromFile("Antonio.ttf");
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
	Player* playerInstance = new Player(clientRef->playerNumber);

	float randomSpawnX = ((float)rand() / (float)RAND_MAX) * (mWindow->getSize().x*0.75f - mWindow->getSize().x*0.25f) + mWindow->getSize().x*0.25f;
	float randomSpawnY = ((float)rand() / (float)RAND_MAX) * (mWindow->getSize().y*0.75f - mWindow->getSize().y*0.25f) + mWindow->getSize().y*0.25f;

	sf::Vector2f spawnPosition(randomSpawnX, randomSpawnY);
	playerInstance->shape.setPosition(spawnPosition);
	playerInstance->healthHolder.setFont(fontText);
	playerInstance->usernameHolder.setFont(fontText);
	players.push_back(playerInstance);

	clientRef->playerBox = playerInstance;

	std::cout << "[RENDER GAME] ON SOCKET CONNECT X:" << randomSpawnX << " Y:" << randomSpawnY << std::endl;
	return spawnPosition;
}

void RenderGame::OnPlayerConnect(std::string username, Player* player)
{
	std::cout << "OnPlayerConnect:" << player->shape.getPosition().x << " " << player->shape.getPosition().y << std::endl;
	player->playerID = username;
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
	// Update Players
	for (Player* player : players) {
		// Check Player collision
		for (size_t i = 0; i < bullets.size(); i++) {
			Bullet* bullet = bullets[i];

			if (!bullet->isDead) {
				if (bullet->shape.getGlobalBounds().intersects(player->shape.getGlobalBounds()))
				{
					// Check if same player fired
					if (bullet->playerNumber != player->playerNumber) {
						// Not same player do damage and check shield
						std::cout << "RENDER HIT REGISTERED :: BulletNumber:" << bullet->playerNumber << " PlayerNumber:" << player->playerNumber<< std::endl;

						// Destroy Bullet
						bullet->isDead = true;
						if (player->isBlocking) {
							// Create new reflected bullet to prevent UDP interpolation issues
							player->bulletsFired++;
							Bullet* bulletInstance = new Bullet(player->playerNumber);
							bulletInstance->bulletID = player->playerID + std::to_string(player->bulletsFired);

							// Get direction from center of player to bullet position aim is towards
							sf::Vector2f aimDir = bullet->shape.getPosition() - player->shape.getPosition();
							// Normalize the direction as direction to reflect bullet to
							sf::Vector2f newHeading = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));

							bulletInstance->shape.setPosition(player->shape.getPosition()
								+ (newHeading * GameConstants::shieldReflectOffset));
							bulletInstance->velocity = newHeading * GameConstants::bulletMaxSpeed;
							bulletInstance->bulletID = player->playerID + std::to_string(player->bulletsFired);
							bullets.push_back(bulletInstance);
							continue;
						}
						else {
							// DamagePlayer
							player->OnHit();
						}
					}
				}
			}
		}


		// Set Aim Position Over Time, regular interpolation
		player->aimShape.setPosition(Interpolate2f(player->aimShape.getPosition(), 
			player->aimAt, 
			GameConstants::playerAimSpeed * dt.asSeconds()));

		// Set Rotation Over Time
		player->shape.setRotation(InterpolateFloat(player->shape.getRotation(),
			GetRotationAim(player->aimShape.getPosition(), player->aimAt),
			GameConstants::playerRotateSpeed * dt.asSeconds()));

		// Interpolate towards velocity
		player->shape.setPosition(Interpolate2f(player->shape.getPosition(),
			player->shape.getPosition() + player->velocity,
			GameConstants::playerSpeed * dt.asSeconds()));

		// Fire Bullet If Possible
		if (player->isAttacking && player->canAttackTimer == 0.f) {
			player->canAttackTimer = GameConstants::playerReloadSpeed;

			player->bulletsFired++;
			Bullet* bulletInstance = new Bullet(player->playerNumber);

			bulletInstance->shape.setPosition(player->shape.getPosition() + (player->heading * 35.f));
			bulletInstance->velocity = player->heading * GameConstants::bulletMaxSpeed;

			bulletInstance->bulletID = player->playerID + std::to_string(player->bulletsFired);
			bullets.push_back(bulletInstance);
		}

		// Update halth
		if (player->health <= 0) {
			// kill player
			player->isDead = true;
		}
		player->ServerUpdate(dt);
	}

	for (size_t i = 0; i < bullets.size(); i++) {
		Bullet* bullet = bullets[i];
		if (!bullet->isDead) {

			sf::Vector2f bulletPos = bullet->shape.getPosition();

			//Check Out of bounds and reverse
			/*if (bulletPos.x < 0) {
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
			}*/

			// Kill if out of render view
			if (bulletPos.x < 0) {
				bullet->isDead = true;
			}
			else if (bulletPos.x > mWindow->getSize().x) {
				bullet->isDead = true;
			}
			else if (bulletPos.y < 0) {
				bullet->isDead = true;
			}
			else if (bulletPos.y > mWindow->getSize().y) {
				bullet->isDead = true;
			}
			bullet->Update(dt);
		}
	}
}

void RenderGame::Render()
{
	//std::cout << "[RENDER GAME] RENDER " << std::endl;

	mWindow->clear();

	// Render Player Boxes
	for (Player* player : players) {
		if (!player->isDead) {
			mWindow->draw(player->shape);
			mWindow->draw(player->aimShape);
			mWindow->draw(player->healthHolder);
			mWindow->draw(player->usernameHolder);
		}
	}
	// Render Player Bullets
	for (Bullet* bullet : bullets) {
		if (!bullet->isDead) {
			mWindow->draw(bullet->shape);
		}
	}
	mWindow->display();
}

sf::Vector2f RenderGame::Interpolate2f(const sf::Vector2f & pointA, const sf::Vector2f & pointB, float factor)
{
	//if (factor > 1.f)
	//	factor = 1.f;

	//else if (factor < 0.f)
	//	factor = 0.f;

	return pointA + (pointB - pointA) * factor;
}

float RenderGame::InterpolateFloat(const float & pointA, const float & pointB, float factor)
{
	//if (factor > 1.f)
	//	factor = 1.f;

	//else if (factor < 0.f)
	//	factor = 0.f;

	return pointA + (pointB) * factor;
}
