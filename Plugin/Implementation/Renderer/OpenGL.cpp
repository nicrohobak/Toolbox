/*
 * OpenGL Renderer Plugin
 *
 * NOTE: This plugin requires -lGL
 */

#include <Toolbox/Plugin.hpp>
#include <Toolbox/Plugin/Interface/Renderer.hpp>

#include <GLES3/gl3.h>

/*****************************************************************************
// Example use (link with: -ldl -lSDL2 -lGL)
#include <iostream>
#include <stdexcept>

#include <Toolbox/Plugin.hpp>
#include <Toolbox/Plugin/Interface/AppWindow.hpp>
#include <Toolbox/Plugin/Interface/Renderer.hpp>

#include <SDL2/SDL.h>		// SDL
#include <GLES3/gl3.h>		// OpenGL


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

	try
	{
		Toolbox::PluginManager PluginManager;
		PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/AppWindow/SDL2.so" );
		PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Renderer/OpenGL.so" );

		Toolbox::AppWindow::Ptr AppWin = PluginManager.Create< Toolbox::AppWindow >( "SDL2" );
		Toolbox::Renderer::Ptr Renderer = PluginManager.Create< Toolbox::Renderer >( "OpenGL" );

		// Create an SDL application window with an OpenGL 3.3 rendering context
		AppWin->Create( "Test App", 800, 600, SDL_WINDOW_OPENGL, 3, 3 );

		static const GLfloat TriangleData[] =
		{    
			 0.0f,	 0.5f,	 0.0f,
			 0.5f,	-0.5f,	 0.0f,
			-0.5f,	-0.5f,	 0.0f,
		};
        
		GLuint Triangle;
		glGenBuffers( 1, &Triangle );
		glBindBuffer( GL_ARRAY_BUFFER, Triangle );
		glBufferData( GL_ARRAY_BUFFER, sizeof(TriangleData), TriangleData, GL_STATIC_DRAW );

		Renderer->BeginFrame();			// Prepare the renderer

			// OpenGL Code -- Draw our triangle
			glBindBuffer( GL_ARRAY_BUFFER, Triangle );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
			glDrawArrays( GL_TRIANGLES, 0, 3 );

		Renderer->EndFrame();			// Finalize the renderer

		AppWin->Swap();					// Swap the frame buffers to display the rendered image

		SDL_Delay( 2000 );
	}
	catch ( std::exception &ex )
	{
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		ReturnCode = 0;
	}

	return ReturnCode;
}
*****************************************************************************/


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_D( Renderer, OpenGL )

		OpenGL()
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

			glGenVertexArrays( 1, &_VertexArrayID );
			glBindVertexArray( _VertexArrayID );
		}

		virtual ~OpenGL()
		{
			if ( _VertexArrayID != GL_INVALID_VALUE )
				glDeleteVertexArrays( 1, &_VertexArrayID );
		}

		virtual void BeginFrame()
		{
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glEnableVertexAttribArray( 0 );
		}

		virtual void EndFrame()
		{
			glDisableVertexAttribArray( 0 );
		}

		protected:
			GLuint		_VertexArrayID;

	END_TOOLBOX_PLUGIN_DEF


	extern "C"
	{
		DEFINE_TOOLBOX_PLUGIN_FACTORY( Renderer, OpenGL )

		// Optional plugin event functions
		// void onLoad();
		// void onUnload();
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


