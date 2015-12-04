#ifndef TOOLBOX_PLUGIN_INTERFACE_IMAGE_HPP
#define TOOLBOX_PLUGIN_INTERFACE_IMAGE_HPP

/*
 * Generic Image
 *
 * A generic, plugin-ready image interface.
 */

/*****************************************************************************
    // Our SDL2_Image-based Image_Plugin can load a variety of formats
	Toolbox::Image::LoadPlugin( "/usr/local/include/Toolbox/Plugin/Common/Image/SDL2_Image.so" );
	Toolbox::Image NewPNG;
	Toolbox::Image NewJPG( "/path/to/image.jpg" );		// Auto-detected by file extension
	NewPNG.Load( "/path/to/image.png", "SDL2 Image" );	// Forced load by a specific plugin
														// Load() and the constructor take the
														//   same arguments.
 ****************************************************************************/

#include <algorithm>

#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	//
	// Forward declaration so our Image_Plugin can accept an Image as a parameter
	//
	class Image;


	enum tColorChannel
	{
		Color_Red,
		Color_Green,
		Color_Blue,
		Color_Alpha,

		Color_MAX,
		Color_FIRST = Color_Red
	};


	//
	// Image_Plugin interface
	//
	DEFINE_TOOLBOX_PLUGIN_INTERFACE( Image_Plugin, "0.1" )

		typedef std::list< std::string >		tExtensionList;
		typedef std::vector< tColorChannel >	tColorChannels;

		virtual tExtensionList Extensions() const = 0;		// Returns a list of extensions handled by this plugin

		virtual void Load( const std::string &fileName, Image *r_Image = NULL ) = 0;
		virtual void Save( const std::string &fileName, const Image &img ) = 0;

	protected:
		//
		// Let plugins set select private variables when loading via our "friend" status
		//
		void setImageWidth( Image &img, size_t width );
		void setImageHeight( Image &img, size_t width );
		void setImagePitch( Image &img, size_t pitch );
		void setImageBPR( Image &img, size_t bitsPer_R );
		void setImageBPG( Image &img, size_t bitsPer_G );
		void setImageBPB( Image &img, size_t bitsPer_B );
		void setImageBPA( Image &img, size_t bitsPer_A );
		void setImageBitOrder( Image &img, const tColorChannels &order );
		void setImageData( Image &img, const std::string &data );

	END_TOOLBOX_PLUGIN_DEF


	//
	// Our generic, plugin-based Image class
	//
	class Image
	{
	public:
		TOOLBOX_MEMORY_POINTERS( Image )

	public:
		static const PluginManager &PluginMgr()
		{
			return _PluginMgr;
		}

		static void LoadPlugin( const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image::LoadPlugin(const std::string &)";
			auto NewPlugin = _PluginMgr.Load( fileName );

			// Make sure this plugin implements the interface we need (any valid version is fine for now)
			if ( !NewPlugin->Version("Image_Plugin").compare(Plugin::Invalid) )
			{
				_PluginMgr.Unload( NewPlugin->Name() );
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Invalid Image plugin." );
			}
		}

		static void UnloadPlugin( const std::string &name )
		{
			_PluginMgr.Unload( name );
		}

	public:
		Image( const std::string &fileName = std::string(""), const std::string &plugin = std::string("") ):
			_Width( 0 ),
			_Height( 0 )
		{
			if ( !fileName.empty() )
				Load( fileName, plugin );
		}

		void Load( const std::string &fileName, const std::string &plugin = std::string("") )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image::Load(const std::string &, const std::string &)";

			if ( _PluginMgr.Plugins().empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No Image plugins loaded!" );

			// Find which plugin we need to load the file with
			if ( !plugin.empty() )
			{
				// If explicitly given, lets use it
				auto ImgPlugin = _PluginMgr.Create< Image_Plugin >( plugin );
				ImgPlugin->Load( fileName, this );
			}
			else
			{
				std::string Extension( getExtFromFilename(fileName) );
				auto Plugins = _PluginMgr.Plugins();

				// If plugin is not provided, we search our plugins for the fileName's extension
				bool Found = false;
				for ( auto p = Plugins.begin(), p_end = Plugins.end(); p != p_end; ++p )
				{
					auto ImgPlugin = _PluginMgr.Create< Image_Plugin >( (*p)->Name() );

					if ( !ImgPlugin )
						throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to create plugin instance." );

					Image_Plugin::tExtensionList Extensions = ImgPlugin->Extensions();

					for ( auto e = Extensions.begin(), e_end = Extensions.end(); e != e_end; ++e )
					{
						if ( !e->compare(Extension) )
						{
							Found = true;
							ImgPlugin->Load( fileName, this );
							break;
						}
					}

					ImgPlugin.reset();
					Extensions.clear();

					if ( Found )
						break;
				}

				if ( !Found )
					throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to load image.  Couldn't find a plugin to load '" + Extension + "' files." );
			}
		}

		void Save( const std::string &fileName, const std::string &imgType = std::string(""), const std::string &plugin = std::string("") )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image::Save(const std::string &, const std::string &)";

			if ( _PluginMgr.Plugins().empty() )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No Image plugins loaded!" );

			// If type is provided, that plugin is used to save
			// If type is not provided, we search our plugins for the fileName's extension
		}

		size_t Width() const
		{
			return _Width;
		}

		size_t Height() const
		{
			return _Height;
		}

		size_t BitsPer_R() const
		{
			return _BitsPer_R;
		}

		size_t BitsPer_G() const
		{
			return _BitsPer_G;
		}

		size_t BitsPer_B() const
		{
			return _BitsPer_B;
		}

		size_t BitsPer_A() const
		{
			return _BitsPer_A;
		}

		const Image_Plugin::tColorChannels &BitOrder() const
		{
			return _BitOrder;
		}

		const char *RawData() const
		{
			return _Data.c_str();
		}

	protected:
		static PluginManager			_PluginMgr;

	protected:
		size_t							_Width;				// Width in pixels
		size_t							_Height;			// Height in pixels
		size_t							_Pitch;				// Number of bytes in a row of pixels

		// Bit/byte ordering information
		size_t							_BitsPer_R;
		size_t							_BitsPer_G;
		size_t							_BitsPer_B;
		size_t							_BitsPer_A;
		Image_Plugin::tColorChannels	_BitOrder;

		std::string						_Data;				// The raw image data

	protected:
		std::string getExtFromFilename( const std::string &fileName ) const
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image::getExtFromFilename(const std::string &)";
			std::string Ext("");

			if ( fileName.empty() )
				return Ext;

			auto LastDot = fileName.find_last_of( '.' );
			Ext = fileName.substr( ++LastDot );

			if ( !Ext.empty() )
				std::transform(Ext.begin(), Ext.end(), Ext.begin(), (int(*)(int))std::toupper);

			return Ext;
		}

		friend class Toolbox::Image_Plugin;
	};

	PluginManager Image::_PluginMgr;


	//
	// Image_Plugin helper functions
	//
	void Image_Plugin::setImageWidth( Image &img, size_t width )
	{
		img._Width = width;
	}

	void Image_Plugin::setImageHeight( Image &img, size_t height )
	{
		img._Height = height;
	}

	void Image_Plugin::setImagePitch( Image &img, size_t pitch )
	{
		img._Pitch = pitch;
	}

	void Image_Plugin::setImageBPR( Image &img, size_t bitsPer_R )
	{
		img._BitsPer_R = bitsPer_R;
	}

	void Image_Plugin::setImageBPG( Image &img, size_t bitsPer_G )
	{
		img._BitsPer_G = bitsPer_G;
	}

	void Image_Plugin::setImageBPB( Image &img, size_t bitsPer_B )
	{
		img._BitsPer_B = bitsPer_B;
	}

	void Image_Plugin::setImageBPA( Image &img, size_t bitsPer_A )
	{
		img._BitsPer_A = bitsPer_A;
	}

	void Image_Plugin::setImageBitOrder( Image &img, const tColorChannels &order )
	{
		img._BitOrder = order;
	}

	void Image_Plugin::setImageData( Image &img, const std::string &data )
	{
		img._Data = data;
	}
}


#endif // TOOLBOX_PLUGIN_INTERFACE_IMAGE_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


