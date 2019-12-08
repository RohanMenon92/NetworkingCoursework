#include "Bullet.hpp"

Bullet::Bullet(float radius)
	: velocity(0.f, 0.f), maxSpeed(100.f)
{
	this->shape.setRadius(radius);
	this->shape.setFillColor(sf::Color::Red);
}

Bullet::~Bullet()
{
}

void Bullet::Update(sf::Time dt)
{
	shape.move(velocity * dt.asSeconds());
}

void Bullet::ClientUpdate(sf::Time dt)
{
	//shape.move(velocity * dt.asSeconds());
}

sf::CircleShape Bullet::GetShape()
{
	return shape;
}

void Bullet::SetClientMoveTo(sf::Vector2f movePos)
{
	clientMoveTo = movePos;
}
