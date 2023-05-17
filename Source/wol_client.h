///////////////////////////////////////////////////////////////////////////////
//!
//! @file		wol_client.h
//! 
//! @brief		A cross platform class to handle wake-on-lan requests.
//! 
//! @author		Chip Brommer
//! 
//! @date		< 05 / 15 / 2023 > Initial Start Date
//!
/*****************************************************************************/
#pragma once
///////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#ifdef WIN32
#include <stdint.h>						// Standard integer types
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#include <map>							// Error enum to strings.
#include <string>						// Strings
#include <regex>						// Regular expression for ip validation
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
//
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CPP_WOL_CLIENT				// Define the cpp UDP client class. 
#define     CPP_WOL_CLIENT
//
///////////////////////////////////////////////////////////////////////////////

namespace Essentials
{
    namespace Utilities
    {
		constexpr static uint8_t	WOL_CLIENT_VERSION_MAJOR = 0;
		constexpr static uint8_t	WOL_CLIENT_VERSION_MINOR = 1;
		constexpr static uint8_t	WOL_CLIENT_VERSION_PATCH = 0;
		constexpr static uint8_t	WOL_CLIENT_VERSION_BUILD = 0;
		constexpr static uint8_t	WOL_SOCKET_TIMEOUT = 100;

		static std::string WolClientVersion = "WoL Client v" +
			std::to_string(WOL_CLIENT_VERSION_MAJOR) + "." +
			std::to_string(WOL_CLIENT_VERSION_MINOR) + "." +
			std::to_string(WOL_CLIENT_VERSION_PATCH) + " - b" +
			std::to_string(WOL_CLIENT_VERSION_BUILD) + ".\n";

		/// <summary>enum for error codes</summary>
		enum class WolClientError : uint8_t
		{
			NONE,
		};

		/// <summary>Error enum to string map</summary>
		static std::map<WolClientError, std::string> WolClientErrorMap
		{
			{WolClientError::NONE,
			std::string("Error Code " + std::to_string((uint8_t)WolClientError::NONE) + ": No error.")},
		};

		/// <summary>Represents an endpoint for a connection</summary>
		struct Endpoint
		{
			std::string ipAddress	= "";
			int16_t		port		= 0;
			std::string macAddress	= "";
		};

		class WOL_Client
		{
		public:
			WOL_Client();
			~WOL_Client();
			int8_t		Send(const std::string& macAddress);
			std::string GetMacAddressFromArpTable(const std::string& ipAddress);
		protected:
		private:
			int8_t		GenerateMagicPacket(const std::string& macAddress, std::vector<uint8_t>& magicPacket);
			int8_t		SendPacket(const std::vector<uint8_t>& packet);
			std::string FormatMacAddress(const std::vector<uint8_t>& macBytes);

			Endpoint* mBroadcastAddress;

        };
    }
}

#endif // CPP_WOL_CLIENT