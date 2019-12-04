#pragma once
#ifndef PLAYER_BOX_HPP
#define PLAYER_BOX_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

class PlayerBox
{
public:
	std::string playerID;

	sf::RectangleShape shape;

	bool isAttacking;
	bool isBlocking;
	bool isMovingForward;

	sf::Vector2f aimAt;	
	float targetRotation;

	void OnAttackPressed();
	void OnAttackUnpressed();

	void OnBlockPressed();
	void OnBlockUnpressed();

	void OnForwardPressed();
	void OnForwardUnpressed();

	void Update(sf::Time dt);

protected:

private:

};
#endif