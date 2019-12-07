#include <string>
#include <iostream>

#include <Client.hpp>
#include <Server.hpp>

//#include<SFML/Graphics.hpp>
//#include<SFML/System.hpp>
//#include<SFML/Audio.hpp>
//#include<SFML/Network.hpp>
//#include<SFML/Window.hpp>

int main(int argc, void** argv[])
{
	std::string netState;
	std::cout << "Are you the server (S/s) or the Client (C/c)? \n";

	bool defined = false;

	do {
		netState.clear();
		std::cin >> netState;
		if (netState == "s" || netState == "S") {
			Server server;
			server.Init();
			//defined = true;
		}
		else if (netState == "c" || netState == "C") {
			std::string username;
			std::cout << "What's your username?\n";
			std::cin >> username;

			Client client(username);
			client.Init();
			//defined = true;
		}
		std::cout << "Invalid input, try again. \n";
	} while (!defined);

	return 0;
}