#ifndef TOOLBOX_SHAREDLIBRARY_HPP
#define TOOLBOX_SHAREDLIBRARY_HPP

/*
 * Toolbox/SharedLibrary.hpp
 *
 * A simple C++ shared library implementation
 */

/*****************************************************************************
 * SharedLibrary Requirements:
 *
 * - MUST link with: -ldl
 *****************************************************************************/

/*****************************************************************************
// Example Program
#include <iostream>
#include <Toolbox/SharedLibrary.hpp>

int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

	try
	{
		// Pass the library path to the constructor to load everything
		Toolbox::SharedLibrary Library( "./path/to/library.so" );

		// Use Toolbox::SharedLibrary::GetSymbol() to get library symbols
		//   directly...like this function pointer...
		typedef int (*tAddIntsFunc)( int a, int b );
		tAddIntsFunc AddInts = Library.GetSymbol< tAddIntsFunc >( "LibraryAddIntsFunc" );

		// ...Or use Toolbox::SharedLibrary::GetValue() to reduce
		//   boilerplate for global variables, etc.
		int a = Library.GetValue< int >( "LibraryGlobalInteger" );
		int b = 7;

		std::cout << a << " + " << b << " = " << (*AddInts)( a, b ) << std::endl;
	}
	catch ( std::exception &ex )
	{
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		ReturnCode = 1;
	}

	return ReturnCode;
}


// Build with:
// g++ -std=c++14 -Wall -pedantic -o example -ldl
*****************************************************************************/

/****************************************************************************/
/****************************************************************************/


#include <stdexcept>
#include <string>

#include <Toolbox/Defines.h>


extern "C"
{
	#include <dlfcn.h>
}


namespace Toolbox
{
	/*
	 * Generic shared library implementation
	 */
	class SharedLibrary
	{
	public:
		TOOLBOX_POINTERS( SharedLibrary )

	public:
		SharedLibrary( const std::string &fileName ):
			_Handle( NULL )
		{
			_Handle = dlopen( fileName.c_str(), RTLD_NOW );

			if ( !_Handle )
				throw std::runtime_error( dlerror() );
		}

		virtual ~SharedLibrary()
		{
			if ( _Handle )
				dlclose( _Handle );
		}

		template <typename tType>
		tType GetSymbol( const std::string &symbol )
		{
			tType Symbol = NULL;
			Symbol = (tType) dlsym( _Handle, symbol.c_str() );

			if ( !Symbol )
				throw std::runtime_error( dlerror() );

			return Symbol;
		}

		template <typename tType>
		tType GetValue( const std::string &symbol )
		{
			return *(GetSymbol<tType *>(symbol));
		}

	protected:
		void *			_Handle;
	};
}


#endif // TOOLBOX_SHAREDLIBRARY_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


