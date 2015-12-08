#ifndef TOOLBOX_APPWINDOW_HPP
#define TOOLBOX_APPWINDOW_HPP

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

	// Set desired window options
	//AppWin->SetOption( Toolbox::AppWindowOption_OpenGL_Enabled, true );
	//AppWin->SetOption( Toolbox::AppWindowOption_OpenGL_MajorVer, 3 );
	//AppWin->SetOption( Toolbox::AppWindowOption_OpenGL_MinorVer, 3 );

	AppWin->Create( "SDL2 Window" );
		// Do things with the window
 	AppWin->Destroy();
 ****************************************************************************/


#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	enum tAppWindowOption
	{
		AppWindowOption_INVALID,

		AppWindowOption_OpenGL_Enabled,
		AppWindowOption_OpenGL_MajorVer,
		AppWindowOption_OpenGL_MinorVer,

		AppWindowOption_MAX,
		AppWindowOption_FIRST = AppWindowOption_OpenGL_Enabled
	};

	DEFINE_TOOLBOX_PLUGIN_INTERFACE( AppWindow, 0.1 )

		typedef unsigned int	tAppWindowOptionValue;

		constexpr static int	DEFAULT_WIDTH = 800;
		constexpr static int	DEFAULT_HEIGHT = 600;

		//////////

		virtual void SetOption( tAppWindowOption option, tAppWindowOptionValue value = tAppWindowOptionValue() ) = 0;
		virtual void UnsetOption( tAppWindowOption option ) = 0;
		virtual tAppWindowOptionValue GetOption( tAppWindowOption option ) = 0;

		virtual void Create( const std::string &title = std::string("Toolbox Application Window"),
							 int width = DEFAULT_WIDTH,
							 int height = DEFAULT_HEIGHT ) = 0;

		virtual void Destroy() = 0;

		virtual void GetPos( int *x, int *y ) = 0;					// Get window position
		virtual void SetPos( int x, int y ) = 0;					// Set window position
		virtual void Center( bool x = true, bool y = true ) = 0;	// Center window on screen

		virtual void Minimize() = 0;								// Minimize the window
		virtual void Maximize() = 0;								// Maximize the window
		virtual void Restore() = 0;									// Restore window to windowed mode

		virtual void Raise() = 0;									// Raise the window above all others

		virtual void Delay( size_t milliseconds ) = 0;				// Delay for X milliseconds

		virtual void Swap()		{ }									// Swap screen buffers (Optional)

	END_TOOLBOX_PLUGIN_DEF
}


#endif // TOOLBOX_APPWINDOW_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


