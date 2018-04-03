#ifndef TOOLBOX_LOG_HPP
#define TOOLBOX_LOG_HPP

/*
 * Toolbox/Log.hpp
 *
 * Generic Application Logging
 */

/*****************************************************************************
 * How to use:
 *
 * First, define a log object:
 *     Toolbox::Log MyLog(	"sample.log",				// File name
 *     						Toolbox::Log::Level::Info,	// Logger log level
 *     						true,						// Overwrite log file?
 *     						true );						// Also log to stdout/stderr
 *
 * Then use that lob object with the Toolbox_LOG() macro for logging
 *     Toolbox_LOG( MyLog, Toolbox::Log::Level::Trace, "This is a trace test." );
 *
 * Change the log verbosity at runtime:
 *     MyLog->LogLevel = Toolbox::Log::Level::Trace;	// Changes the default
 *     													// verbosity for the
 *														// log object.
 *
 *****************************************************************************
 * Notes:
 * - By default, all Log objects will log to stdout/stderr unless explicitly
 *   disabled.
 *
 * - Log objects will only log statements at their level or less.  Verbosity
 *   is increased by increasing the log level. 
 *
 * - Any Log object set to Toolbox::Log::Level::Debug or higher will always
 *   output filename and line numbers for all logs.
 *
 * - Some log levels (fatal/error/warn/debug/trace) will always output
 *   filename and line numbers, regardless of the Log object's current log
 *   level.
 *
 * - Some log levels (fatal/error/warn) will log to stderr instead of stdout
 *   when standard output is enabled.
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>


namespace Toolbox
{
    #define Toolbox_LOG( _log, _logLevel, _msg )												\
		if ( (_logLevel) == Toolbox::Log::Level::Any || (_logLevel) <= (_log).LogLevel )		\
		{																						\
			(_log).Write( (_logLevel), __FILE__, __LINE__ ) << _msg << std::endl;				\
																								\
			if ( (_log).StdOutput() )															\
			{																					\
				if ( Toolbox::Log::LevelDataTable[_logLevel].std_err )							\
					std::cerr << _msg << std::endl;												\
				else																			\
					std::cout << _msg << std::endl;												\
			}																					\
		}


	class Log
	{
	public:
		enum Level
		{
			Any,
			Fatal,
			Error,
			Warn,
			Info,
			Debug,
			Trace,

			MAX,

			FIRST	= Any,
			DEFAULT	= Info,
		};

		struct LevelData
		{
			Level			level;
			const char *	name;
			bool			std_err;
		};

		static const LevelData LevelDataTable[];

		const std::string DefaultTimestampFormat = "%Y-%m-%d %X";

	public:
		Level			LogLevel;				// The lowest log level this logger will output

	public:
		Log():
			LogLevel( Level::DEFAULT ),
			_timestampFormat( DefaultTimestampFormat ),
			_std_output( true )
		{
		}

		Log( Level level ):
			LogLevel( level ),
			_timestampFormat( DefaultTimestampFormat ),
			_std_output( true )
		{
		}

		Log( const std::string &fileName, Level level = Level::DEFAULT, bool overwrite = false, bool std_output = true ):
			LogLevel( level ),
			_timestampFormat( DefaultTimestampFormat ),
			_std_output( std_output )
		{
			Open( fileName, overwrite );
		}

		virtual ~Log()
		{
			if ( IsOpen() )
				Close();
		}

		const std::string &TimestampFormat() const
		{
			return _timestampFormat;
		}

		// strftime format string
		void SetTimestampFormat( const std::string &format )
		{
			_timestampFormat = format;
		}

		bool StdOutput() const
		{
			return _std_output;
		}

		void StdOutput( bool std_output )
		{
			_std_output = std_output;
		}

		bool IsOpen() const
		{
			return _file.is_open();
		}

		// Unless we Open() a file, we will only log to stdout/stderr
		void Open( const std::string &fileName, bool overwrite = false )
		{
			if ( IsOpen() )
				Close();

			if ( fileName.empty() )
				throw std::runtime_error( "Toolbox::Log::Open(): No filename provided." );

			if ( overwrite )
				_file.open( fileName.c_str(), std::ofstream::out | std::ofstream::trunc );
			else
				_file.open( fileName.c_str(), std::ofstream::out | std::ofstream::app );
		};

		void Close()
		{
			_file.close();
		}

		// In almost all circumstances, this should be called via the Toolbox_LOG() macro rather than directly
		std::ofstream &Write( Level level = Level::Any, const char *fileName = NULL, unsigned int lineNum = 0 )
		{
			std::string CurTimestamp = curTimestamp();

			if ( IsOpen() )
			{
				if ( LogLevel >= Level::Debug || LevelDataTable[level].std_err )
					_file << CurTimestamp << " [" << LevelDataTable[level].name << "] [" << fileName << ", line " << lineNum << "]: ";
				else
					_file << CurTimestamp << " [" << LevelDataTable[level].name << "]: ";
			}

			if ( _std_output )
			{
				if ( LogLevel >= Level::Debug || LevelDataTable[level].std_err )
					std::cerr << CurTimestamp << " [" << LevelDataTable[level].name << "] [" << fileName << ", line " << lineNum << "]: ";
				else
					std::cout << CurTimestamp << " [" << LevelDataTable[level].name << "]: ";
			}

			return _file;
		}

	protected:
		std::ofstream	_file;					// The file to log to (optional)
		std::string		_timestampFormat;		// The strftime format string
		bool			_std_output;			// Enables stdout/stderr output

		std::string curTimestamp() const
		{
			static char buf[80];
			static time_t now;
			static struct tm tstruct;

			memset( buf, 80, 0 );

			now = time( 0 );
			tstruct = *localtime( &now );
			strftime( buf, sizeof(buf), _timestampFormat.c_str(), &tstruct );

			return std::string( buf );
		}
	};

	const Log::LevelData Log::LevelDataTable[] =
	{
		{ Any,		"-----",	false	},
		{ Fatal,	"FATAL",	true	},
		{ Error,	"ERROR",	true	},
		{ Warn,		"WARN ",	true	},
		{ Info,		"INFO ",	false	},
		{ Debug,	"DEBUG",	false	},
		{ Trace,	"TRACE",	false	},
		{ MAX,		NULL,		true	}
	};
}


#endif // TOOLBOX_LOG_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper



