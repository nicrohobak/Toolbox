#ifndef TOOLBOX_ARGUMENTS_HPP
#define TOOLBOX_ARGUMENTS_HPP

/*
 * Toolbox/Arguments.hpp
 *
 * A simple C++ quote-aware, "command-line" parser implementation
 */


/*****************************************************************************
 * Example program:

	#include <iostream>
	#include <stdexcept>

	#include <Toolbox/Arguments.hpp>


	void DisplayArguments( const Toolbox::Arguments &Args, size_t nestLevel = 0 )
	{
		std::string Prefix("");

		for ( size_t i = 0; i < nestLevel; ++i )
			Prefix.push_back( '\t' );

		std::cout << Prefix << "Number of arguments: " << Args.Count() << std::endl;

		for ( int a = 0, a_end = Args.Count(); a != a_end; ++a )
		{
			const Toolbox::Arguments &CurArg = Args[a];
			std::cout << Prefix << "Arg " << a << ": " << CurArg.Str() << std::endl;

			if ( CurArg.Count() > 0 )
				DisplayArguments( CurArg, nestLevel + 1 );
		}
	}


	int main( int argc, char *argv[] )
	{
		int ReturnCode = 0;

		try
		{
			std::string TestStr( "This is an 'alternating quote type test with \"nested 'nested deep \"way down deep\" deep' quotes\" test' test." );
			std::cout << "TestStr: " << TestStr << std::endl;

			Toolbox::Arguments Args( TestStr );
			DisplayArguments( Args );

			// Reverse the starting quote type, and it should still act essentially the same
			TestStr = "This is an \"alternating quote type test with 'nested \"nested deep 'way down deep' deep\" quotes' test\" test.";
			std::cout << "TestStr: " << TestStr << std::endl;

			Args.Parse( TestStr );
			DisplayArguments( Args );
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


#include <list>
#include <memory>
#include <string>

#include <cctype>


namespace Toolbox
{
	class Arguments
	{
	public:
		typedef std::shared_ptr< Arguments >	Ptr;
		typedef std::list< std::string >		StringList;
		typedef std::list< Arguments::Ptr >		ArgList;
		typedef ArgList::iterator				iterator;
		typedef ArgList::const_iterator			const_iterator;
		typedef ArgList::reverse_iterator		reverse_iterator;
		typedef ArgList::const_reverse_iterator	const_reverse_iterator;

	public:
		static bool IsQuoteChar( const char c )
		{
			if ( c == '\'' || c == '"' )
				return true;

			return false;
		}

		static Ptr New()
		{
			Ptr NewPtr = Ptr( new Arguments() );
			return NewPtr;
		}

		static Ptr New( const std::string &str, bool parseStr = true )
		{
			Ptr NewPtr = Ptr( new Arguments(str, parseStr) );
			return NewPtr;
		}

		static Ptr New( const char *str, bool parseStr = true )
		{
			Ptr NewPtr = Ptr( new Arguments(str, parseStr) );
			return NewPtr;
		}

	public:
		Arguments():
			_QuoteChar( '\0' )
		{
		}

		Arguments( const std::string &str, bool parseStr = true ):
			_QuoteChar( '\0' )
		{
			if ( parseStr )
				Parse( str );
			else
				_Orig = str;
		}

		Arguments( const char *str, bool parseStr = true ):
			_QuoteChar( '\0' )
		{
			if ( parseStr )
				Parse( std::string(str) );
			else
				_Orig = std::string( str );
		}

		size_t Count() const
		{
			return _Args.size();
		}

		size_t Num() const
		{
			return Count();
		}

		const std::string &Str() const
		{
			return _Orig;
		}

		Arguments operator[]( size_t index )
		{
			size_t CurIndex = 0;
			for ( ArgList::iterator i = _Args.begin(), i_end = _Args.end(); i != i_end; ++i, ++CurIndex )
			{
				if ( CurIndex == index )
					return **i;
			}

			return Arguments();
		}

		const Arguments operator[]( size_t index ) const
		{
			size_t CurIndex = 0;
			for ( ArgList::const_iterator i = _Args.begin(), i_end = _Args.end(); i != i_end; ++i, ++CurIndex )
			{
				if ( CurIndex == index )
					return **i;
			}

			return Arguments();
		}

		void erase( size_t index )
		{
			size_t CurIndex = 0;
			for ( ArgList::iterator i = _Args.begin(), i_end = _Args.end(); i != i_end; ++i, ++CurIndex )
			{
				if ( CurIndex == index )
				{
					_Args.erase( i );
					rebuildOrigFromArgs();
					return;
				}
			}
		}

		void erase( iterator pos )
		{
			_Args.erase( pos );
			rebuildOrigFromArgs();
		}

		void erase( const_iterator pos )
		{
			_Args.erase( pos );
			rebuildOrigFromArgs();
		}

		void pop()
		{
			pop_front();
		}

		void pop_front()
		{
			_Args.pop_front();
			rebuildOrigFromArgs();
		}

		void pop_back()
		{
			_Args.pop_back();
			rebuildOrigFromArgs();
		}

		iterator begin()
		{
			return _Args.begin();
		}

		iterator end()
		{
			return _Args.end();
		}

		const_iterator begin() const
		{
			return _Args.begin();
		}

		const_iterator end() const
		{
			return _Args.end();
		}

		reverse_iterator rbegin()
		{
			return _Args.rbegin();
		}

		reverse_iterator rend()
		{
			return _Args.rend();
		}

		const_reverse_iterator rbegin() const
		{
			return _Args.rbegin();
		}

		const_reverse_iterator rend() const
		{
			return _Args.rend();
		}

		void Parse( const std::string &str )
		{
			_Orig = str;

			if ( !_Args.empty() )
				_Args.clear();

			parse( *this, _Orig, _Orig.begin() );
		}

	protected:
		std::string		_Orig;
		ArgList			_Args;
		char			_QuoteChar;

	protected:
		Ptr append( Ptr args )
		{
			if ( args )
				_Args.push_back( args );

			return args;
		}

		Ptr append( const std::string &arg, bool parseStr = true )
		{
			if ( !arg.empty() )
			{
				Ptr NewArg = New( arg, parseStr );
				_Args.push_back( NewArg );
				return NewArg;
			}

			return Ptr();
		}

		Ptr append( const char *arg, bool parseStr = true )
		{
			if ( arg != NULL )
			{
				Ptr NewArg = New( arg, parseStr );
				_Args.push_back( NewArg );
				return NewArg;
			}
		}

		// Rebuilds _Orig based on the child arguments
		void rebuildOrigFromArgs()
		{
			_Orig = "";

			for ( iterator a = begin(), a_end = end(); a != a_end; ++a )
			{
				Arguments &CurArg = **a;

				if ( !_Orig.empty() )
					_Orig.push_back( ' ' );

				// If it has a quoted argument, lets rebuild that aspect as well
				if ( CurArg.begin() != CurArg.end() )
				{
					_Orig.push_back( _QuoteChar );
					_Orig.append( CurArg.Str() );
					_Orig.push_back( _QuoteChar );

				}
				else
					_Orig.append( CurArg.Str() );
			}
		}

	protected:
		// Parses the contents of _Orig and populate our ArgList
		static std::string::const_iterator parse( Arguments &This, const std::string &str, std::string::const_iterator pos )
		{
			std::string::const_iterator CurPos = pos;
			std::string CurArg("");

			for ( std::string::const_iterator End = str.end(); CurPos != End; ++CurPos )
			{
				if ( isspace(*CurPos) )
				{
					if ( !CurArg.empty() )
					{
						This.append( CurArg, false );
						CurArg.clear();
					}
					continue;
				}
				else if ( IsQuoteChar(*CurPos) )
				{
					// Are we closing a quote?...
					if ( *CurPos == This._QuoteChar )
					{
						if ( !CurArg.empty() )
						{
							This.append( CurArg, false );
							CurArg.clear();
						}

						// Now, flip the quote character now so we can rebuild/display properly
						if ( This._QuoteChar == '\'' )
							This._QuoteChar = '"';
						else
							This._QuoteChar = '\'';

						if ( This._Orig.empty() )
							This.rebuildOrigFromArgs();

						return ++CurPos;
					}

					// Or just opening a new one?
					Ptr NewArg = New();
					NewArg->_QuoteChar = *CurPos;
					CurPos = parse( *NewArg.get(), str, ++CurPos );
					This.append( NewArg );
					CurArg.clear();
					continue;
				}

				CurArg.push_back( *CurPos );
			}

			if ( !CurArg.empty() )
				This.append( CurArg, false );
		}
	};
}


#endif // TOOLBOX_ARGUMENTS_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


