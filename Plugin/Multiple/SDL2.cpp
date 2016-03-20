/*
 * SDL2 Plugin
 * Implements AppWindow and Renderer2D
 *
 * Link plugin with: -fPIC -shared -lSDL2
 * Link main app with: -ldl
 */

#include <iostream>
#include <map>

#include <SDL2/SDL.h>

#include <Toolbox/AppWindow.hpp>
#include <Toolbox/Renderer2D.hpp>


namespace Toolbox
{
	//
	// AppWindow plugin
	//
	TOOLBOX_DEFINE_PLUGIN_D( AppWindow, SDL2Window )

		SDL2Window():
			_Window( NULL ),
			_OpenGLContext( NULL )
		{
		}

		virtual ~SDL2Window()
		{
			this->Destroy();
		}

		virtual void Create( const std::string &title, int width, int height )
		{
			constexpr const char *dbg_CurFunc = "AppWindow::SDL2Window::Create(const std::string &, int, int)";

			if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to initialize SDL: " + SDL_GetError() );

			long int WindowFlags = 0;

			if ( _WindowOptions[ AppWindowOption_OpenGL_Enabled ] != 0 )
				WindowFlags |= SDL_WINDOW_OPENGL;


			_Window = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, WindowFlags );

			if ( !_Window )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to create the window: " + SDL_GetError() );

			// If we're creating an OpenGL context, do that here
			if ( !(WindowFlags & 1 << SDL_WINDOW_OPENGL) )
			{
				int GLMajorVer = this->GetOption( AppWindowOption_OpenGL_MajorVer );
				int GLMinorVer = this->GetOption( AppWindowOption_OpenGL_MinorVer );;

				// Default version is 3.3
				if ( GLMajorVer == 0 )
				{
					GLMajorVer = 3;
					GLMinorVer = 3;
				}
				
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVer );
				SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, GLMinorVer );

				_OpenGLContext = SDL_GL_CreateContext( _Window );

				if ( !_OpenGLContext )
					throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to create the OpenGL context: " + SDL_GetError() );
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

		virtual void Delay( size_t milliseconds )
		{
			SDL_Delay( milliseconds );
		}

		virtual void Swap()
		{
			SDL_GL_SwapWindow( _Window );
		}

	protected:
		SDL_Window *		_Window;
		SDL_GLContext		_OpenGLContext;

	TOOLBOX_END_PLUGIN_DEF


	//
	// Renderer2D Plugin
	//
	TOOLBOX_DEFINE_PLUGIN( Renderer2D, SDL2Renderer )

		virtual void BeginFrame()
		{
			constexpr const char *dbg_CurFunc = "Renderer2D::SDL2Renderer::BeginFrame()";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void EndFrame()
		{
			constexpr const char *dbg_CurFunc = "Renderer2D::SDL2Renderer::EndFrame()";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Sprite_Delete( const std::string &name )
		{
			constexpr const char *dbg_CurFunc = "Renderer2D::SDL2Renderer::Sprite_Delete(const std::string &)";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Sprite_Load( const std::string &name, const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "Renderer2D::SDL2Renderer::Sprite_Load(const std::string &)";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Sprite_Render( const std::string &name )
		{
			constexpr const char *dbg_CurFunc = "Renderer2D::SDL2Renderer::Sprite_Render(const std::string &)";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

	TOOLBOX_END_PLUGIN_DEF


	extern "C"
	{
		TOOLBOX_DEFINE_PLUGIN_INFO( "SDL2",
									"0.1",
									"AppWindow, Renderer2D" )

		TOOLBOX_DEFINE_PLUGIN_FACTORY( AppWindow, SDL2Window )
		TOOLBOX_DEFINE_PLUGIN_FACTORY( Renderer2D, SDL2Renderer )

		// Optional plugin event functions
		// void onLoad();
		// void onUnload();
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


