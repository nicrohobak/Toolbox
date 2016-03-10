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
#include <Toolbox/Network/Telnet.h>


namespace Toolbox
{
	namespace Network
	{
		class TelnetSocket : public Socket
		{
		public:
			TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( TelnetSocket )
			{
				TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onConnect",		&TelnetSocket::onConnect )
				TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onClose",		&TelnetSocket::onClose )
				TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onHandleChar",	&TelnetSocket::onHandleChar )
				TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onHandleLine",	&TelnetSocket::onHandleLine )
			}

			TOOLBOX_EVENT_HANDLER( onConnect )
			{
				std::cout << this << " -- TelnetSocket::onConnect()" <<  std::endl;

				// Use the << operator to add to the outgoing buffer (std::stringstream converts the stream to std::string)
				*this << endl;
				*this << "=======================================" << endl;
				*this << " Welcome to the Example Telnet Server!" << endl << endl;
				*this << "   (Any command typed will be echoed" << endl;
				*this << "    back.)" << endl << endl;
				*this << " (Type 'quit' to disconnect.)" << endl;
				*this << " (Type 'shutdown' to stop the server.)" << endl;
				*this << "=======================================" << endl << endl;

				// Send the outgoing buffer
				this->Flush();

				// Or write to the socket immediately with Write()
				// Prompt
				this->Write( ") " );
			}

			TOOLBOX_EVENT_HANDLER( onClose )
			{
				std::cout << this << " -- TelnetSocket::onClose()" <<  std::endl;
				Write( "\n\rServer) Bye bye!\n\r\n\r" );
			}

			TOOLBOX_EVENT_HANDLER( onHandleChar )
			{
				std::cout << this << " -- TelnetSocket::HandleChar(): " << eventData["input"].AsChar() << std::endl;
			}

			TOOLBOX_EVENT_HANDLER( onHandleLine )
			{
				std::string Line( eventData["input"].AsStr() );

				std::cout << this << " -- TelnetSocket::HandleLine(): " << Line << std::endl;

				if ( !Line.compare("quit") )
				{
					Close();
					return;
				}

				if ( !Line.compare("shutdown") )
				{
					if ( _Server )
					{
						std::cout << this << " -- TelnetSocket::onHandleLine(): Shutdown command received!" <<  std::endl;

						for ( auto s = _Server->begin(), s_end = _Server->end(); s != s_end; ++s )
						{
							// Give an extra newline to the ones who didn't dispatch the command
							if ( s->get() != this )
								*s->get() << endl;

							*s->get() << endl;
							*s->get() << "The server is shutting down...goodbye!" << endl << endl;
							(*s)->Flush();
						}

						_Server->Stop();
					}
					else
						throw std::runtime_error( "TelnetSocket::onHandleLine(): Attempted server shutdown with invalid _Server pointer." );

					return;
				}

				*this << endl;									// Layout/Spacing
				*this << "Server ) " << Line << endl << endl;	// Echo
				*this << ") ";									// Prompt
				Flush();										// Send
			}
		};


		class TelnetServer : public Server
		{
		public:
			TelnetServer( short int port = DEFAULT_PORT ):
				Server( port )
			{
			}

			TelnetServer( asio::io_service &io, short int port = DEFAULT_PORT ):
				Server( io, port )
			{
			}


		protected:
			TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( TelnetSocket )
		};
	}
}


#endif // TOOLBOX_NETWORK_TELNET_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


