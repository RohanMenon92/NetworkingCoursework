#pragma once
#ifndef PLAYER_BOX_HPP
#define PLAYER_BOX_HPP

#include <cmath>
#include<iostream>
#include<string>

#include<GameConstants.hpp>
#include<Messages.hpp>
#include<SFML/System/Time.hpp>
#include<SFML/Graphics/Transformable.hpp>
#include<SFML/Graphics/Drawable.hpp>
#include<SFML/Graphics/Color.hpp>
#include<SFML/Graphics/CircleShape.hpp>
#include<SFML/Graphics/Sprite.hpp>
#include<SFML/Graphics/Text.hpp>

class Player
{
public:
	sf::Text healthHolder;
	sf::Text usernameHolder;

	bool isDead = false;
	// Notifies to not send player anymore after sending dead update
	int isDeadSentCount = GameConstants::onKillSendTimeout;
	
	std::string playerID;

	sf::CircleShape shape;
	sf::CircleShape aimShape;

	bool isAttacking;
	bool isBlocking;
	bool isMovingForward;

	int bulletsFired;
	float canAttackTimer;
	int health;

	sf::Vector2f aimAt;
	sf::Vector2f velocity;
	sf::Vector2f heading;
	float targetRotation;

	sf::Vector2f clientLookTo;

	/** The messages that have been received about this player. */
	std::vector<PlayerMessage> messagesStored;

	void ServerUpdate(sf::Time dt);
	void SetClientLookTo(sf::Vector2f lookPos);
	void ClientUpdate(sf::Time dt);
	void AddMessage(const PlayerMessage &msg);
	sf::Vector2f PredictPosition(float time);
	void SetAimPos(sf::Vector2f aimAt);

	Player();
	virtual ~Player();

protected:

};
#endif