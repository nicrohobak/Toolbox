/*
 * SDL2-based App Window Plugin
 *
 * NOTE: This plugin requires -lSDL2
 */

#include <iostream>

#include <SDL2/SDL.h>

#include <Toolbox/Plugin/Interface/AppWindow.hpp>
#include <Toolbox/Plugin/Interface/Renderer2D.hpp>


namespace Toolbox
{
	//
	// AppWindow plugin
	//
	DEFINE_TOOLBOX_PLUGIN_D( AppWindow, SDL2Window )

		SDL2Window():
			_Window( NULL ),
			_OpenGLContext( NULL )
		{
		}

		virtual ~SDL2Window()
		{
			this->Destroy();
		}

		virtual void Create( const std::string &title, int width, int height, long int flags, int glMajorVer, int glMinorVer )
		{
			if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) )
				throw std::runtime_error( std::string("AppWindow(SDL2): Failed to initialize SDL: ") + SDL_GetError() );

			_Window = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags );

			if ( !_Window )
				throw std::runtime_error( std::string("AppWindow(SDL2): Failed to create the window: ") + SDL_GetError() );

			if ( !(flags & 1 << SDL_WINDOW_OPENGL) )
			{
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, glMajorVer );
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, glMinorVer );

				_OpenGLContext = SDL_GL_CreateContext( _Window );

				if ( !_OpenGLContext )
					throw std::runtime_error( std::string("AppWindow(SDL2): Failed to create the OpenGL context: ") + SDL_GetError() );
			}
		}

		virtual void Destroy() 
		{
			if ( _OpenGLContext )
				SDL_GL_DeleteContext( _OpenGLContext );

			if ( _Window )
				SDL_DestroyWindow( _Window );

			SDL_Quit();
		}

		virtual void GetPos( int *x, int *y )
		{
			int X = 0, Y = 0;

			if ( _Window )
				SDL_GetWindowPosition( _Window, &X, &Y );

			if ( x )
				*x = X;

			if ( y )
				*y = Y;
		}

		virtual void SetPos( int x, int y )
		{
			SDL_SetWindowPosition( _Window, x, y );
		}

		virtual void Center( bool x, bool y )
		{
			SDL_SetWindowPosition( _Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
		}

		virtual void Minimize()
		{
			SDL_MinimizeWindow( _Window );
		}

		virtual void Maximize()
		{
			SDL_MaximizeWindow( _Window );
		}

		virtual void Restore()
		{
			SDL_RestoreWindow( _Window );
		}

		virtual void Raise()
		{
			SDL_RaiseWindow( _Window );
		}

		virtual void Swap()
		{
			SDL_GL_SwapWindow( _Window );
		}


	protected:
		SDL_Window *	_Window;
		SDL_GLContext	_OpenGLContext;

	END_TOOLBOX_PLUGIN_DEF


	//
	// Renderer2D Plugin
	//
	DEFINE_TOOLBOX_PLUGIN( Renderer2D, SDL2Renderer )

		virtual void BeginFrame()
		{
			// TODO: Write me!
		}

		virtual void EndFrame()
		{
			// TODO: Write me!
		}

		virtual void Sprite_Delete( const std::string &name )
		{
			// TODO: Write me!
		}

		virtual void Sprite_Load( const std::string &name, const std::string &fileName )
		{
			// TODO: Write me!
		}

		virtual void Sprite_Render( const std::string &name )
		{
			// TODO: Write me!
		}

	END_TOOLBOX_PLUGIN_DEF


	extern "C"
	{
		DEFINE_TOOLBOX_PLUGIN_INFO( "SDL2",
									"0.1",
									"AppWindow, Renderer2D" )

		DEFINE_TOOLBOX_PLUGIN_FACTORY( AppWindow, SDL2Window )
		DEFINE_TOOLBOX_PLUGIN_FACTORY( Renderer2D, SDL2Renderer )

		// Optional plugin event functions
		// void onLoad();
		// void onUnload();
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


