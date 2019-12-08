#pragma once
#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>


class Bullet
{
public:
	std::string bulletID;
	sf::CircleShape shape;
	sf::Vector2f velocity;
	sf::Vector2f clientMoveTo;

	float maxSpeed;

	Bullet(float radius = 5.f);
	virtual ~Bullet();

	void Update(sf::Time dt);
	void ClientUpdate(sf::Time dt);

	sf::CircleShape GetShape();
	void SetClientMoveTo(sf::Vector2f lookPos);
};
#endif