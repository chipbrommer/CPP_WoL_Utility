// CPP_WoL_Utility.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "Source/wol_client.h"

int main()
{
	std::cout << "Hello CMake." << std::endl;

	Essentials::Utilities::WOL_Client wol;

	std::cout << Essentials::Utilities::WolClientVersion << std::endl;

	return 0;
}
