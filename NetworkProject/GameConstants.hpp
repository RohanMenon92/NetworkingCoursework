#pragma once
#ifndef GAME_CONSTANTS
#define GAME_CONSTANTS

#include <SFML/Graphics/Color.hpp>

namespace GameConstants
{
	// How many packets to send of kill
	const int onKillSendTimeout = 50;
	
	const int healthMax = 10;
	const float bulletMaxSpeed = 200.f;

	const float playerFireOffset = 35.f;	
	const float shieldReflectOffset = 25.f;

	const float playerReloadSpeed = 2.f;
	const float playerAimSpeed = 10.f;
	const float playerInterpolateSpeed = 2.f;
	const float bulletInterpolateSpeed = 2.f;

	//const float bulletTeleportBuffer = 20.f;
	const float playerRotateSpeed = 10.f;

	const float bulletSpeed = 20.f;
	const float playerSpeed = 10.f;

	const unsigned int Version(1);
	const float TickPerSec(60.f);

	const sf::Color colorPlayerMap[] = {
		sf::Color::Blue,
		sf::Color::Green,
		sf::Color::Red,
		sf::Color::Cyan,
		sf::Color::Magenta,
		sf::Color::Yellow
	};
}
#endif