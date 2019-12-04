#pragma once
#ifndef BALL_HPP
#define BALL_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>


class Bullet
{
public:
	int bulletID;
	sf::CircleShape shape;
	sf::Vector2f currVelocity;
	float maxSpeed;

	Bullet(float radius = 5.f)
		: currVelocity(0.f, 0.f), maxSpeed(15.f)
	{
		this->shape.setRadius(radius);
		this->shape.setFillColor(sf::Color::Red);
	}
};
#endif