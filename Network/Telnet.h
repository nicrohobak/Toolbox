#ifndef TOOLBOX_NETWORK_TELNET_H
#define TOOLBOX_NETWORK_TELNET_H


/*
 * -- Commands
 *

Name	Decimal Code	Meaning	Comment
----------------------------------------
SE		240		End of subnegotiation parameters	
NOP 	241		No operation	
DM		242		Data mark		Indicates the position of a Synch event within the data stream. This should always be accompanied by a TCP urgent notification.
BRK		243		Break			Indicates that the "break" or "attention" key was hi.
IP		244		Suspend			Interrupt or abort the process to which the NVT is connected.
AO		245		Abort output	Allows the current process to run to completion but does not send its output to the user.
AYT		246		Are you there	Send back to the NVT some visible evidence that the AYT was received.
EC		247		Erase character	The receiver should delete the last preceding undeleted character from the data stream.
EL		248		Erase line		Delete characters from the data stream back to but not including the previous CRLF.
GA		249		Go ahead		Under certain circumstances used to tell the other end that it can transmit.
SB		250		Subnegotiation	Subnegotiation of the indicated option follows.
WILL	251		will			Indicates the desire to begin performing, or confirmation that you are now performing, the indicated option.
WONT	252		wont			Indicates the refusal to perform, or continue performing, the indicated option.
DO		253		do				Indicates the request that the other party perform, or confirmation that you are expecting the other party to perform, the indicated option.
DONT	254		dont			Indicates the demand that the other party stop performing, or confirmation that you are no longer expecting the other party to perform, the indicated option.	
IAC		255		IAC				Interpret As (a) Command


 *
 * -- Options
 * 

Options marked with a '*' are "common" options, according to Microsoft

 Code   Name                  RFC
--------------------------------------
   0    Binary Transmission   [RFC856]
*  1    Echo                  [RFC857]
   2    Reconnection          [DDN Protocol Handbook, "Telnet Reconnection Option", "Telnet Output Line Width Option", "Telnet Output Page Size Option", NIC 50005, December 1985.]
*  3    Suppress Go Ahead     [RFC858]
        Approx Message Size   ["The Ethernet, A Local Area Network: Data Link Layer and Physical Layer Specification", AA-K759B-TK, Digital Equipment Corporation, Maynard, MA. Also as: "The Ethernet - A Local Area Network", Version 1.0,
   4    Negotiation           Digital Equipment Corporation, Intel Corporation, Xerox Corporation, September 1980. And: "The Ethernet, A Local Area Network: Data Link Layer and Physical Layer Specifications", Digital, Intel and Xerox,
                              November 1982. And: XEROX, "The Ethernet, A Local Area Network: Data Link Layer and Physical Layer Specification", X3T51/80-50, Xerox Corporation, Stamford, CT., October 1980.]
*  5    Status                [RFC859]
*  6    Timing Mark           [RFC860]
   7    Remote Controlled     [RFC726]
        Trans and Echo
   8    Output Line Width     [DDN Protocol Handbook, "Telnet Reconnection Option", "Telnet Output Line Width Option", "Telnet Output Page Size Option", NIC 50005, December 1985.]
   9    Output Page Size      [DDN Protocol Handbook, "Telnet Reconnection Option", "Telnet Output Line Width Option", "Telnet Output Page Size Option", NIC 50005, December 1985.]
        Output
  10    Carriage-Return       [RFC652]
        Disposition
  11    Output Horizontal Tab [RFC653]
        Stops
  12    Output Horizontal Tab [RFC654]
        Disposition
  13    Output Formfeed       [RFC655]
        Disposition
  14    Output Vertical       [RFC656]
        Tabstops
  15    Output Vertical Tab   [RFC657]
        Disposition
  16    Output Linefeed       [RFC658]
        Disposition
  17    Extended ASCII        [RFC698]
  18    Logout                [RFC727]
  19    Byte Macro            [RFC735]
  20    Data Entry Terminal   [RFC1043][RFC732]
  21    SUPDUP                [RFC736][RFC734]
  22    SUPDUP Output         [RFC749]
  23    Send Location         [RFC779]
* 24    Terminal Type         [RFC1091]
  25    End of Record         [RFC885]
  26    TACACS User           [RFC927]
        Identification
  27    Output Marking        [RFC933]
  28    Terminal Location     [RFC946]
        Number
  29    Telnet 3270 Regime    [RFC1041]
  30    X.3 PAD               [RFC1053]
* 31    Negotiate About       [RFC1073]
        Window Size
* 32    Terminal Speed        [RFC1079]
* 33    Remote Flow Control   [RFC1372]
* 34    Linemode              [RFC1184]
  35    X Display Location    [RFC1096]
* 36    Environment Option    [RFC1408]
  37    Authentication Option [RFC2941]
  38    Encryption Option     [RFC2946]
  39    New Environment       [RFC1572]
        Option
  40    TN3270E               [RFC2355]
  41    XAUTH                 [Rob_Earhart]
  42    CHARSET               [RFC2066]
  43    Telnet Remote Serial  [Robert_Barnes]
        Port (RSP)
  44    Com Port Control      [RFC2217]
        Option
  45    Telnet Suppress Local [Wirt_Atmar]
        Echo
  46    Telnet Start TLS      [Michael_Boe]
  47    KERMIT                [RFC2840]
  48    SEND-URL              [David_Croft]
  49    FORWARD_X             [Jeffrey_Altman]
50-137  Unassigned            [IANA]
  138   TELOPT PRAGMA LOGON   [Steve_McGregory]
  139   TELOPT SSPI LOGON     [Steve_McGregory]
  140   TELOPT PRAGMA         [Steve_McGregory]
        HEARTBEAT
141-254 Unassigned
  255   Extended-Options-List [RFC861]

*/

#include <sstream>


namespace Toolbox
{
	namespace Network
	{
		namespace Telnet
		{
			enum Command
			{
				Cmd_NULL	= 0,

				Cmd_SE		= 240,		// End of Subnegotiation
				Cmd_NOP,				// No Operation
				Cmd_DM,					// Data Mark
				Cmd_BRK,				// Break
				Cmd_IP,					// Suspend
				Cmd_AO,					// Abort Output
				Cmd_AYT,				// Are You There?
				Cmd_EC,					// Erase Character
				Cmd_EL,					// Erase Line
				Cmd_GA,					// Go Ahead
				Cmd_SB,					// SuBnegotiation
				Cmd_WILL,				// Will	 (client informing us of used option)
				Cmd_WONT,				// Won't (client informing us of unused option)
				Cmd_DO,					// Do	 (client requesting us to use an option)
				Cmd_DONT,				// Don't (client requesting us to not use an option)
				Cmd_IAC		= 255,

				Cmd_MAX,
				Cmd_FIRST	= Cmd_NULL,
				Cmd_DEFAULT	= Cmd_NULL
			};

			enum Option
			{
				Opt_NULL			= 0,

				Opt_Echo			= 1,
				Opt_SuppressGoAhead	= 3,
				Opt_Status			= 5,
				Opt_TimingMark		= 6,
				Opt_TermType		= 24,
				Opt_WindowSize		= 31,
				Opt_TermSpeed		= 32,
				Opt_RemoteFlowCtrl	= 33,
				Opt_LineMode		= 34,
				Opt_EnvVars			= 36,

				Opt_MAX,
				Opt_FIRST			= Opt_NULL,

				Opt_DEFAULT			= Cmd_NULL
			};


			const std::string OptionName( Option opt )
			{
				std::stringstream Name("");

				switch ( opt )
				{
					case Opt_Echo:
						Name << "Echo";
						break;
					case Opt_SuppressGoAhead:
						Name << "Suppress Go Ahead";
						break;
					case Opt_Status:
						Name << "Status";
						break;
					case Opt_TimingMark:
						Name << "Timing";
						break;
					case Opt_TermType:
						Name << "Terminal Type";
						break;
					case Opt_WindowSize:
						Name << "NAWS (Negotiate About Window Size)";
						break;
					case Opt_TermSpeed:
						Name << "Terminal Speed";
						break;
					case Opt_RemoteFlowCtrl:
						Name << "Remote Flow Control";
						break;
					case Opt_LineMode:
						Name << "Line Mode";
						break;
					case Opt_EnvVars:
						Name << "Environment Variables";
						break;
					default:
						Name << "Unrecognized Telnet Option (" << opt << ")";
						break;
				}

				return Name.str();
			}
		}
	}
}


#endif // TOOLBOX_NETWORK_TELNET_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


