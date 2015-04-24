#ifndef TOOLBOX_PLUGIN_INTERFACE_APPWINDOW_HPP
#define TOOLBOX_PLUGIN_INTERFACE_APPWINDOW_HPP

/*
 * Generic GUI Application Window Interface
 *
 * A simple, generic interface for creating an application window.
 */

/*****************************************************************************
    // Example use:
	Toolbox::PluginManager PluginManager;
	PluginManager.Load( "/path/to/Toolbox/Plugin/Common/AppWindow/sdl.so" );
	AppWindow::Ptr AppWin = PluginManager.Create< AppWindow >( "SDL" );
	AppWin->Create( "SDL Window" );
		// Do things with the window
 	AppWin->Destroy();
 ****************************************************************************/


#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_INTERFACE( AppWindow, 0.1 )

		constexpr static int DEFAULT_WIDTH = 800;
		constexpr static int DEFAULT_HEIGHT = 600;

		virtual void Create( const std::string &title = std::string("App"), int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, long int flags = 0 ) = 0;
		virtual void Destroy() = 0;

	END_TOOLBOX_PLUGIN_DEF
}


#endif // TOOLBOX_PLUGIN_INTERFACE_APPWINDOW_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


