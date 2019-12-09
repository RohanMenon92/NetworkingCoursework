#pragma once
#ifndef BULLET_HPP
#define BULLET_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include<Messages.hpp>
#include<GameConstants.hpp>

class Bullet
{
public:
	int playerNumber;

	std::string bulletID;
	sf::CircleShape shape;
	sf::Vector2f velocity;
	sf::Vector2f clientMoveTo;

	// Notifies to not render bullet anymore
	bool isDead = false;
	// Notifies to not send bullet anymore after sending dead update
	int isDeadSentCount = GameConstants::onKillSendTimeout;

	std::vector<BulletMessage> messagesStored;

	Bullet(int playerNumber, float radius = 5.f);
	virtual ~Bullet();

	void Update(sf::Time dt);
	void ClientUpdate(sf::Time dt);

	void AddMessage(const BulletMessage &msg);
	sf::Vector2f PredictPosition(float time);
};
#endif