#ifndef TOOLBOX_RENDERER2D_HPP
#define TOOLBOX_RENDERER2D_HPP

/*
 * Generic 2D Rendering Interface
 *
 * A simple, generic interface for creating a 2D rendering context.
 */

/* NOTE: Plugin Required! ****************************************************

	* Default plugin implementation available at:
	  <Toolbox/Plugin/Common/Renderer2D>

	* For more information, please refer to <Toolbox/Plugin/README>

 ****************************************************************************/

/*****************************************************************************
    // Example use:
	Toolbox::PluginManager PluginManager;
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Multiple/SDL2.so" );

	// Our SDL2 plugin happens to provide both
	Toolbox::AppWindow::Ptr AppWin = PluginManager.Create< Toolbox::AppWindow >( "SDL2" );
	Toolbox::Renderer2D::Ptr Renderer = PluginManager.Create< Toolbox::Renderer2D >( "SDL2" );

	Renderer->Sprite_Load( "MySprite", "./mysprite.png" );

	// Create an SDL window with an OpenGL 3.3 rendering context
	AppWin->Create( "SDL2 Window", 800, 600" );
		Renderer->BeginFrame();
			Renderer->Sprite_Render( "MySprite" );
		Renderer->EndFrame();
		AppWin->Swap();					// Swap VRAM
 	AppWin->Destroy();
 ****************************************************************************/

#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_INTERFACE( Renderer2D, "0.1" )

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		// Sprites
		virtual void Sprite_Delete( const std::string &name ) = 0;								// Sprites are created automatically when necessary
		virtual void Sprite_Load( const std::string &name, const std::string &fileName ) = 0;
		virtual void Sprite_Render( const std::string &name ) = 0;

	END_TOOLBOX_PLUGIN_DEF
}


#endif // TOOLBOX_RENDERER2D_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


