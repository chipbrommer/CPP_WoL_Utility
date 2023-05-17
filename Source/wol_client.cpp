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
            mBroadcastAddress = {};
        }

        WOL_Client::~WOL_Client()
        {

        }

        int8_t WOL_Client::Send(const std::string& macAddress)
        {
            std::vector<uint8_t> magicPacket;
            if (GenerateMagicPacket(macAddress, magicPacket) < 0)
            {
                return -1;
            }

            if (SendPacket(magicPacket) < 0)
            {
                return -1;
            }

            return 0;
        }

        std::string WOL_Client::GetMacAddressFromArpTable(const std::string& ipAddress)
        {
            std::string macAddress;

            // Open a socket to access ARP table
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd == -1) 
            {
                std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
                return macAddress;
            }

            // Get network interfaces
            struct ifaddrs* ifAddrList;
            if (getifaddrs(&ifAddrList) == -1) 
            {
                std::cerr << "Error getting network interfaces: " << strerror(errno) << std::endl;
                close(sockfd);
                return macAddress;
            }

            // Iterate through the network interfaces
            struct ifaddrs* ifAddr;
            for (ifAddr = ifAddrList; ifAddr != nullptr; ifAddr = ifAddr->ifa_next) 
            {
                if (ifAddr->ifa_addr == nullptr)
                {
                    continue;
                }

                // Check if it's an IPv4 interface and matches the broadcast address
                if (ifAddr->ifa_addr->sa_family == AF_INET) 
                {
                    struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(ifAddr->ifa_addr);

                    if (inet_ntoa(addr->sin_addr) == mBroadcastAddress->ipAddress) 
                    {
                        // Read ARP table entries
                        arpreq arp;
                        memset(&arp, 0, sizeof(arp));
                        arp.arp_pa.sa_family = AF_INET;
                        inet_pton(AF_INET, ipAddress.c_str(), &(arp.arp_pa.sa_data[2]));

                        if (ioctl(sockfd, SIOCGARP, &arp) == 0) 
                        {
                            std::stringstream ss;
                            ss << std::hex << std::setfill('0');

                            for (int i = 0; i < 6; ++i) 
                            {
                                ss << std::setw(2) << static_cast<int>(static_cast<uint8_t>(arp.arp_ha.sa_data[i]));
                                if (i < 5) ss << ":";
                            }

                            macAddress = ss.str();
                        }

                        break;
                    }
                }
            }

            freeifaddrs(ifAddrList);
            close(sockfd);
            return macAddress;
        }

        int8_t WOL_Client::GenerateMagicPacket(const std::string& macAddress, std::vector<uint8_t>& magicPacket)
        {
            std::stringstream ss(macAddress);
            std::vector<uint8_t> macBytes;

            // Parse the MAC address into individual bytes
            for (int i= 0; i < 6; ++i) 
            {
                unsigned int byte;
                ss >> std::hex >> byte;
                macBytes.push_back(static_cast<uint8_t>(byte));

                if (i < 5)
                {
                    ss.ignore(1, ':');
                }
            }

            // Build the magic packet
            magicPacket.clear();
            magicPacket.resize(6, 0xFF); // Add 6 bytes of 0xFF as the sync stream

            for (int i = 0; i < 16; ++i) 
            {
                magicPacket.insert(magicPacket.end(), macBytes.begin(), macBytes.end());
            }
        }

        int8_t WOL_Client::SendPacket(const std::vector<uint8_t>& packet)
        {
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd == -1) 
            {
                std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
                return -1;
            }

            int broadcastEnable = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) 
            {
                std::cerr << "Error setting socket options: " << strerror(errno) << std::endl;
                close(sockfd);
                return -1;
            }

            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(9); // Wake-on-LAN uses port 9

            if (inet_aton(mBroadcastAddress->ipAddress.c_str(), &addr.sin_addr) == 0) 
            {
                std::cerr << "Invalid broadcast address: " << mBroadcastAddress->ipAddress << std::endl;
                close(sockfd);
                return -1;
            }

            ssize_t bytesSent = sendto(sockfd, packet.data(), packet.size(), 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
            if (bytesSent == -1) 
            {
                std::cerr << "Error sending packet: " << strerror(errno) << std::endl;
                close(sockfd);
                return -1;
            }

            std::cout << "WoL packet sent successfully to MAC address: " << FormatMacAddress(packet) << std::endl;
            close(sockfd);
            return 0;
        }

        std::string WOL_Client::FormatMacAddress(const std::vector<uint8_t>& macBytes)
        {
            std::stringstream ss;
            ss << std::uppercase << std::setfill('0') << std::hex;

            for (size_t i = 0; i < macBytes.size(); ++i) 
            {
                ss << std::setw(2) << static_cast<int>(macBytes[i]);

                if (i < macBytes.size() - 1)
                {
                    ss << ":";
                }
            }
            return ss.str();
        }

    }
}