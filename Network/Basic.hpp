#ifndef TOOLBOX_NETWORK_BASIC_HPP
#define TOOLBOX_NETWORK_BASIC_HPP

/*
 * Toolbox/Network/Basic.hpp
 *
 * Basic networking
 * Currently depends on ASIO (http://think-async.com/)
 */


/*****************************************************************************
 * Example program:


 class CustomSocket : public Toolbox::Network::Socket
{
public:
	TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( CustomSocket )
	{
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onHandleChar",	&CustomSocket::onHandleChar )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onHandleLine",	&CustomSocket::onHandleLine )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onClose",		&CustomSocket::onClose )
	}

	TOOLBOX_EVENT_HANDLER( onHandleChar )
	{
		std::cout << this << " -- CustomSocket::HandleChar(): " << eventData["input"].AsChar() << std::endl;
	}

	TOOLBOX_EVENT_HANDLER( onHandleLine )
	{
		std::string Line( eventData["input"].AsStr() );

		std::cout << this << " -- CustomSocket::HandleLine(): " << Line << std::endl;

		if ( !Line.compare("quit") )
		{
			Close();
			return;
		}

		if ( !Line.compare("shutdown") )
		{
			if ( _Server )
			{
				std::cout << this << " -- CustomSocket::onHandleLine(): Shutdown command received!" <<  std::endl;

				for ( auto s = _Server->begin(), s_end = _Server->end(); s != s_end; ++s )
					(*s)->Write( "\n\rThe server is shutting down...goodbye!\n\r\n\r" );

				_Server->Stop();
			}
			else
			{
				Write( "Failed to shutdown the server!" );
			}

			return;
		}

		// Echo
		Write( "\n\rServer) " );
		Write( Line );
		Write( "\n\r\n\r" );

		// Prompt
		Write( ") " );
	}

	TOOLBOX_EVENT_HANDLER( onClose )
	{
		std::cout << this << " -- CustomSocket::onClose()" <<  std::endl;
		Write( "\n\rServer) Bye bye!\n\r\n\r" );
	}
};


class CustomServer : public Toolbox::Network::Server
{
public:
	CustomServer( short int port = 9876 ):
		Toolbox::Network::Server( port )
	{
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onNewConnection",	&CustomServer::onNewConnection )
	}

protected:
	TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( CustomSocket )

	TOOLBOX_EVENT_HANDLER( onNewConnection )
	{
		CustomSocket *NewSocket = eventData["socket"].AsPtr< CustomSocket >();

		std::cout << "CustomServer::onNewConnection()" << std::endl;
		std::cout << "    Address: " << NewSocket << std::endl;

		NewSocket->Write( "\n\r=======================================\n\r" );
		NewSocket->Write( "  Welcome to the Example Echo Server!\n\r\n\r" );
		NewSocket->Write( "   (Any command typed will be echoed\n\r" );
		NewSocket->Write( "    back.)\n\r" );
		NewSocket->Write( " (Type 'quit' to disconnect.)\n\r" );
		NewSocket->Write( " (Type 'shutdown' to stop the server.)\n\r" );
		NewSocket->Write( "=======================================\n\r\n\r" );

		// Prompt
		NewSocket->Write( ") " );
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

#include <Toolbox/Event.hpp>
#include <Toolbox/Memory.hpp>


namespace Toolbox
{
	namespace Network
	{
		//
		// Constants and Typedefs
		//
		const short int DEFAULT_PORT			= 9876;
		typedef asio::ip::tcp::socket			CoreSocket;
		typedef std::shared_ptr< CoreSocket >	CoreSocket_Ptr;


		//
		// Macro Definitions for simplified inheritence
		//
		// Constructor definition for custom Toolbox::Network::Socket classes
		#define TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_PARAMS	Toolbox::Network::Server &server, Toolbox::Network::CoreSocket_Ptr socket
		#define TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_ARGS		server, socket

		// Constructor definition for custom Toolbox::Network::Socket classes
		#define TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( tCustomSocket )						\
				tCustomSocket( TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_PARAMS ):				\
					Toolbox::Network::Socket( TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_ARGS )

		// Constructor definition for custom Toolbox::Network::Server classes
		#define TOOLBOX_NETWORK_SERVER_CONSTRUCTOR_PARAMS		short int port = Toolbox::Network::DEFAULT_PORT
		#define TOOLBOX_NETWORK_SERVER_CONSTRUCTOR_ARGS			port

		// createSocket() definition for custom Toolbox::Network::Server classes
		#define TOOLBOX_NETWORK_SERVER_SOCKET_CONSTRUCTOR_ARGS	*this, _NewSocket

		#define TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( tCustomSocket )											\
				virtual Toolbox::Network::Socket::Ptr createSocket()											\
				{																								\
					return std::make_shared< tCustomSocket >( TOOLBOX_NETWORK_SERVER_SOCKET_CONSTRUCTOR_ARGS );	\
				}


		//
		// Forward Declarations
		//
		class Server;


		// Locally emits (but doesn't handle) the following events:
		//
		// - onHandleChar	- Called for each byte (char) received
		// 					  ["input"]	- The input byte received (as int)
		//
		// - onHandleLine	- Called each time we receive a line terminator [\n,\r,\0,\003,\004] AND our line buffer is not empty
		// 					  ["input"]	- The input line received, as std::string
		//
		// - onClose		- Called when the socket is closed, prior to disconnecting (Write() is still valid here)
		//
		class Socket : public std::enable_shared_from_this< Socket >,
					   public Event::Listener
		{
		public:
			TOOLBOX_MEMORY_POINTERS_AND_LISTS( Socket )
			constexpr static size_t BUFFER_SIZE = 1;

		protected:
			// Called for every incoming byte
			// If not using ASCII network data, this can be overridden to change the basic behavior
			virtual void readChar( unsigned char input )
			{
				Event::Data EventData;

				EventData["input"] = input;
				this->HandleEvent( "onHandleChar", EventData );	// "Local" events can just call the handler directly

				// Abandon ship if we get closed as we handle the above event
				if ( !_Server )
					return;

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
							EventData["input"] = _LineBuf;
							this->HandleEvent( "onHandleLine", EventData );	// "Local" events can just call the handler directly
							_LineBuf.clear();
						}

						break;
					}

					default:
						_LineBuf.push_back( input );
						break;
				}
			}

		public:
			Socket( TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_PARAMS ):
				_Socket( socket ),
				_Server( &server )
			{
				resetCharBuf();
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
			void resetCharBuf()
			{
				memset( _CharBuf, BUFFER_SIZE + 1, 0 );
			}

			void doRead();

		protected:
			CoreSocket_Ptr	_Socket;
			unsigned char	_CharBuf[ BUFFER_SIZE + 1 ];
			std::string		_LineBuf;

			Server *		_Server;

			friend class Server;
		};


		//
		// Should aways be created as a Server::Ptr so children sockets can maintain weak pointers to their parent server
		//
		// Locally emits (but doesn't handle) the following events:
		// - onNewConnection	-- Called upon each new connection
		// 						   ["socket"]	- Pointer to newly created socket
		class Server : public std::enable_shared_from_this< Server >,
					   public Event::Listener
		{
		public:
			TOOLBOX_MEMORY_POINTERS_AND_LISTS( Server )
			typedef Socket::List::iterator					iterator;
			typedef Socket::List::const_iterator			const_iterator;
			typedef Socket::List::reverse_iterator			reverse_iterator;
			typedef Socket::List::const_reverse_iterator	const_reverse_iterator;

		protected:
			// You'll want to define this in your own derived class, providing your own custom socket class instead
			TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( Socket )

		public:
			Server( short int port = DEFAULT_PORT ):
				_IOService( NULL ),
				_ManageIOService( true ),
				_Port( port ),
				_Acceptor( NULL )
			{
			}

			Server( asio::io_service &io, short int port = DEFAULT_PORT ):
				_IOService( &io ),
				_ManageIOService( false ),
				_Port( port ),
				_Acceptor( NULL )
			{
			}

			// Copy constructor
			Server( Server &s ):
				_IOService( NULL ),
				_ManageIOService( true ),
				_Port( s._Port ),
				_Acceptor( NULL )
			{
			}

			// Move constructor
			Server( Server &&s ):
				_IOService( s._IOService ),
				_ManageIOService( s._ManageIOService ),
				_Port( s._Port ),
				_Acceptor( s._Acceptor ),
				_Sockets( s._Sockets )
			{
			}

			virtual ~Server()
			{
				// Make sure to cleanup the sockets before we cleanup the IOService, etc.
				_Sockets.clear();

				if ( _Acceptor )
					delete _Acceptor;

				if ( _ManageIOService && _IOService )
					delete _IOService;
			}

			// Copy assignment
			Server &operator=( Server &rhs )
			{
				_Port	= rhs._Port;
			}

			// Move assignment
			Server &operator=( Server &&rhs )
			{
				_IOService			= rhs._IOService;
				_ManageIOService	= rhs._ManageIOService;
				_Port				= rhs._Port;
				_Acceptor			= rhs._Acceptor;
				_NewSocket			= rhs._NewSocket;
				_Sockets			= rhs._Sockets;
			}

			iterator begin()
			{
				return _Sockets.begin();
			}

			const_iterator begin() const
			{
				return _Sockets.begin();
			}

			reverse_iterator rbegin()
			{
				return _Sockets.rbegin();
			}

			const_reverse_iterator rbegin() const
			{
				return _Sockets.rbegin();
			}

			iterator end()
			{
				return _Sockets.end();
			}

			const_iterator end() const
			{
				return _Sockets.end();
			}

			reverse_iterator rend()
			{
				return _Sockets.rend();
			}

			const_reverse_iterator rend() const
			{
				return _Sockets.rend();
			}

			asio::io_service &IO()
			{
				return *_IOService;
			}

			int Port() const
			{
				return _Port;
			}

			void Run()
			{
				doAccept();
				_IOService->run();
			}

			void Stop()
			{
				_IOService->stop();
			}

		protected:
			void doAccept()
			{
				if ( !_IOService )
					_ManageIOService = true;

				if ( _ManageIOService && !_IOService )
					_IOService = new asio::io_service;

				if ( !_Acceptor )
					_Acceptor = new asio::ip::tcp::acceptor( *_IOService, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), _Port) );

				if ( !_NewSocket )
					_NewSocket = std::make_shared< CoreSocket >( *_IOService );

				_Acceptor->async_accept( *_NewSocket,
										[this]( std::error_code ec )
										{
											if ( !ec )
											{
												Socket::Ptr NewSocket = this->createSocket();
												_Sockets.push_back( NewSocket );

												Event::Data EventData;
												EventData["socket"].AssignPtr( NewSocket.get() );		// Use AssignPtr or you may get bool!
												this->HandleEvent( "onNewConnection", EventData );

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
			asio::io_service *			_IOService;
			bool						_ManageIOService;

			int							_Port;

			// Have to put this on the stack for copyable purposes (so we can use std::bind() on member functions, for events)
			asio::ip::tcp::acceptor *	_Acceptor;

			CoreSocket_Ptr				_NewSocket;
			Socket::List				_Sockets;

			friend class Socket;
		};


		//
		// Socket functions that require a fully-defined Server
		//
		void Socket::Close()
		{
			// "Local" events can just call the handler directly
			this->HandleEvent( "onClose" );

			if ( _Server )
				_Server->removeSocket( shared_from_this() );
	
			_Server = NULL;
		}

		void Socket::doRead()
		{
			resetCharBuf();
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


#endif // TOOLBOX_NETWORK_BASIC_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


