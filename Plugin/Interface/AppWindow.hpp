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
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Multiple/SDL2.so" );
	AppWindow::Ptr AppWin = PluginManager.Create< AppWindow >( "SDL2" );
	AppWin->Create( "SDL2 Window" );
		// Do things with the window
 	AppWin->Destroy();
 ****************************************************************************/


#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_INTERFACE( AppWindow, 0.1 )

		constexpr static int DEFAULT_WIDTH = 800;
		constexpr static int DEFAULT_HEIGHT = 600;

		// TODO: Make this properly generic
		virtual void Create( const std::string &title = std::string("Toolbox Application Window"),
							 int width = DEFAULT_WIDTH,
							 int height = DEFAULT_HEIGHT,
							 long int flags = 0,
							 int glMajorVer = 0,
							 int glMinorVer = 0 ) = 0;

		virtual void Destroy() = 0;

		virtual void GetPos( int *x, int *y ) = 0;					// Get window position
		virtual void SetPos( int x, int y ) = 0;					// Set window position
		virtual void Center( bool x = true, bool y = true ) = 0;	// Center window on screen

		virtual void Minimize() = 0;								// Minimize the window
		virtual void Maximize() = 0;								// Maximize the window
		virtual void Restore() = 0;									// Restore window to windowed mode

		virtual void Raise() = 0;									// Raise the window above all others

		virtual void Swap()			{ }								// Swap screen buffers (Optional)

	END_TOOLBOX_PLUGIN_DEF
}


#endif // TOOLBOX_PLUGIN_INTERFACE_APPWINDOW_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


