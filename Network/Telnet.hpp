#ifndef TOOLBOX_NETWORK_TELNET_HPP
#define TOOLBOX_NETWORK_TELNET_HPP

/*
 * Toolbox/Network/Telnet.hpp
 *
 * A Customizable Telnet Server
 */


/*****************************************************************************
 * Example program:


//
// Define a few macros to make things easier to type/read
//
#define CMD_FUNC( tCmd )					TOOLBOX_EVENT_HANDLER( tCmd )

#define SET_MEMBER_CMD_FUNC( tCmd, Func )	\
		_Commands.SetEventHandler( tCmd, std::bind(Func, TOOLBOX_EVENT_MEMBER_BIND_PARAM) );


class MySocket : public Toolbox::Network::TelnetSocket
{
public:
    TOOLBOX_PARENT( Toolbox::Network::TelnetSocket )		// Requirement of TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR

	TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR( MySocket )
	{
		// Set Toolbox::Network::TelnetSocket event handlers
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onWindowSize",			&MySocket::onWindowSize )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onEnableTelnetOption",	&MySocket::onEnableTelnetOption )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onDisableTelnetOption",	&MySocket::onDisableTelnetOption )

		// Set Toolbox::Network::Socket event handlers
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onConnect",		&MySocket::onConnect )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onClose",		&MySocket::onClose )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onHandleChar",	&MySocket::onHandleChar )
		TOOLBOX_EVENT_SET_MEMBER_HANDLER( "onHandleLine",	&MySocket::onHandleLine )

		// Add telnet commands
		SET_MEMBER_CMD_FUNC( "?",			&MySocket::Help );
		SET_MEMBER_CMD_FUNC( "compact",		&MySocket::Compact );
		SET_MEMBER_CMD_FUNC( "clear",		&MySocket::CLS );
		SET_MEMBER_CMD_FUNC( "cls",			&MySocket::CLS );
		SET_MEMBER_CMD_FUNC( "help",		&MySocket::Help );
		SET_MEMBER_CMD_FUNC( "prompt",		&MySocket::Prompt );
		SET_MEMBER_CMD_FUNC( "quit",		&MySocket::Quit );
		SET_MEMBER_CMD_FUNC( "show",		&MySocket::Show );
		SET_MEMBER_CMD_FUNC( "shutdown",	&MySocket::Shutdown );
		//SET_MEMBER_CMD_FUNC( "termtype",	&MySocket::Termtype );
		SET_MEMBER_CMD_FUNC( "windowsize",	&MySocket::Windowsize );
		SET_MEMBER_CMD_FUNC( "who",			&MySocket::Who );
	}

	//
	// TelnetSocket Event Handlers
	//
	TOOLBOX_EVENT_HANDLER( onWindowSize )
	{
		*this << "Server) Window size set to: " << _WindowSize.Width << "," << _WindowSize.Height << endl;
	}

	TOOLBOX_EVENT_HANDLER( onEnableTelnetOption )
	{
		*this << "Server) Telnet option enabled: " << Toolbox::Network::Telnet::OptionName( (Toolbox::Network::Telnet::Option)eventData["option"].AsInt() ) << endl;
	}

	TOOLBOX_EVENT_HANDLER( onDisableTelnetOption )
	{
		*this << "Server) Telnet option disabled: " << Toolbox::Network::Telnet::OptionName( (Toolbox::Network::Telnet::Option)eventData["option"].AsInt() ) << endl;
	}

	//
	// Socket Event Handlers
	//
	TOOLBOX_EVENT_HANDLER( onConnect )
	{
		std::list< std::string > CurrentClients;

		// Inform all connected clients
		for ( auto s = _Server->begin(), s_end = _Server->end(); s != s_end; ++s )
		{
			// Skip ourselves
			if ( s->get() == this )
				continue;

			// Currently using this pointer addresses as chat IDs
			*s->get() << "Server) " << this << " has connected." << endl;

			std::stringstream ID("");
			ID << this;
			CurrentClients.push_back( ID.str() );
		}

		// Use the << operator to add to the outgoing buffer (std::stringstream converts the stream to std::string)
		*this << "=======================================" << endl;
		*this << " Welcome to the Example Telnet Server!" << endl << endl;
		*this << "  Anything typed will be sent to all" << endl;
		*this << "     currently connected clients." << endl << endl;
		*this << " (Type '/?' for help.)" << endl;
		*this << "=======================================" << endl;
	}

	TOOLBOX_EVENT_HANDLER( onClose )
	{
		Write( "\n\rServer) Bye bye!\n\r\n\r" );
	}

	TOOLBOX_EVENT_HANDLER( onHandleChar )
	{
		//std::cout << this << " -- MySocket::HandleChar(): " << eventData["input"].AsChar() << std::endl;
	}

	TOOLBOX_EVENT_HANDLER( onHandleLine )
	{
		std::string Line( eventData["input"].AsStr() );

		// Parse slash commands
		if ( *Line.begin() == '/' )
		{
			// Delete the slash from the command
			Line.erase( Line.begin() );

			Toolbox::Event::Data EventData;
			std::string Word, Command;

			std::stringstream Chopper( Line );
			Chopper >> Word;
			EventData["command"] = Command = Word;

			// Trim the command off of the args
			const char WhitespaceChars[] = " \t\n\r";
			EventData["args"] = Line.substr( Line.find_first_of(WhitespaceChars) + 1 );

			// Pull each separate argument apart too -- Starting count from 1 to avoid arg0
			for ( int i = 1; ; ++i )
			{
				Word.clear();
				std::stringstream CurLabel("");
				CurLabel << "arg" << i;
				Chopper >> Word;

				if ( Word.empty() )
					break;

				EventData[ CurLabel.str() ] = Word;
			}

			// And pass it to our makeshift command parser
			if ( !_Commands.HandleEvent(Command, EventData) )
				*this << "Server) *** Command not found: /" << Line << endl;
		}
		else
		{
			// Send to all connected clients
			for ( auto s = _Server->begin(), s_end = _Server->end(); s != s_end; ++s )
			{
				// Skip ourselves
				if ( s->get() == this )
					continue;

				// Currently using this pointer addresses as chat IDs
				*s->get() << this << " ) " << Line << endl;
			}

			*this << "*" << this << " ) " << Line << endl;
		}
	}

	//
	// Client Commands
	//
	CMD_FUNC( Compact )
	{
		*this << "Compact mode: ";

		if ( this->CompactMode() )
			*this << "Disabled." << endl;
		else
			*this << "Enabled." << endl;

		this->CompactMode( !_CompactMode );
	}

	CMD_FUNC( CLS )
	{
		*this << Toolbox::Network::VT100::Cmd( Toolbox::Network::VT100::Cmd_EraseDisplay );
		*this << Toolbox::Network::VT100::Cmd( Toolbox::Network::VT100::Cmd_SetCursorPos, 0, 0 );
	}

	CMD_FUNC( Help )
	{
		*this << "Available slash commands:" << endl;

		for ( auto c = _Commands.begin(), c_end = _Commands.end(); c != c_end; ++c )
			*this << "\t" << c->first << endl;
	}

	CMD_FUNC( Prompt )
	{
		_Prompt = eventData["args"].AsStr();

		*this << "Prompt set to: " << _Prompt << endl;
	}

	CMD_FUNC( Quit )
	{
		Close();
	}

	CMD_FUNC( Show )
	{
		*this << "Current client options:" << endl;
		*this << "  - Window Size" << endl;
		*this << "    - Width:  " << _WindowSize.Width << endl;
		*this << "    - Height: " << _WindowSize.Height << endl;
	}

	CMD_FUNC( Shutdown )
	{
		if ( _Server )
		{
			// NOTE: Using streams here (during server shutdown) won't work because they add to the output buffer...but the output buffer is flushed with the help of the server.
			//       The current implementation includes an automatic prompt with streams too...something we don't necessarily want to display during a shutdown message.  Write() gets around this issue too.
			std::stringstream Msg("");
			Msg << endl << "The server is shutting down...goodbye!" << endl;

			for ( auto s = _Server->begin(), s_end = _Server->end(); s != s_end; ++s )
			{
				// Give an extra newline to the ones who didn't dispatch the command
				if ( s->get() != this )
					(*s)->Write( endl );

				(*s)->Write( Msg.str() );
			}

			_Server->Stop();
		}
		else
			throw std::runtime_error( "MySocket::Shutdown(): Attempted server shutdown with invalid _Server pointer." );
	}

	CMD_FUNC( Termtype )
	{
		// TODO: Set the term type
	}

	CMD_FUNC( Windowsize )
	{
		auto Width = eventData.Find( "arg1" );
		auto Height = eventData.Find( "arg2" );

		if ( Width == eventData.end() || Height == eventData.end() )
		{
			RequestOpt( Toolbox::Network::Telnet::Opt_WindowSize );
			*this << "Server) Attempting to query your client for its current window size..."; 
			return;
		}

		_WindowSize.Width = Width->second.AsUShort();
		_WindowSize.Height = Height->second.AsUShort();

		*this << "Server) Window size set to: " << _WindowSize.Width << "," << _WindowSize.Height;
	}

	CMD_FUNC( Who )
	{
		std::stringstream ID("");
		std::list< std::string > Clients;

		// Inform all connected clients
		for ( auto s = _Server->begin(), s_end = _Server->end(); s != s_end; ++s )
		{
			ID.str("");
			ID << s->get();

			// Skip ourselves
			if ( s->get() == this )
				ID << " (You)";

			Clients.push_back( ID.str() );
		}

		*this << "Active clients:" << endl;

		for ( auto c = Clients.begin(), c_end = Clients.end(); c != c_end; ++c )
			*this << "\t" << *c << endl;
	}

protected:
	Toolbox::Event::Listener	_Commands;				// Reuse Toolbox::Event to help with command handling
};


class MyServer : public Toolbox::Network::TelnetServer
{
public:
	MyServer( short int port = 9876 ):
		Toolbox::Network::TelnetServer( port )
	{
	}

protected:
	TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( MySocket )
};


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

    try
    {
		MyServer Server;
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

#include <bitset>

#include <boost/date_time/posix_time/posix_time.hpp>	// It would be nice to elimiate this dependency...

#include <Toolbox/Network/Basic.hpp>
#include <Toolbox/Network/Telnet.h>
#include <Toolbox/Network/VT100.h>


namespace Toolbox
{
	namespace Network
	{
		//
		// Definitions
		//
		typedef std::bitset< Telnet::Option::Opt_MAX >		tTelnetOptions;


		//
		// Locally emits (but doesn't handle) the following events:
		//
		// - onWindowSize			- Called when the window size is renegotiated
		// 							  ["prev_width"] - (short int)
		// 							  ["prev_height"] - (short int)
		//
		// - onEnableTelnetOption	- Called when a telnet option is successfully enabled
		// 							  ["option"] - (enum) Toolbox::Network::Telnet::Option
		//
		// - onDisableTelnetOption	- Called when a telnet option is successfully disabled
		// 							  ["option"] - (enum) Toolbox::Network::Telnet::Option
		//
		class TelnetSocket : public Socket
		{
		public:
			typedef Socket		tParent;				// Requirement of TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR

			// Internal state for telnet protocol handling
			enum Readmode
			{
				Readmode_FIRST,

				Readmode_Normal = Readmode_FIRST,
				Readmode_Newline,
				Readmode_Cmd,
				Readmode_Opt,
				Readmode_Subneg,
				Readmode_Will_Do,
				Readmode_Wont_Dont,

				Readmode_MAX,

				Readmode_DEFAULT = Readmode_Normal
			};

		public:
			TelnetSocket():
				_Readmode( Readmode_DEFAULT ),
				_Prompt( "> " ),
				_CompactMode( false )
			{
				// Set default telnet options
				setDefaultOptions();
			}

			TelnetSocket( const std::string &host, const std::string &port ):
				_Readmode( Readmode_DEFAULT ),
				_Prompt( "> " ),
				_CompactMode( false ),
				Socket( host, port )
			{
				// Set default telnet options
				setDefaultOptions();
			}

			TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_START_INIT( TelnetSocket )
				_Readmode( Readmode_DEFAULT ),
				_Prompt( "> " ),
				_CompactMode( false ),
			TOOLBOX_NETWORK_SOCKET_CONSTRUCTOR_END_INIT
			{
				// Set default telnet options
				setDefaultOptions();

				// Ask the client for its current window size
				RequestOpt( Telnet::Opt_WindowSize );
			}

			virtual ~TelnetSocket()
			{
			}

			bool CompactMode() const
			{
				return _CompactMode;
			}

			void CompactMode( bool compact )
			{
				_CompactMode = compact;
			}

			unsigned short Width() const
			{
				return _WindowSize.Width;
			}

			unsigned short Height() const
			{
				return _WindowSize.Height;
			}

			const std::string &Prompt() const
			{
				return _Prompt;
			}

			virtual void SetPrompt( const std::string &prompt )
			{
				_Prompt = prompt;
			}

			// Makes a request to the client to enable an option
			void RequestOpt( Telnet::Option opt );

			bool IsOptEnabled( Telnet::Option opt )
			{
				return _Options[ opt ];
			}

			virtual void Flush()
			{
				if ( _SendBuf.empty() )
					return;

				// Start us off on a fresh line each time
				this->Write( endl );

				if ( !this->CompactMode() )
					*this << endl << _Prompt;
				else
					*this << _Prompt;

				tParent::Flush();
			}

		protected:
			tTelnetOptions	_Options;				// Current telnet option status

			Readmode		_Readmode;				// Telnet-handler state
			Telnet::Option	_CurOpt;				// Telnet-handler current telnet option
			std::string		_CurOptData;			// Telnet-handler current telnet option data
			tTelnetOptions	_OutstandingQueries;	// Allows us to keep track of queries we've sent the client so we to properly handle the response (instead of treating it as a client request)

			std::string		_Prompt;

			bool			_CompactMode;

			struct WindowSize
			{
				WindowSize():
					Width(80),
					Height(24)
				{
				}

				unsigned short int 	Width;
				unsigned short int 	Height;
			}						_WindowSize;

		protected:
			// Overriding this lets us interject telnet handling before onHandleChar and onHandleLine are triggered so they can still be used as-expected
			// Implemented below the TelnetServer definition
			virtual void readChar( unsigned char input );

			// Returns 'true' if setting the option was successful
			bool setOptEnabled( Telnet::Option opt, bool enabled = true );

			void setDefaultOptions()
			{
				_Options[ Telnet::Opt_Echo ]			= true;
				_Options[ Telnet::Opt_SuppressGoAhead ]	= false;
				_Options[ Telnet::Opt_Status ]			= false;
				_Options[ Telnet::Opt_TimingMark ]		= false;
				_Options[ Telnet::Opt_TermType ]		= false;
				_Options[ Telnet::Opt_WindowSize ]		= true;
				_Options[ Telnet::Opt_TermSpeed ]		= false;
				_Options[ Telnet::Opt_RemoteFlowCtrl ]	= false;
				_Options[ Telnet::Opt_LineMode ]		= true;
				_Options[ Telnet::Opt_EnvVars ]			= false;
			}

			void parseChar( unsigned char ch )
			{
				Event::Data EventData;

				switch ( ch )
				{
					case '\n':		// Newline
					case '\r':		// Carriage Return
					case '\003':	// End of Line
					case '\004':	// End of Transmission
					{
						if ( !_LineBuf.empty() )
						{
							_Readmode = Readmode_Newline;

							EventData["input"] = _LineBuf;
							HandleEvent( "onHandleLine", EventData );

							_LineBuf.clear();
						}
						break;
					}

					// Backspace
					case 127:
					{
						Write( "\b \b" );

						if ( _LineBuf.size() > 0 )
							_LineBuf.pop_back();

						break;
					}

					default:
					{
						// Echo
						if ( IsOptEnabled(Telnet::Opt_Echo) )
						{
							std::stringstream Char;
							Char << ch;
							Write( Char.str() );
						}

						_LineBuf.push_back( ch );
						EventData["input"] = ch;
						HandleEvent( "onHandleChar", EventData );
						break;
					}
				}
			}

			void handleTelnetOption()
			{
				switch ( _CurOpt )
				{
					case Telnet::Opt_TermType:
					{
						// TODO: Implement me
						break;
					}

					case Telnet::Opt_WindowSize:
					{
						// Get the hight and low bits for width and height
						unsigned char Width_H  = _CurOptData.c_str()[0];
						unsigned char Width_L  = _CurOptData.c_str()[1];
						unsigned char Height_H = _CurOptData.c_str()[2];
						unsigned char Height_L = _CurOptData.c_str()[3];

						Event::Data EventData;
						EventData["prev_width"]  = _WindowSize.Width;
						EventData["prev_height"] = _WindowSize.Height;

						if ( Width_H != 0 )
							_WindowSize.Width = ((unsigned short)Width_H + 255) + (unsigned short)Width_L;
						else
							_WindowSize.Width = (unsigned short)Width_L;

						if ( Height_H != 0 )
							_WindowSize.Height = ((unsigned short)Height_H + 255) + (unsigned short)Height_L;
						else
							_WindowSize.Height = (unsigned short)Height_L;

						HandleEvent( "onWindowSize", EventData );
						break;
					}

					case Telnet::Opt_LineMode:
					{
						// TODO: Implement me
						break;
					}

					default:
						// Invalid telnet option
						break;
				}
			}
		};


		class TelnetServer : public Server
		{
		public:
			typedef asio::deadline_timer		Timer;
			typedef std::shared_ptr< Timer >	Timer_Ptr;

			constexpr static unsigned int DEFAULT_OUTPUT_PULSE_DELAY = 100;

		public:
			TelnetServer( short int port = DEFAULT_PORT ):
				_OutputPulseDelay( DEFAULT_OUTPUT_PULSE_DELAY ),
				Server( port )
			{
				setCapabilities();
			}

			TelnetServer( asio::io_service &io, short int port = DEFAULT_PORT ):
				_OutputPulseDelay( DEFAULT_OUTPUT_PULSE_DELAY ),
				Server( io, port )
			{
				setCapabilities();
			}

			virtual ~TelnetServer()
			{
			}

			bool IsOptEnabled( Telnet::Option opt )
			{
				return _Options[ opt ];
			}

			void SetOptEnabled( Telnet::Option opt, bool enabled = true )
			{
				_Options[ opt ] = enabled;
			}

			// Sets the delay in milliseconds
			void SetOutputPulseDelay( unsigned int delay )
			{
				_OutputPulseDelay = delay;

				if ( _OutputTimer )
				{
					_OutputTimer->expires_from_now( boost::posix_time::milliseconds(_OutputPulseDelay) );
					_OutputTimer->async_wait( std::bind(&TelnetServer::handleOutputPulse, this) );
				}
			}

		protected:
			tTelnetOptions			_Options;			// Available telnet options

			unsigned int			_OutputPulseDelay;	// In milliseconds
			Timer_Ptr				_OutputTimer;		// Client output buffers are automatically flushed at regular intervals

		protected:
			TOOLBOX_NETWORK_SERVER_CREATE_SOCKET( TelnetSocket )

			virtual void doAccept()
			{
				// Make sure our io_service exists...
				Server::doAccept();

				// So we can add our timer to it
				_OutputTimer = Timer_Ptr( new Timer(*_IOService) );
				SetOutputPulseDelay( _OutputPulseDelay );
			}

			void setCapabilities()
			{
				_Options[ Telnet::Opt_Echo ]			= true;
				_Options[ Telnet::Opt_SuppressGoAhead ]	= true;
				_Options[ Telnet::Opt_Status ]			= false;
				_Options[ Telnet::Opt_TimingMark ]		= false;
				_Options[ Telnet::Opt_TermType ]		= false;
				_Options[ Telnet::Opt_WindowSize ]		= true;
				_Options[ Telnet::Opt_TermSpeed ]		= false;
				_Options[ Telnet::Opt_RemoteFlowCtrl ]	= false;
				_Options[ Telnet::Opt_LineMode ]		= true;
				_Options[ Telnet::Opt_EnvVars ]			= false;
			}

			// Keep the virtual handler function free of timer programming obligations
			void handleOutputPulse()
			{
				onOutputPulse();

				if ( _OutputTimer )
				{
					_OutputTimer->expires_from_now( boost::posix_time::milliseconds(_OutputPulseDelay) );
					_OutputTimer->async_wait( std::bind(&TelnetServer::handleOutputPulse, this) );
				}
			}

			// Calls Flush() on all clients
			virtual void onOutputPulse()
			{
				for ( auto s = _Sockets.begin(), s_end = _Sockets.end(); s != s_end; ++s )
					(*s)->Flush();
			}
		};


		void TelnetSocket::RequestOpt( Telnet::Option opt )
		{
			if ( _Server )
			{
				// Server socket
				TelnetServer *ThisServer = dynamic_cast< TelnetServer * >( _Server );

				if ( !ThisServer )
					throw std::runtime_error( "TelnetSocket::EnableOpt" );

				// Only allow this to happen if our server allows it
				if ( !ThisServer->IsOptEnabled(opt) )
					return;

				const char Request[] = {	(char)Telnet::Cmd_IAC,
											(char)Telnet::Cmd_DO,
											(char)opt,
											'\0' };

				// Add to our count so we interpret responses properly
				_OutstandingQueries.set( opt );
				Write( Request );
			}
			else
			{
				// Client socket
				const char Request[] = {	(char)Telnet::Cmd_IAC,
											(char)Telnet::Cmd_WILL,
											(char)opt,
											'\0' };

				// Add to our count so we interpret responses properly
				_OutstandingQueries.set( opt );
				Write( Request );
			}
		}


		//
		// The primary telnet handler function
		//
		void TelnetSocket::readChar( unsigned char input )
		{
			// Setup a couple different perspectives of the input
			Telnet::Command CurCmd = (Telnet::Command)input;
			Telnet::Option  CurOpt = (Telnet::Option)input;

			// Prepare for a response, if needed
			// [0] - IAC (must start response)
			// [1] - Telnet::Option
			// [2] - Optional Param 1
			// [3] - Optional Param 2
			char Response[4] = { (char)Telnet::Cmd_IAC, '\0', '\0', '\0' };

			switch ( _Readmode )
			{
				case Readmode_Newline:
				{
					// The next state is always Readmode_Normal
					_Readmode = Readmode_Normal;

					// But we should skip the newline's counterpart/terminator if we have one
					if ( input == '\n'
					  || input == '\r'
					  || input == '\0' )
						break;

					// Otherwise, we can fall-through to process with Readmode_Normal
				}

				case Readmode_Normal:
				{
					// Handle telnet commands
					if ( input == Telnet::Cmd_IAC )
					{
						_Readmode = Readmode_Cmd;
						break;
					}

					Event::Data EventData;

					// Line mode
					if ( !_Options[Telnet::Opt_SuppressGoAhead] )
					{
						if ( input == '\n'			// Newline
						  || input == '\r'			// Carriage Return
						  || input == '\003'		// End of Line
						  || input == '\004' )		// End of Transmission
						{
							// Echo
							if ( _Options[Telnet::Opt_Echo] )
								Write( _LineBuf );

							EventData["input"] = _LineBuf;
							HandleEvent( "onHandleLine", EventData );
							_LineBuf.clear();

							_Readmode = Readmode_Newline;
							break;
						}
						else
							_LineBuf.push_back( input );
					}
					else // Character mode
						parseChar( input );

					break;
				}

				case Readmode_Cmd:
				{
					switch ( CurCmd )
					{
						// Subnegotiation End
						case Telnet::Cmd_SE:
						{
							// Parse the received option, now that we've got it all
							handleTelnetOption();

							// Once parsed, reset our option data
							_CurOpt = Telnet::Opt_NULL;
							_CurOptData.clear();
							break;
						}

						// No Operation
						case Telnet::Cmd_NOP:
						{
							// TODO: Implement me
							break;
						}

						// Data Mark
						case Telnet::Cmd_DM:
						{
							// TODO: Implement me
							break;
						}

						// Break
						case Telnet::Cmd_BRK:
						{
							// TODO: Implement me
							break;
						}

						// Suspend
						case Telnet::Cmd_IP:
						{
							// TODO: Implement me
							break;
						}

						// Abort Output
						case Telnet::Cmd_AO:
						{
							// TODO: Implement me
							break;
						}

						// Are You There?
						case Telnet::Cmd_AYT:
						{
							// With AYT, just echo it back
							Response[1] = (char)Telnet::Cmd_AYT;
							Write( Response );
							break;
						}

						// Erase Character
						case Telnet::Cmd_EC:
						{
							if ( _LineBuf.size() > 0 )
								_LineBuf.pop_back();
							break;
						}

						// Erase Line
						case Telnet::Cmd_EL:
						{
							_LineBuf.clear();
							break;
						}

						// Go Ahead
						case Telnet::Cmd_GA:
						{
							// TODO: Implement me
							break;
						}

						// Subnegotiation Begin
						case Telnet::Cmd_SB:
						{
							_Readmode = Readmode_Subneg;
							return;
						}

						// Do / Will
						case Telnet::Cmd_DO:
						case Telnet::Cmd_WILL:
						{
							_Readmode = Readmode_Will_Do;
							return;
						}

						// Don't / Won't
						case Telnet::Cmd_DONT:
						case Telnet::Cmd_WONT:
						{
							_Readmode = Readmode_Wont_Dont;
							return;
						}

						// Interpret As Command
						// - When encountered here, it's the 2nd in sequence, and thus a "normal" ASCII 255 character
						case Telnet::Cmd_IAC:
						{
							Event::Data EventData;

							// Character Mode
							if ( _Options[Telnet::Opt_SuppressGoAhead] )
								parseChar( input );
							else // Line mode
								_LineBuf.push_back( input );

							break;
						}

						default:
							// Unhandled telnet command!
							break;
					}

					// For just about every command we want to return to normal...check above for 'return' calls on 3 character telnet commands (SB/WILL/WONT/etc.)
					_Readmode = Readmode_Normal;

					break;
				}

				case Readmode_Opt:
				{
					if ( CurCmd == Telnet::Cmd_IAC )
						_Readmode = Readmode_Cmd;
					else
						_CurOptData.push_back( input );

					break;
				}

				case Readmode_Subneg:
				{
					switch ( CurOpt )
					{
						case Telnet::Opt_Status:
						case Telnet::Opt_TermType:
						case Telnet::Opt_WindowSize:
						case Telnet::Opt_LineMode:
							_CurOpt = (Telnet::Option)input;
							break;

						default:
							_Readmode = Readmode_Normal;
							break;
					}

					_Readmode = Readmode_Opt;
					break;
				}

				case Readmode_Will_Do:
				{
					_Readmode = Readmode_Normal;

					if ( _Server )
					{
						// Server socket
						TelnetServer *ThisServer = dynamic_cast< TelnetServer * >( _Server );

						if ( !ThisServer )
							throw std::runtime_error( "TelnetSocket::readChar(): _Server pointer is not a valid TelnetServer." );

						// Check our outstanding queries so we don't always respond with "WILL" when the client acknowledges
						if ( _OutstandingQueries[CurOpt] )
						{
							_OutstandingQueries.reset( CurOpt );
							break;
						}

						// Don't bother informing that we'll do LineMode at all (linux telnet hates this, so I assume others will to)
						if ( CurOpt == Telnet::Opt_LineMode )
						{
							setOptEnabled( CurOpt );
							break;
						}

						// Only report we will if we're able to set it...also, don't bother informating that we won't do LineMode at all (just like above)
						if ( setOptEnabled(CurOpt) && CurOpt != Telnet::Opt_LineMode )
						{
							Response[1] = (char)Telnet::Cmd_WILL;

							Event::Data EventData;
							EventData["option"] = CurOpt;
							HandleEvent( "onEnableTelnetOption", EventData );
						}
						else
							Response[1] = (char)Telnet::Cmd_WONT;

						Response[2] = (char)CurOpt;
						Write( Response );
					}
					else
					{
						// Client socket
						// TODO: Write me!
					}
					break;
				}

				case Readmode_Wont_Dont:
				{
					_Readmode = Readmode_Normal;

					setOptEnabled( CurOpt, false );

					Event::Data EventData;
					EventData["option"] = CurOpt;
					HandleEvent( "onDisableTelnetOption", EventData );

					Response[1] = (char)Telnet::Cmd_WONT;
					Response[2] = (char)CurOpt;
					Write( Response );
					break;
				}

				default:
					// Unhandled mode!
					break;
			}
		}

		bool TelnetSocket::setOptEnabled( Telnet::Option opt, bool enabled )
		{
			if ( _Server )
			{
				// Server socket
				TelnetServer *ThisServer = dynamic_cast< TelnetServer * >( _Server );

				if ( !ThisServer )
					throw std::runtime_error( "TelnetSocket::setOptEnabled(): _Server pointer is not a valid TelnetServer." );

				// We can always disable something
				if ( !enabled )
				{
					_Options.reset( opt );
					return true;
				}

				// But we might not always be able to enable something
				if ( !ThisServer->IsOptEnabled(opt) )
					return false;
			}

			_Options[ opt ] = enabled;
			return true;
		}
	}
}


#endif // TOOLBOX_NETWORK_TELNET_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


