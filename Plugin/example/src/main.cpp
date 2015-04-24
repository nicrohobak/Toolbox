/*
 * Plugin Example
 */

#include <iostream>

#include <Toolbox/Plugin.hpp>

#include "SimpleTask_interface.h"


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

	try
	{
		Toolbox::PluginManager PluginManager;

		const std::string PluginDir( "./plugin/" );
		const std::string PluginExt( ".so" );
		PluginManager.Load( PluginDir + "simple1" + PluginExt );
		PluginManager.Load( PluginDir + "simple2" + PluginExt );

		// Use the Toolbox::Plugin::Manager to create specific implementations of our SimpleTask interface
		SimpleTask::Ptr Simple1 = PluginManager.Create< SimpleTask >( "SimpleImple1" );
		SimpleTask::Ptr Simple2 = PluginManager.Create< SimpleTask >( "SimpleImple2" );
		
		int a = 5, b = 7;

		std::cout << a << " " << Simple1->Operation() << " = " << Simple1->Task( a ) << std::endl;
		std::cout << b << " " << Simple1->Operation() << " = " << Simple1->Task( b ) << std::endl;
		std::cout << a << " " << Simple2->Operation() << " = " << Simple2->Task( a ) << std::endl;
		std::cout << b << " " << Simple2->Operation() << " = " << Simple2->Task( b ) << std::endl;
	}
	catch ( std::exception &ex )
	{
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		ReturnCode = 1;
	}

	return ReturnCode;
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


