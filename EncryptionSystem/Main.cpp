#include <iostream>

#include "Client/CalcClient.h"
#include "Server/CalcServer.h"


int main()
{
	bool IsServer;

	std::cout << "SYSTEM::Client(0), Server(1) : ";
	std::cin >> IsServer;

	if (IsServer)
	{
		//CalcServer calcServer = CalcServer();
	}
	else
	{
		//CalcClient calcClient = CalcClient();
	}

	std::cout << "APP::Exit" << std::endl;

	return 0;
}