#ifndef TOOLBOX_NETWORK_TELNET_HPP
#define TOOLBOX_NETWORK_TELNET_HPP

/*
 * Toolbox/Network/Telnet.hpp
 *
 * A Telnet Server
 *
 * Currently just a stub awaiting code migration...
 */


/*****************************************************************************
 * Example program:


// Build with:
// g++ -std=c++14 -Wall -pedantic -o example
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/


#include <Toolbox/Network/Basic.hpp>


namespace Toolbox
{
	namespace Network
	{
		class TelnetSocket : public Socket
		{
		public:
			TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( TelnetSocket )

			virtual void HandleChar( unsigned char input )
			{
			}

			virtual void HandleLine( const std::string &input )
			{
			}

		protected:
			virtual void onClose()
			{
			}
		};


		class TelnetServer : public Server
		{
		protected:
			TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( TelnetSocket )

			virtual void onNewConnection( Socket::Ptr newSocket )
			{
			}
		};
	}
}


#endif // TOOLBOX_NETWORK_TELNET_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


