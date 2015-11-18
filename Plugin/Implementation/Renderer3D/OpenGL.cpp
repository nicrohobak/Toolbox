/*
 * OpenGL Renderer Plugin
 *
 * Link plugin with: -fPIC -shared -lGL
 * Link main app with: -ldl (and -lSDL2, if using the example below)
 */

/*****************************************************************************
// Example use
#include <iostream>
#include <stdexcept>

#include <Toolbox/Plugin.hpp>
#include <Toolbox/Plugin/Interface/AppWindow.hpp>
#include <Toolbox/Plugin/Interface/Renderer.hpp>

#include <SDL2/SDL.h>		// For SDL_Delay()


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

	try
	{
		Toolbox::PluginManager PluginManager;
		PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Multiple/SDL2.so" );
		PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Renderer/OpenGL.so" );

		Toolbox::AppWindow::Ptr AppWin = PluginManager.Create< Toolbox::AppWindow >( "SDL2" );
		Toolbox::Renderer::Ptr Renderer = PluginManager.Create< Toolbox::Renderer >( "OpenGL" );

		// Create an SDL application window with an OpenGL 3.3 rendering context
		// TODO: Make AppWindow::Create() properly generic
		AppWin->Create( "Test App", 800, 600, SDL_WINDOW_OPENGL, 3, 3 );

		// Establish vertices for a simple triangle
		const float TriangleData[] =
		{    
			 0.0f,	 0.5f,	 0.0f,
			 0.5f,	-0.5f,	 0.0f,
			-0.5f,	-0.5f,	 0.0f,
		};

		// Create the Triangle model as we set its vertices
		Renderer->Model_SetVertices( "Triangle", sizeof(TriangleData), TriangleData );

		Renderer->BeginFrame();						// Prepare the renderer
			Renderer->Model_Render( "Triangle" );	// Render our triangle
		Renderer->EndFrame();						// Finalize the renderer

		AppWin->Swap();								// Swap the frame buffers to display
													//   the rendered image
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

#include <map>

#include <Toolbox/Plugin.hpp>
#include <Toolbox/Plugin/Interface/Renderer3D.hpp>

#include <GLES3/gl3.h>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_D( Renderer3D, GLRenderer )

		typedef std::map< std::string, GLuint >	tModelMap;

		GLRenderer()
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

			glGenVertexArrays( 1, &_VertexArrayID );
			glBindVertexArray( _VertexArrayID );
		}

		virtual ~GLRenderer()
		{
			if ( _VertexArrayID != GL_INVALID_VALUE )
				glDeleteVertexArrays( 1, &_VertexArrayID );

			for ( auto m = _Models.begin(), m_end = _Models.end(); m != m_end; ++m )
				glDeleteBuffers( 1, &m->second );
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

		virtual void Model_Delete( const std::string &name )
		{
			constexpr const char *dbg_CurFunc = "OpenGL::GLRenderer::Model_Delete(const std::string &)";

			if ( name.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model name provided." );

			auto m = _Models.find( name );

			if ( m == _Models.end() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Model '" + name + " not found." );

			glDeleteBuffers( 1, &(m->second) );

			_Models.erase( m );
		}

		virtual void Model_Load( const std::string &name, const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "OpenGL::GLRenderer::Model_Load(const std::string &, const std::string &)";

			if ( name.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model name provided." );

			return;

			// TODO: Write me
		}

		virtual void Model_SetVertices( const std::string &name, size_t numVertices, const float *vertices )
		{
			constexpr const char *dbg_CurFunc = "OpenGL::GLRenderer::Model_SetVertices(const std::string &, size_t, const float *)";

			if ( name.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model name provided." );

			if ( !vertices || numVertices == 0 )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model data provided." );
			
			GLuint ID = model_GetOrCreateID( name );

			glBindBuffer( GL_ARRAY_BUFFER, ID );
			glBufferData( GL_ARRAY_BUFFER, numVertices, vertices, GL_STATIC_DRAW );

			if ( glGetError() != GL_NO_ERROR )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Error setting buffer data." );
		}

		virtual void Model_Render( const std::string &name )
		{
			glBindBuffer( GL_ARRAY_BUFFER, _Models[name] );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
			glDrawArrays( GL_TRIANGLES, 0, 3 );
		}

		protected:
			GLuint		_VertexArrayID;
			tModelMap	_Models;

		protected:
			// Gets the current ID, or creates a new one
			GLuint model_GetOrCreateID( const std::string &name )
			{
				constexpr const char *dbg_CurFunc = "OpenGL::GLRenderer::model_GetOrCreateID(const std::string &)";

				GLuint ID = GL_INVALID_VALUE;

				auto m = _Models.find( name );

				// If it doesn't exist yet, create it
				if ( m == _Models.end() )
				{
					glGenBuffers( 1, &ID );

					if ( glGetError() != GL_NO_ERROR )
						throw std::runtime_error( std::string(dbg_CurFunc) + ": Couldn't find/create model ID." );

					_Models[ name ] = ID;
				}
				else
					ID = m->second;

				return ID;
			}

	END_TOOLBOX_PLUGIN_DEF


	extern "C"
	{
		DEFINE_TOOLBOX_PLUGIN_INFO( "OpenGL",
									"0.1",
									"Renderer3D" )

		DEFINE_TOOLBOX_PLUGIN_FACTORY( Renderer3D, GLRenderer )


		// Optional plugin event functions
		// void onLoad();
		// void onUnload();
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


