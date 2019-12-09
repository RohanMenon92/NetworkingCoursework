#ifndef MESSAGES_H
#define MESSAGES_H

#pragma once

#include <SFML/System.hpp>

/** A position update message. */
struct PlayerMessage {	
	/** The object number of this player within the game world. */
	int playerNumber;

	/** The object name of this player within the game world. */
	std::string playerID;

	/** The coordinates of this player within the game world. */
	sf::Vector2f position;

	// Aiming at position
	sf::Vector2f aimAt;

	// player health
	int health;

	// Is player attacking
	bool isAttacking;

	// Is player blocking
	bool isBlocking;

	/** The time at which this message was sent.
		(Not the time at which it was received!) */
	float time;

	// Is player dead
	bool isDead;
};

/** A position update message for bullets. */
struct BulletMessage {
	/** The object number of this player within the game world. */
	int playerNumber;

	/** The object number of this bullet within the game world. */
	std::string bulletID;

	/** The coordinates of this bullet within the game world. */
	sf::Vector2f position;

	/** The time at which this message was sent.
		(Not the time at which it was received!) */
	float time;

	// Can have properties to check if it's interacting

	// Is bullet dead
	bool isDead;
};

/** A control signal for players. */
struct ControlMessage {
	sf::Vector2f mousePos;
	bool isForwardPressed;
	bool isAttackPressed;
	bool isBlockPressed;
	float atTime;
};
#endif

// Connect details
//NetworkValues::CONNECT <<
//spawnPosition.x << spawnPosition.y << playerNumber << serverTime;
