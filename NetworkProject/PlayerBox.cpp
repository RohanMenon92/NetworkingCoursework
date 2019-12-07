#include "PlayerBox.hpp"

Player::Player() :
	isAttacking(false)
	, isBlocking(false)
	, isMovingForward(false)
	, targetRotation(0)
	, aimAt(100.f, 100.f)
	, bulletsFired(0)
	, aimShape(5.f)
	, moveForwardSpeed(40.f)
{
	shape.setRadius(25.f);
	shape.setFillColor(sf::Color::White);
	shape.setOrigin(25.f, 25.f);

	aimShape.setFillColor(sf::Color::Green);
	//	player.setPointCount(3);
	aimShape.setOrigin(5.f, 5.f);
}

void Player::Update(sf::Time dt)
{
	if (canAttackTimer > 0.f) {
		canAttackTimer -= dt.asSeconds();
	}
	else {
		canAttackTimer = 0.f;
	}

	sf::Vector2f aimDir = aimAt - shape.getPosition();
	heading = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));

	sf::Vector2f playerPos = shape.getPosition();
	float rotation = shape.getRotation();

	if (isBlocking) {
		shape.setFillColor(sf::Color::Blue);
		velocity.x = 0.f;
		velocity.y = 0.f;
	} else {
		if (isAttacking) {
			shape.setFillColor(sf::Color::Red);
		}

		if (isMovingForward) {
			velocity = heading * moveForwardSpeed;
		}
		else {
			velocity.x = 0.f;
			velocity.y = 0.f;
		}
	}

	if (!isBlocking && !isAttacking) {
		shape.setFillColor(sf::Color::White);
	}

	shape.move(velocity * dt.asSeconds());
}

void Player::ClientUpdate(sf::Time dt)
{
	if (isBlocking) {
		shape.setFillColor(sf::Color::Blue);
	}
	else {
		if (isAttacking) {
			shape.setFillColor(sf::Color::Red);
		}
	}

	aimShape.setPosition(aimAt);

	shape.move(velocity * dt.asSeconds());
}

void Player::SetAimPos(sf::Vector2f aimingAt)
{
	aimAt = aimingAt;
}
