// CPP_WoL_Utility.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "Source/wol_client.h"

int main()
{
	std::cout << "Hello CMake." << std::endl;

	Essentials::Utilities::WOL_Client wol;
	 
	std::cout << Essentials::Utilities::WolClientVersion << std::endl;

	std::string mac = wol.GetMacAddressFromArpTable("192.168.1.15", "eth0");

	std::cout << mac << std::endl;

	if (wol.SendMagicPacket(mac) < 0)
	{
		std::cout << wol.GetLastError();
		std::cout << "Sending Magic Packet Failed!\n";
	}
	else
	{
		std::cout << "Magic Packet Sent!\n";
	}

	return 0;
}
