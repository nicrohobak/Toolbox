#ifndef TOOLBOX_NETWORK_VT100_H
#define TOOLBOX_NETWORK_VT100_H


#include <exception>
#include <sstream>


namespace Toolbox
{
	namespace Network
	{
		namespace VT100
		{
			enum Command
			{
				Cmd_NULL,

				Cmd_GetCursorPos,
				Cmd_SetCursorPos,
				Cmd_SetCursorCol,
				Cmd_MoveCursorUp,
				Cmd_MoveCursorDown,
				Cmd_MoveCursorForward,
				Cmd_MoveCursorBackward,
				Cmd_SaveCursorPos,
				Cmd_RestoreCursorPos,
				Cmd_HideCursor,
				Cmd_ShowCursor,
				Cmd_EraseLine,
				Cmd_EraseDisplay,

				Cmd_MAX,
				Cmd_FIRST = Cmd_NULL,

				Cmd_DEFAULT = Cmd_NULL
			};


			std::string Cmd( Command cmd, unsigned short num = 0 )
			{
				constexpr const char *dbg_CurFunc = "Toolbox::Network::VT100::Cmd(Command, unsigned short)";
				std::stringstream Cmd("");

				if ( cmd == Cmd_SetCursorPos )
					throw std::runtime_error( std::string(dbg_CurFunc) + ": ERROR -- CmdSetCursorPos requires Cmd() to be called with VT100:Cmd(Command, unsigned short, unsigned short)." );

				Cmd.str("");
				Cmd << "\033[";

				// Only add the number if it's required and valid
				if ( num != 0
				  && cmd != Cmd_GetCursorPos
				  && cmd != Cmd_SaveCursorPos
				  && cmd != Cmd_RestoreCursorPos
				  && cmd != Cmd_HideCursor
				  && cmd != Cmd_ShowCursor )
					Cmd << num;

				switch ( cmd )
				{
					case Cmd_GetCursorPos:
						Cmd << "6n";
						break;
					case Cmd_SetCursorCol:
						Cmd << "G";
						break;
					case Cmd_MoveCursorUp:
						Cmd << "A";
						break;
					case Cmd_MoveCursorDown:
						Cmd << "B";
						break;
					case Cmd_MoveCursorForward:
						Cmd << "C";
						break;
					case Cmd_MoveCursorBackward:
						Cmd << "D";
						break;
					case Cmd_SaveCursorPos:
						Cmd << "s";
						break;
					case Cmd_RestoreCursorPos:
						Cmd << "u";
						break;
					case Cmd_HideCursor:
						Cmd << "?25l";
						break;
					case Cmd_ShowCursor:
						Cmd << "?25h";
						break;
					case Cmd_EraseLine:
						Cmd << "K";
						break;
					case Cmd_EraseDisplay:
						Cmd << "2J";
						break;
					default:
						break;
				}

				return Cmd.str();
			}
					
				
			std::string Cmd( Command cmd, unsigned short x, unsigned short y )
			{
				constexpr const char *dbg_CurFunc = "Toolbox::Network::VT100::Cmd(Command, unsigned short, unsigned short)";
				std::stringstream Cmd("");

				if ( cmd != Cmd_SetCursorPos )
					throw std::runtime_error( std::string(dbg_CurFunc) + ": ERROR -- Every command except CmdSetCursorPos requires Cmd() to be called with VT100:Cmd(Command, unsigned short)." );

				Cmd.str();
				Cmd << "\033[";

				switch ( cmd )
				{
					case Cmd_SetCursorPos:
						Cmd << x << ";" << y << "H";		// Either of these...
						//Cmd << x << ";" << y << "j";		// ...should do the same thing
						break;
					default:
						break;
				}

				return Cmd.str();
			}
		}
	}
}


#endif // TOOLBOX_NETWORK_VT100_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


