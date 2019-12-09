#include<ServerConfiguration.hpp>
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
}

void Bullet::AddMessage(const BulletMessage &msg)
{
	if (messagesStored.size() == 2) {
		// if before first message Store message in first vector and shift	
		if (msg.time > messagesStored[0].time) {
			messagesStored[1] = messagesStored[0];
			messagesStored[0] = msg;
		}
		// else if before second message Store message in second vector
		else if (messagesStored[1].time > msg.time) {
			messagesStored[1] = msg;
		}
	}
	else if (messagesStored.size() == 1) {
		if (msg.time > messagesStored[0].time) {
			BulletMessage lastMessage = messagesStored[0];
			messagesStored[0] = msg;
			messagesStored.push_back(lastMessage);
		}
		else if (msg.time != messagesStored[0].time) {
			messagesStored.push_back(msg);
		}
	}
	else if (messagesStored.size() == 0) {
		messagesStored.push_back(msg);
	}
}

sf::Vector2f Bullet::PredictPosition(float time)
{
	if (messagesStored.size() == 2) {
		//We can linear interpolate to this position based on last 2 game times and positions
		sf::Vector2f posDifference = messagesStored[0].position - messagesStored[1].position;
		float timeDifference = messagesStored[0].time - messagesStored[1].time;

		velocity = posDifference / timeDifference;

		return messagesStored[0].position + (velocity * (time - messagesStored[0].time));
	}
	else if (messagesStored.size() == 1) {
		//We can linear interpolate to this position based on current game time and position
		sf::Vector2f posDifference = shape.getPosition() - messagesStored[0].position;
		float timeDifference = time - messagesStored[0].time;

		velocity = posDifference / timeDifference;

		return shape.getPosition() + (velocity * (time - messagesStored[0].time));
	}
	else {
		return shape.getPosition();
	}
}