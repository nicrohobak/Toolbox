#ifndef TOOLBOX_NETWORK_HPP
#define TOOLBOX_NETWORK_HPP

/*
 * Toolbox/Network.hpp
 *
 * Basic networking, expandable via plugins
 * Currently depends on ASIO (http://think-async.com/)
 */


/*****************************************************************************
 * Example program:


 class CustomSocket : public Toolbox::Network::Socket
{
public:
	TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( CustomSocket )

	virtual void HandleChar( unsigned char input )
	{
		std::cout << "CustomSocket::HandleChar(): " << input << std::endl;
	}

	virtual void HandleLine( const std::string &input )
	{
		if ( !input.compare("quit") )
		{
			Close();
			return;
		}

		// Echo
		Write( "\n\rServer) " );
		Write( input );
		Write( "\n\r\n\r" );

		// Prompt
		Write( ") " );
	}

protected:
	virtual void onClose()
	{
		std::cout << "CustomSocket::onClose()" << std::endl;
		Write( "\n\rServer) Bye bye!\n\r\n\r" );
	}
};


class CustomServer : public Toolbox::Network::Server
{
protected:
	TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( CustomSocket )

	virtual void onNewConnection( Toolbox::Network::Socket::Ptr newSocket )
	{
		std::cout << "CustomServer::onNewConnection()" << std::endl;
		std::cout << "    Address: " << (void *)(newSocket.get()) << std::endl;

		newSocket->Write( "\n\r=====================================\n\r" );
		newSocket->Write( " Welcome to the Example Echo Server!\n\r\n\r" );
		newSocket->Write( " (Any command typed will be echoed\n\r" );
		newSocket->Write( "  back.)\n\r" );
		newSocket->Write( " (Type 'quit' to disconnect.)\n\r" );
		newSocket->Write( "=====================================\n\r\n\r" );

		// Prompt
		newSocket->Write( ") " );
	}
};


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

    try
    {
		CustomServer Server;
		Server.Run();
    }
    catch ( std::exception &ex )
    {
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		ReturnCode = 1;
    }

    return ReturnCode;
}


// Build with:
// g++ -std=c++14 -Wall -pedantic -o example
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <asio.hpp>

#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	namespace Network
	{
		//
		// Typedefs
		//
		typedef asio::ip::tcp::socket			CoreSocket;
		typedef std::shared_ptr< CoreSocket >	CoreSocket_Ptr;


		//
		// Macro Definitions for simplified inheritence
		//
		#define TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_PARAMS	Toolbox::Network::Server &server, Toolbox::Network::CoreSocket_Ptr socket
		#define TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_ARGS		server, socket

		// Constructor definition for custom Toolbox::Network::Socket classes
		#define TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( tCustomSocket )						\
				tCustomSocket( TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_PARAMS ):				\
					Toolbox::Network::Socket( TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_ARGS )	\
				{																		\
				}

		#define TOOLBOX_NETWORK_SERVER_SOCKET_CONSTRUCTOR_ARGS	*this, _NewSocket

		// createSocket() definition for custom Toolbox::Network::Server classes
		#define TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( tCustomSocket )											\
				virtual Toolbox::Network::Socket::Ptr createSocket()											\
				{																								\
					return std::make_shared< tCustomSocket >( TOOLBOX_NETWORK_SERVER_SOCKET_CONSTRUCTOR_ARGS );	\
				}


		//
		// Forward Declarations
		//
		class Server;


		class Socket : public std::enable_shared_from_this< Socket >
		{
		public:
			TOOLBOX_MEMORY_POINTERS_AND_LISTS( Socket )
			constexpr static size_t BUFFER_SIZE = 1;

		public:
			virtual void HandleChar( unsigned char input )
			{
				// Called for each byte received (even when HandleLine() is called)
			}

			virtual void HandleLine( const std::string &input )
			{
				// Called each time we receive [\n,\r,\0,\3,\4] AND our line buffer is not empty
			}

		protected:
			virtual void onClose()
			{
				// Called when the socket is closed, prior to disconnecting (Write() is still valid here)
			}

		public:
			Socket( TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_PARAMS ):
				_Socket( socket ),
				_Server( &server )
			{
				ResetCharBuf();
			}

			virtual ~Socket()
			{
			}

			void Write( const std::string &msg )
			{
				asio::async_write( *_Socket,
									asio::buffer(msg.c_str(), msg.length()),
									[this]( std::error_code ec, size_t length )
									{
										if ( !ec )
										{
											// onWrite
										}
									} );
			}

			void Close();

		protected:
			void ResetCharBuf()
			{
				memset( _CharBuf, BUFFER_SIZE + 1, 0 );
			}

			void doRead();

			void readChar( unsigned char input )
			{
				switch ( input )
				{
					case 0:		// NULL
						break;

					case '\n':	// Newline
					case '\r':	// Carriage Return
					case 3:		// End of Text
					case 4:		// End of Line
					{
						if ( !_LineBuf.empty() )
						{
							this->HandleLine( _LineBuf );
							_LineBuf.clear();
						}

						break;
					}

					default:
						_LineBuf.push_back( input );
						break;
				}

				this->HandleChar( input );
			}

		protected:
			CoreSocket_Ptr	_Socket;
			unsigned char	_CharBuf[ BUFFER_SIZE + 1 ];
			std::string		_LineBuf;

			Server *		_Server;

			friend class Server;
		};


		class Server
		{
		public:
			TOOLBOX_MEMORY_POINTERS_AND_LISTS( Server )

		protected:
			// You'll want to define this in your own derived class, providing your own custom socket class instead
			TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( Socket )

			virtual void onNewConnection( Socket::Ptr newSocket )
			{
				// Called upon each new connection
			}

		public:
			Server( short int port = 9876 ):
				_IOService( new asio::io_service ),
				_ManageIOService( true ),
				_Acceptor( *_IOService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port) ),
				_NewSocket( std::make_shared< CoreSocket >(*_IOService) )
			{
				doAccept();
			}

			Server( asio::io_service &io, short int port = 9876 ):
				_IOService( &io ),
				_ManageIOService( false ),
				_Acceptor( *_IOService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port) ),
				_NewSocket( std::make_shared< CoreSocket >(*_IOService) )
			{
				doAccept();
			}

			~Server()
			{
				if ( _ManageIOService )
					delete _IOService;
			}

			asio::io_service &IO()
			{
				return *_IOService;
			}

			void Run()
			{
				_IOService->run();
			}

		protected:
			void doAccept()
			{
				_Acceptor.async_accept( *_NewSocket,
										[this]( std::error_code ec )
										{
											if ( !ec )
											{
												Socket::Ptr NewSocket = createSocket();
												_Sockets.push_back( NewSocket );
												this->onNewConnection( NewSocket );
												NewSocket->doRead();

												_NewSocket = std::make_shared< CoreSocket >( *_IOService );
											}

											doAccept();
										} );
			}

			void removeSocket( Socket::Ptr socket )
			{
				if ( !socket )
					return;

				for ( auto s = _Sockets.begin(), s_end = _Sockets.end(); s != s_end; ++s )
				{
					if ( *s == socket )
					{
						_Sockets.erase( s );
						return;
					}
				}
			}

		protected:
			asio::io_service *		_IOService;
			bool					_ManageIOService;

			asio::ip::tcp::acceptor	_Acceptor;
			CoreSocket_Ptr			_NewSocket;

			Socket::List			_Sockets;

			friend class Socket;
		};


		//
		// Socket functions that require a fully-defined Server
		//
		void Socket::Close()
		{
			this->onClose();
	
			if ( _Server )
				_Server->removeSocket( shared_from_this() );
	
			_Server = NULL;
		}

		void Socket::doRead()
		{
			ResetCharBuf();
			_Socket->async_read_some( asio::buffer(_CharBuf, BUFFER_SIZE),
									[this]( std::error_code ec, size_t length )
									{
										if ( ec )
										{
											Close();
											return;
										}
										
										this->readChar( _CharBuf[0] );

										// Validate that we still have a server so we don't try a new read after being closed
										if ( _Server )
											doRead();
									} );
		}

	}
}


#endif // TOOLBOX_NETWORK_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


