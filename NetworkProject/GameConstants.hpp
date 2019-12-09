#pragma once
#ifndef GAME_CONSTANTS
#define GAME_CONSTANTS

namespace GameConstants
{
	// How many packets to send of kill
	const int onKillSendTimeout = 50;
	const int healthMax = 10;

	const float playerAimSpeed = 10.f;
	const float playerInterpolateSpeed = 2.f;
	const float bulletInterpolateSpeed = 2.f;

	//const float bulletTeleportBuffer = 20.f;
	const float playerRotateSpeed = 10.f;

	const float bulletSpeed = 20.f;
	const float playerSpeed = 10.f;

	const unsigned int Version(1);
	const float TickPerSec(60.f);
}
#endif