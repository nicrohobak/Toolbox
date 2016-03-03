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

#include <Toolbox/AppWindow.hpp>
#include <Toolbox/Renderer.hpp>

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

		// Set AppWindow options to create an OpenGL 3.3 rendering context
		AppWin->SetOption( Toolbox::AppWindowOption_OpenGL_Enabled,  true );
		AppWin->SetOption( Toolbox::AppWindowOption_OpenGL_MajorVer, 3 );
		AppWin->SetOption( Toolbox::AppWindowOption_OpenGL_MinorVer, 3 );

		// Create our SDL application window
		AppWin->Create( "Test App", 800, 600 );

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

#include <Toolbox/Renderer3D.hpp>

#include <GLES3/gl3.h>


namespace Toolbox
{
/*
	GLuint vertexShader, fragmentShader;

	// Create the shader objects
	vertexShader = glCreateShader( GL_VERTEX_SHADER );
	fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

	// Attach(reassign) shader source
	glShaderSourceARB( vertexShader, 1, &VertexShaderSource, &vsLength );
	glShaderSourceARB( fragmentShader, 1, &FragmentShaderSource, &fsLength );

	// Compile
	glCompileShaderARB( vertexShader );
	glCompileShaderARB( fragmentShader );

	// Check compilation status
	GLint compiled;

	glGetObjectParameteriv( vertexShader, GL_COMPILE_STATUS, &compiled );

	if ( compiled )
		; // Success

	// Error info
	GLint b_len = 0;
	GLsizei s_len = 0;

	getShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &b_len );

	if ( b_len > 1 )
	{
		GLchar *compiler_log = (GLchar *)malloc( b_len );

		glGetInfoLogARB( vertexShader, b_len, &s_len, compiler_log );

		std::cout << "Error: " << compiler_log << std::endl;

		free( compiler_log );
	}


	// Linking
	GLuint ShaderProgram = glCreateProgram();

	glAttachShader( ShaderProgram, vertexShader );
	glAttachShader( ShaderProgram, fragmentShader );

	glLinkProgram( ShaderProgram );


	// Error checking
	GLuint linked;

	glGetProgramiv( ShaderProgram, GL_LINK_STATUS &linked );

	if ( linked )
		; // Success


	// Using
	glUseProgram( ShaderProgram );

*/


	class GLShader
	{
	public:
		TOOLBOX_MEMORY_POINTERS( GLShader )

	public:
		GLuint		ID;
		Shader::Ptr	Data;

	public:
		GLShader( GLuint id = GL_INVALID_VALUE, Shader::Ptr shader = Shader::Ptr() ):
			ID( id ),
			Data( shader )
		{
		}
	};


	class GLModel
	{
	public:
		TOOLBOX_MEMORY_POINTERS( GLModel )

	public:
		GLuint		ID;
		Model::Ptr	Data;

	public:
		GLModel( GLuint id = GL_INVALID_VALUE, Model::Ptr model = Model::Ptr() ):
			ID( id ),
			Data( model )
		{
		}
	};


	TOOLBOX_DEFINE_PLUGIN_D( Renderer3D, GLRenderer )

		typedef std::map< std::string, GLShader >	tShaderMap;
		typedef std::map< std::string, GLModel >	tModelMap;

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
				glDeleteBuffers( 1, &m->second.ID );
		}

		//
		// Core
		//
		virtual void PrepareScene()
		{
			//constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::PrepareScene()";
	
			// Iterate through each model
			//   Build a shader program for every unique combination
			//   Assign shader program IDs to each model
			//   Add each model to a list based on its shader program ID
		}

		virtual void BeginFrame()
		{
			//constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::BeginFrame()";
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			glEnableVertexAttribArray( 0 );
		}

		virtual void EndFrame()
		{
			//constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::EndFrame()";
			glDisableVertexAttribArray( 0 );
		}

		//
		// Shaders
		//
		virtual void Shader_Load( const std::string &shader, const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Shader_Load(const std::string &, const std::string &)";


			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual Shader::Ptr Shader_Get( const std::string &shader )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Shader_Get(const std::string &)";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Shader_Delete( const std::string &shader )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Shader_Delete(const std::string &)";

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		//
		// Textures
		//
		virtual void Texture_Load( const std::string &texture, const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Texture_Load(const std::string &, const std::string &)";


			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual Texture::Ptr Texture_Get( const std::string &texture )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Texture_Get(const std::string &)";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Texture_Delete( const std::string &texture )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Texture_Delete(const std::string &)";

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		//
		// Models
		//
		virtual void Model_Load( const std::string &model, const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Model_Load(const std::string &, const std::string &)";

			if ( model.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model name provided." );

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual Model::Ptr Model_Get( const std::string &model )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Model_Get(const std::string &)";
			Model::Ptr ReturnModel;

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
			return ReturnModel;
		}

		virtual void Model_Delete( const std::string &model )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Model_Delete(const std::string &)";

			if ( model.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model name provided." );

			auto m = _Models.find( model );

			if ( m == _Models.end() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Model '" + model + " not found." );

			glDeleteBuffers( 1, &(m->second.ID) );
			_Models.erase( m );
		}

		virtual void Model_AddShader( const std::string &model, const std::string &shader )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Model_AddShader(const std::string &, const std::string &)";

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Model_RemoveShader( const std::string &model, const std::string &shader )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Model_RemoveShader(const std::string &, const std::string &)";

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		virtual void Model_SetVertices( const std::string &model, size_t numVertices, const float *vertices )
		{
			constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::Model_SetVertices(const std::string &, size_t, const float *)";

			if ( model.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model name provided." );

			if ( !vertices || numVertices == 0 )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No model data provided." );

			GLModel CurModel = model_GetOrCreateID( model );

			glBindBuffer( GL_ARRAY_BUFFER, CurModel.ID );
			glBufferData( GL_ARRAY_BUFFER, numVertices, vertices, GL_STATIC_DRAW );

			if ( glGetError() != GL_NO_ERROR )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Error setting buffer data." );
		}

		virtual void Model_Render( const std::string &model )
		{
			glBindBuffer( GL_ARRAY_BUFFER, _Models[model].ID );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
			glDrawArrays( GL_TRIANGLES, 0, 3 );
		}

		protected:
			GLuint		_VertexArrayID;
			tShaderMap	_Shaders;
			tModelMap	_Models;

		protected:
			// Gets the current ID, or creates a new one
			GLModel &model_GetOrCreateID( const std::string &model )
			{
				constexpr const char *dbg_CurFunc = "Renderer3D::GLRenderer::model_GetOrCreateID(const std::string &)";

				auto m = _Models.find( model );

				// If it doesn't exist yet, create it
				if ( m == _Models.end() )
				{
					GLuint ID = GL_INVALID_VALUE;
					glGenBuffers( 1, &ID );

					if ( glGetError() != GL_NO_ERROR )
						throw std::runtime_error( std::string(dbg_CurFunc) + ": Couldn't find/create model ID." );

					_Models[ model ] = GLModel( ID );
					return _Models[ model ];
				}
				else
					return m->second;
			}

	TOOLBOX_END_PLUGIN_DEF


	extern "C"
	{
		TOOLBOX_DEFINE_PLUGIN_INFO( "OpenGL",
									"0.1",
									"Renderer3D" )

		TOOLBOX_DEFINE_PLUGIN_FACTORY( Renderer3D, GLRenderer )


		// Optional plugin event functions
		// void onLoad();
		// void onUnload();
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


