///////////////////////////////////////////////////////////////////////////////
//!
//! @file		wol_client.cpp
//! 
//! @brief		Implementation of the wol (wake-on-lan) client class
//! 
//! @author		Chip Brommer
//! 
//! @date		< 05 / 15 / 2023 > Initial Start Date
//!
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#include	"wol_client.h"				// WoL Client Class
//
///////////////////////////////////////////////////////////////////////////////

namespace Essentials
{
    namespace Utilities
    {
        WOL_Client::WOL_Client()
        {
            mLastError      = WolClientError::NONE;
            mLastOSError    = "";
        }

        WOL_Client::~WOL_Client()
        {

        }

        std::string WOL_Client::GetMacAddressFromArpTable(const std::string& ipAddress, const std::string& ethDevice)
        {
            std::string macAddress = "";

#ifdef _WIN32
            IPAddr destIp = {};

            if (inet_pton(AF_INET, ipAddress.c_str(), &(destIp)) <= 0)
            {
                mLastError = WolClientError::ADDRESS_NOT_SUPPORTED;
            }

            uint8_t macAddr[6] = {};
            ULONG macAddrLen = 6;
            DWORD retValue = SendARP(destIp, 0, macAddr, &macAddrLen);

            if (retValue == NO_ERROR) 
            {
                char macAddressStr[18] = { 0 };

                snprintf(macAddressStr, sizeof(macAddressStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                            macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

                macAddress = macAddressStr;
            }
#else
            ifreq ifr {};
            int sock = socket(AF_INET, SOCK_DGRAM, 0);

            strncpy(ifr.ifr_name, ethDevice.c_str(), IFNAMSIZ);

            if (ioctl(sock, SIOCGIFHWADDR, &ifr) != -1) 
            {
                char macAddressStr[18] = { 0 };

                sprintf(macAddressStr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                    (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

                macAddress = macAddressStr;
            }
            close(sock);
#endif

            return macAddress;
        }

        int8_t WOL_Client::SendMagicPacket(const std::string& macAddress)
        {
#ifdef WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "Failed to initialize Winsock\n";
                return -1;
            }

            SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock == INVALID_SOCKET) 
            {
                std::cerr << "Failed to create socket\n";
                WSACleanup();
                return -1;
            }
#else
            int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (sock < 0) 
            {
                std::cerr << "Failed to create socket\n";
                return -1;
            }
#endif

            MagicPacket magicPacket(macAddress);

            // Set up the broadcast address
            sockaddr_in broadcastAddr{};
            memset(&broadcastAddr, 0, sizeof(broadcastAddr));
            broadcastAddr.sin_family = AF_INET;
            broadcastAddr.sin_port = htons(9);
            broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

            // Send the magic packet
            int bytesSent = sendto(sock, reinterpret_cast<const char*>(magicPacket.data.data()), magicPacket.data.size(), 0,
                reinterpret_cast<sockaddr*>(&broadcastAddr), sizeof(broadcastAddr));

            int rtn = 0;

            if (bytesSent <= 0) 
            {
                mLastError = WolClientError::SEND_MAGIC_PACKET_FAIL;
                rtn = -1;
            }

#ifdef WIN32
            closesocket(sock);
            WSACleanup();
#else
            close(sock);
#endif
            return rtn;
        }

        std::string WOL_Client::GetLastError()
        {
            if (mLastError == WolClientError::SEND_MAGIC_PACKET_FAIL)
            {
                return WolClientErrorMap[mLastError] + "\n" + mLastOSError;
            }

            return WolClientErrorMap[mLastError];
        }
    }
}