#ifndef NETWORKVALUES_HPP_INCLUDED
#define NETWORKVALUES_HPP_INCLUDED

namespace NetworkValues
{
	const unsigned int	CONNECT(100); // { string username }
	const unsigned int	RETURN_CONNECT(101); // { string username }
	const unsigned int	NOTIFY(102); // { string username, string message }
	const unsigned int	PLAYER_DISCONNECTED(103); // { string username }
	const unsigned int	PLAYER_CONNECTED(104); // { string username }

	const unsigned int DISCONNECT(201);
	const unsigned int CONTROL(203); // { string username, vector2f mousePositions, keyPressed values }
	const unsigned int ENDGAME(205); // { string usernameHitting, string usernameHit }

	const unsigned int  RENDER_BULLET(210);
	const unsigned int  RENDER_PLAYER(211);
} // namespace NetworkValues.

#endif // NETWORKVALUES_HPP_INCLUDED
