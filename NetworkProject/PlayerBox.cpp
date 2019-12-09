#include "PlayerBox.hpp"

Player::Player(int playerNumber) :
	isAttacking(false)
	, playerNumber(playerNumber)
	, isBlocking(false)
	, isMovingForward(false)
	, targetRotation(0)
	, aimAt(100.f, 100.f)
	, bulletsFired(0)
	, aimShape(10.f)
	, health(GameConstants::healthMax)
{
	shape.setRadius(25.f);
	shape.setFillColor(sf::Color::White);
	shape.setOrigin(25.f, 25.f);
	aimShape.setFillColor(GameConstants::colorPlayerMap[playerNumber]);
	aimShape.setOrigin(5.f, 5.f);

	healthHolder.setCharacterSize(20);
	healthHolder.setStyle(sf::Text::Regular);
	healthHolder.setFillColor(GameConstants::colorPlayerMap[playerNumber]);

	usernameHolder.setCharacterSize(30);
	usernameHolder.setStyle(sf::Text::Bold);
	usernameHolder.setFillColor(GameConstants::colorPlayerMap[playerNumber]);
}

Player::~Player()
{
}

void Player::OnHit()
{
	health--;
}

void Player::ServerUpdate(sf::Time dt)
{
	if (canAttackTimer > 0.f) {
		canAttackTimer -= dt.asSeconds();
	}
	else {
		canAttackTimer = 0.f;
	}

	sf::Vector2f aimDir = aimAt - shape.getPosition();
	// get heading your aim is towards
	heading = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));

	sf::Vector2f playerPos = shape.getPosition();
	float rotation = shape.getRotation();

	// 
	if (isBlocking) {
		shape.setFillColor(sf::Color::Blue);
		velocity.x = 0.f;
		velocity.y = 0.f;
	} else {
		if (isAttacking) {
			shape.setFillColor(sf::Color::Red);
		}

		if (isMovingForward) {
			velocity = heading * GameConstants::playerSpeed;
		}
		else {
			velocity.x = 0.f;
			velocity.y = 0.f;
		}
	}

	if (!isBlocking && !isAttacking) {
		shape.setFillColor(sf::Color::White);
	}

	// position healthCounter
	healthHolder.setString(std::to_string(health) + "/" + std::to_string(GameConstants::healthMax));
	healthHolder.setPosition(shape.getPosition() -
		sf::Vector2f(healthHolder.getLocalBounds().width / 2, healthHolder.getLocalBounds().height / 2));

	// position username
	usernameHolder.setString(playerID);
	usernameHolder.setPosition(shape.getPosition() - sf::Vector2f(0, -50) -
		sf::Vector2f(usernameHolder.getLocalBounds().width / 2, usernameHolder.getLocalBounds().height / 2));
}

void Player::SetClientLookTo(sf::Vector2f lookPos) {
	clientLookTo = lookPos;
}

void Player::ClientUpdate(sf::Time dt)
{
	if (isBlocking) {
		shape.setFillColor(GameConstants::colorPlayerMap[playerNumber]);
	}
	else {
		if (isAttacking) {
			shape.setFillColor(sf::Color::Red);
		}
	}

	if (!isBlocking && !isAttacking) {
		shape.setFillColor(sf::Color::White);
	}

	// position healthCounter
	healthHolder.setString(std::to_string(health) + "/" + std::to_string(GameConstants::healthMax));
	healthHolder.setPosition(shape.getPosition() - 
		sf::Vector2f(healthHolder.getLocalBounds().width/2, healthHolder.getLocalBounds().height / 2));

	// position username
	usernameHolder.setString(playerID);
	usernameHolder.setPosition(shape.getPosition() - sf::Vector2f(0, -50) - 
	   sf::Vector2f(usernameHolder.getLocalBounds().width / 2, usernameHolder.getLocalBounds().height / 2));
}

void Player::AddMessage(const PlayerMessage& msg) {
	if (messagesStored.size() == 2) {
		// if before first message Store message in first vector and shift	
		if (msg.time > messagesStored[0].time) {
			messagesStored[1] = messagesStored[0];
			messagesStored[0] = msg;
		}
		// else if before second message Store message in second vector
		else if(messagesStored[1].time > msg.time) {
			messagesStored[1] = msg;
		}
	}
	else if (messagesStored.size() == 1) {
		if (msg.time > messagesStored[0].time) {
			PlayerMessage lastMessage = messagesStored[0];
			messagesStored[0] = msg;
			messagesStored.push_back(lastMessage);
		} else if (msg.time != messagesStored[0].time) {
			messagesStored.push_back(msg);
		}
	}
	else if (messagesStored.size() == 0) {
		messagesStored.push_back(msg);
	}
}

sf::Vector2f Player::PredictPosition(float time) {
	if (messagesStored.size() == 2) {
		//We can interpolate to this position based on last 2 game times and positions
		sf::Vector2f posDifference = messagesStored[0].position - messagesStored[1].position;
		float timeDifference = messagesStored[0].time - messagesStored[1].time;

		velocity = posDifference / timeDifference;

		return messagesStored[0].position + (velocity * (time - messagesStored[0].time));
	}
	else if (messagesStored.size() == 1) {
		//We can interpolate to this position based on current game time and position
		sf::Vector2f posDifference = shape.getPosition() - messagesStored[0].position;
		float timeDifference = time - messagesStored[0].time;

		velocity = posDifference / timeDifference;

		return shape.getPosition() + (velocity * (time - messagesStored[0].time));
	}
	else {
		return shape.getPosition();
	}
}

void Player::SetAimPos(sf::Vector2f aimingAt)
{
	aimAt = aimingAt;
}
