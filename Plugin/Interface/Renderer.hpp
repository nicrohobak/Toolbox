#ifndef TOOLBOX_PLUGIN_INTERFACE_RENDERER_HPP
#define TOOLBOX_PLUGIN_INTERFACE_RENDERER_HPP

/*
 * Generic GL Rendering Interface
 *
 * A simple, generic interface for creating a graphics library rendering context.
 */

/*****************************************************************************
    // Example use:
	Toolbox::PluginManager PluginManager;
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/AppWindow/SDL2.so" );
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Renderer/OpenGL.so" );

	Toolbox::AppWindow::Ptr AppWin = PluginManager.Create< Toolbox::AppWindow >( "SDL2" );
	Toolbox::Renderer::Ptr Renderer = PluginManager.Create< Toolbox::Renderer >( "OpenGL" );

	// Create an SDL window with an OpenGL 3.3 rendering context
	AppWin->Create( "SDL2 Window", 800, 600, SDL_WINDOW_OPENGL, 3, 3 );
		Renderer->BeginFrame();
			// OpenGL rendering code
		Renderer->EndFrame();
		AppWin->Swap();					// Swap VRAM
 	AppWin->Destroy();
 ****************************************************************************/

#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_INTERFACE( Renderer, "0.1" )

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		// Models
		virtual void Model_Delete( const std::string &name ) = 0;								// Models are created automatically when necessary
		virtual void Model_Load( const std::string &name, const std::string &fileName ) = 0;
		virtual void Model_SetVertices( const std::string &name, size_t numVertices, const float *vertices ) = 0;
		virtual void Model_Render( const std::string &name ) = 0;

	END_TOOLBOX_PLUGIN_DEF
}


#endif // TOOLBOX_PLUGIN_INTERFACE_RENDERER_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


