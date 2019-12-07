#pragma once
#ifndef PLAYER_BOX_HPP
#define PLAYER_BOX_HPP

#include<iostream>

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>

class Player
{
public:
	std::string playerID;

	sf::CircleShape shape;
	sf::CircleShape aimShape;

	bool isAttacking;
	bool isBlocking;
	bool isMovingForward;

	int bulletsFired;
	float canAttackTimer;
	float health;

	sf::Vector2f aimAt;
	sf::Vector2f velocity;
	sf::Vector2f heading;
	float targetRotation;

	void Update(sf::Time dt);
	void ClientUpdate(sf::Time dt);
	void SetAimPos(sf::Vector2f aimAt);
	Player();

	sf::CircleShape GetShape();
	sf::CircleShape GetAimShape();

protected:

private:
	float moveForwardSpeed;
};
#endif