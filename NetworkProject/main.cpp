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
			// Server does not need instancing as there should be only 1 locally
			Server* server = new Server(ServerConfiguration::LocalIPAddress);
			server->Init();
		}
		else if (netState == "c" || netState == "C") {
			std::string username;
			std::string ip;
			std::cout << "What's your username?\n";
			std::cin >> username;
			std::cout << "Is server local? y/n \n";
			std::string isServerLocal;
			std::cin >> isServerLocal;
			if (isServerLocal == "y" || "Y") {
				ip = ServerConfiguration::LocalIPAddress;
			}
			else {
				std::cout << "What is the Host IP? y/n \n";
				std::string hostIP;
				std::cin >> hostIP;
				ip = hostIP;
			}
			
			// Client needs instancing as there can be multiple locally
			Client* client = new Client(username, ip);
		}
		std::cout << "Invalid input, try again. \n";
	} while (!defined);

	return 0;
}