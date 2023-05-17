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

        }

        WOL_Client::~WOL_Client()
        {

        }

        int8_t WOL_Client::Send(const std::string& macAddress)
        {
            return -1;
        }

        std::string WOL_Client::GetMacAddressFromArpTable(const std::string& ipAddress)
        {
            return "";
        }

        void WOL_Client::GenerateMagicPacket(const std::string& macAddress, std::vector<uint8_t>& magicPacket)
        {

        }

        void WOL_Client::SendPacket(const std::vector<uint8_t>& packet)
        {

        }

        std::string WOL_Client::FormatMacAddress(const std::vector<uint8_t>& macBytes)
        {
            return "";
        }

    }
}