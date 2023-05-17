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
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#endif
#include <map>							// Error enum to strings.
#include <string>						// Strings
#include <regex>						// Regular expression for ip validation
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
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
			ADDRESS_NOT_SUPPORTED,
			SEND_MAGIC_PACKET_FAIL,
			CREATE_SOCKET_FAILURE,
		};

		/// <summary>Error enum to string map</summary>
		static std::map<WolClientError, std::string> WolClientErrorMap
		{
			{WolClientError::NONE,
			std::string("Error Code " + std::to_string((uint8_t)WolClientError::NONE) + ": No error.")},
			{WolClientError::ADDRESS_NOT_SUPPORTED,
			std::string("Error Code " + std::to_string((uint8_t)WolClientError::ADDRESS_NOT_SUPPORTED) + ": Address not supported.")},
			{WolClientError::SEND_MAGIC_PACKET_FAIL,
			std::string("Error Code " + std::to_string((uint8_t)WolClientError::SEND_MAGIC_PACKET_FAIL) + ": Failed to send magic packet.")},
		};

		/// <summary>Structure for the WoL Magic packet</summary>
		struct MagicPacket 
		{
			std::vector<uint8_t> data;

			MagicPacket(const std::string& macAddress) 
			{
				// 6 bytes of 0xFF followed by 16 repetitions of the target MAC address
				std::string delimiter = ":";
				std::string tempAddress = macAddress;
				std::string byte;
				size_t pos = 0;

				while ((pos = tempAddress.find(delimiter)) != std::string::npos) 
				{
					byte = tempAddress.substr(0, pos);
					unsigned char value = static_cast<unsigned char>(std::strtoul(byte.c_str(), nullptr, 16));
					data.push_back(value);
					tempAddress.erase(0, pos + delimiter.length());
				}
				unsigned char value = static_cast<unsigned char>(std::strtoul(tempAddress.c_str(), nullptr, 16));
				data.push_back(value);

				data.resize(6 + 16 * data.size(), 0xFF);
			}
		};

		class WOL_Client
		{
		public:
			WOL_Client();
			~WOL_Client();
			std::string GetMacAddressFromArpTable(const std::string& ipAddress, const std::string& ethDevice);
			int8_t		SendMagicPacket(const std::string& macAddress);
			std::string GetLastError();
		protected:
		private:
			WolClientError	mLastError;
			std::string		mLastOSError;
        };
    }
}

#endif // CPP_WOL_CLIENT