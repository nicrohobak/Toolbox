#ifndef TOOLBOX_RENDERER3D_HPP
#define TOOLBOX_RENDERER3D_HPP

/*
 * Generic 3D Rendering Interface
 *
 * A simple, generic interface for creating a 3D rendering context.
 */


/* NOTE: Plugin Required! ****************************************************

	 * Default plugin implementation available at:
		<Toolbox/Plugin/Common/Renderer3D>

	* For more information, please refer to <Toolbox/Plugin/README>

 ****************************************************************************/


/*****************************************************************************
    // Example use:
	Toolbox::PluginManager PluginManager;
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Multiple/SDL2.so" );
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Renderer3D/OpenGL.so" );

	Toolbox::AppWindow::Ptr AppWin = PluginManager.Create< Toolbox::AppWindow >( "SDL2" );
	Toolbox::Renderer3D::Ptr Renderer = PluginManager.Create< Toolbox::Renderer3D >( "OpenGL" );

	Renderer->Model_Load( "MyModel", "./MyModel.3d" );

	// Create an SDL window with an OpenGL 3.3 rendering context
	AppWin->Create( "SDL2 Window", 800, 600, SDL_WINDOW_OPENGL, 3, 3 );
		Renderer->BeginFrame();
			Renderer->Model_Render( "MyModel" );
		Renderer->EndFrame();
		AppWin->Swap();					// Swap VRAM
 	AppWin->Destroy();
 ****************************************************************************/

// Proposed:
/*****************************************************************************
    // Example use:
	Toolbox::PluginManager PluginManager;
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Multiple/SDL2.so" );
	PluginManager.Load( "/usr/local/include/Toolbox/Plugin/Common/Renderer3D/OpenGL.so" );

	Toolbox::AppWindow::Ptr AppWin = PluginManager.Create< Toolbox::AppWindow >( "SDL2" );
	Toolbox::Renderer3D::Ptr Renderer = PluginManager.Create< Toolbox::Renderer3D >( "OpenGL" );

	// Prepare Toolbox::Image with a plugin that can load our .png texture
	Toolbox::Image::LoadPlugin( "/usr/local/include/Toolbox/Plugin/Image/SDL2_Image.so" );

	// Prepare Toolbox::Model with a plugin that can load our mesh
	//Toolbox::Model::LoadPlugin( "/usr/local/include/Toolbox/Plugin/Renderer3D/plugin.so" );

	// Create an SDL window with an OpenGL 3.3 rendering context
	AppWin->SetOption( AppWindowOption_OpenGL_Enable, true );
	AppWin->SetOption( AppWindowOption_OpenGL_MajorVer, 3 );
	AppWin->SetOption( AppWindowOption_OpenGL_MinorVer, 3 );

	AppWin->Create( "SDL2/OpenGL", 800, 600 );

		// Load the shaders from their source files
		Renderer->Shader_Load( "Vertex", "./vertex.glsl" );
		Renderer->Shader_Load( "Fragment", "./fragment.glsl" );

		// Load texture data
		Renderer->Texture_Load( "Texture", "./texture.png" );

		// Load model data
		Renderer->Model_Load( "MyModel", "./MyModel.3d" );

		// Texture Assignment
		Renderer->Model_SetTexture( "MyModel", "Texture" );

		// Shader Assignment
		Renderer->Model_AddShader( "MyModel", "Vertex" );
		Renderer->Model_AddShader( "MyModel", "Fragment" );

		// Create Shader Programs and basic Render Lists
		Renderer->PrepareScene();

		Renderer->BeginFrame();
			Renderer->Model_Render( "MyModel" );
		Renderer->EndFrame();
		AppWin->Swap();					// Swap VRAM
 	AppWin->Destroy();
 ****************************************************************************/

/*

 * Things we interact with stay live in system RAM under application control

 * Things we put into VRAM can be manipulated via their system RAM handle objects

 * Things in VRAM are removed from VRAM when destroyed by the application or when explicitly removed.

*/


#include <fstream>

#include <Toolbox/Image.hpp>


namespace Toolbox
{
	//
	// Shader Types
	//
	enum tShader
	{
		Shader_Invalid,

		Shader_Vertex,
		Shader_Fragment,
		Shader_Geometry,

		Shader_Tesselation,
		Shader_Evaluation,
		Shader_Compute,

		Shader_MAX,
		Shader_FIRST = Shader_Vertex
	};


	//
	// Shaders
	//
	class Shader
	{
	public:
		TOOLBOX_MEMORY_POINTERS_AND_LISTS( Shader )

	public:
		Shader( const std::string &fileName = std::string(), tShader type = Shader_Invalid ):
			_Type( Shader_Invalid )
		{
			if ( !fileName.empty() && type != Shader_Invalid )
				Load( fileName, type );
		}

		void Load( const std::string &fileName, tShader type )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Shader::Load(const std::string &, tShader)";

			_Type = Shader_Invalid;

			if ( fileName.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No filename provided." );

			std::ifstream File( fileName );

			while ( File.good() )
				_Source.push_back( File.get() );

			if ( _Source.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Source file empty." );

			_Type = type;
		}

	protected:
		tShader		_Type;
		std::string	_Source;
	};


	//
	// Textures -- Essentially just a wrapper for Toolbox::Image
	//
	class Texture
	{
	public:
		TOOLBOX_MEMORY_POINTERS_AND_LISTS( Texture )

	public:
		Texture( const std::string &fileName = std::string(""), const std::string &imagePlugin = std::string("") )
		{
			if ( !fileName.empty() )
				Load( fileName, imagePlugin );
		}

		void Load( const std::string &fileName, const std::string &imagePlugin = std::string("") )
		{
			_Source.Load( fileName, imagePlugin );
		}

	protected:
		Image	_Source;
	};


	//
	// Forward declaration so Model_Plugin can accept Model parameters
	//
	class Model;


	//
	// Model_Plugin
	//
	TOOLBOX_DEFINE_PLUGIN_INTERFACE( Model_Plugin, "0.1" )

		typedef std::list< std::string >		tExtensionList;

		virtual tExtensionList Extensions() const = 0;		// Returns a list of extensions handled by this plugin

		virtual void Load( const std::string &fileName, Model *r_Model = NULL ) = 0;
		virtual void Save( const std::string &fileName, const Model &model ) = 0;

	protected:
		//
		// Let plugins set private variables when loading via our "friend" status
		//
/*
		void setModelTexture( Model &model, const Texture &texture );
*/

	TOOLBOX_END_PLUGIN_DEF


	//
	// Models
	//
	class Model
	{
	public:
		TOOLBOX_MEMORY_POINTERS_AND_LISTS( Model )
		TOOLBOX_DEFINE_STATIC_PLUGIN_MGR( Model, Model_Plugin )

	public:
		Model( const std::string &fileName = std::string(""), const std::string &plugin = std::string("") )
		{
			if ( !fileName.empty() )
				Load( fileName, plugin );
		}

		// TODO: Look into ways to make this function more generic for different plugin types
		//       Template function in Plugin.hpp maybe?
		void Load( const std::string &fileName, const std::string &plugin = std::string("") )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Model::Load(const std::string &, const std::string &)";

			if ( fileName.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No file name provided." );

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		void Save( const std::string &fileName, const std::string &plugin = std::string("") )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Model::Save(const std::string &, const std::string &)";

			if ( fileName.empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No file name provided." );

			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented.  (Sorry!)" );
		}

		// Vertices
		// Texture
		// Shader(s)

		friend class Toolbox::Model_Plugin;
	};

	TOOLBOX_DECLARE_STATIC_PLUGIN_MGR( Model )


	//
	// Model_Plugin helper functions
	//
/*
	void Model_Plugin::setModelTexture( Model &model, const Texture &texture )
	{
		model.SetTexture( texture );
	}
*/


	//
	// Renderer3D -- The main renderer interface
	//
	TOOLBOX_DEFINE_PLUGIN_INTERFACE( Renderer3D, "0.1" )

		//
		// Core
		//
		virtual void PrepareScene() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		//
		// Shaders
		// 
		virtual void Shader_Load( const std::string &shader, const std::string &fileName ) = 0;
		virtual Shader::Ptr Shader_Get( const std::string &shader ) = 0;
		virtual void Shader_Delete( const std::string &shader ) = 0;

		//
		// Textures
		//
		virtual void Texture_Load( const std::string &texture, const std::string &fileName ) = 0;
		virtual Texture::Ptr Texture_Get( const std::string &texture ) = 0;;
		virtual void Texture_Delete( const std::string &texture ) = 0;

		//
		// Models
		//
		virtual void Model_Load( const std::string &model, const std::string &fileName ) = 0;
		virtual Model::Ptr Model_Get( const std::string &model ) = 0;
		virtual void Model_Delete( const std::string &model ) = 0;

		// Add specific shaders to a model and calculate/assign when the model is moved to VRAM
		virtual void Model_AddShader( const std::string &model, const std::string &shader ) = 0;
		virtual void Model_RemoveShader( const std::string &model, const std::string &shader ) = 0;

		virtual void Model_SetVertices( const std::string &model, size_t numVertices, const float *vertices ) = 0;
		virtual void Model_Render( const std::string &model ) = 0;

	TOOLBOX_END_PLUGIN_DEF
}


#endif // TOOLBOX_RENDERER3D_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


