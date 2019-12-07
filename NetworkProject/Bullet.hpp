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
	float maxSpeed;

	Bullet(float radius = 5.f);

	void Update(sf::Time dt);
	void ClientUpdate(sf::Time dt);
};
#endif