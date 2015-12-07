/*
 * SDL2_Image-based Image_Plugin
 *
 * Link plugin with: -fPIC -shared -lSDL2_Image
 * Link main app with: -ldl
 */

#include <iostream>

#include <SDL2/SDL_image.h>

#include <Toolbox/Image.hpp>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN( Image_Plugin, SDL2_Image )

		virtual tExtensionList Extensions() const
		{
			Image_Plugin::tExtensionList List;
			List.push_back( std::string("BMP") );
			List.push_back( std::string("CUR") );
			List.push_back( std::string("GIF") );
			List.push_back( std::string("ICO") );
			List.push_back( std::string("JPG") );
			List.push_back( std::string("LBM") );
			List.push_back( std::string("PCX") );
			List.push_back( std::string("PNG") );
			List.push_back( std::string("PNM") );
			List.push_back( std::string("TGA") );
			List.push_back( std::string("TIF") );
			List.push_back( std::string("XCF") );
			List.push_back( std::string("XPM") );
			List.push_back( std::string("XV") );
			return List;
		}

		virtual void Load( const std::string &fileName, Image *r_Img )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image_Plugin(SDL2_Image)::Load(const std::string &, Toolbox::Image *)";

			if ( !r_Img )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": No return image pointer provided." );

			SDL_Surface *img = IMG_Load( fileName.c_str() );

			if ( !img )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to load '" + fileName + "'" );

			tColorChannels BitOrder;

			switch ( img->format->format )
			{
				case SDL_PIXELFORMAT_RGB332:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					Image_Plugin::setImageBPR( *r_Img, 3 );
					Image_Plugin::setImageBPG( *r_Img, 3 );
					Image_Plugin::setImageBPB( *r_Img, 2 );
					Image_Plugin::setImageBPA( *r_Img, 0 );
					break;

				case SDL_PIXELFORMAT_RGB444:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					Image_Plugin::setImageBPR( *r_Img, 4 );
					Image_Plugin::setImageBPG( *r_Img, 4 );
					Image_Plugin::setImageBPB( *r_Img, 4 );
					Image_Plugin::setImageBPA( *r_Img, 0 );
					break;

				case SDL_PIXELFORMAT_RGB555:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_BGR555:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
					}
					Image_Plugin::setImageBPR( *r_Img, 5 );
					Image_Plugin::setImageBPG( *r_Img, 5 );
					Image_Plugin::setImageBPB( *r_Img, 5 );
					Image_Plugin::setImageBPA( *r_Img, 0 );
					break;

				case SDL_PIXELFORMAT_ARGB4444:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Alpha );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_RGBA4444:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Alpha );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_ABGR4444:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Alpha );
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_BGRA4444:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Alpha );
					}
					Image_Plugin::setImageBPR( *r_Img, 4 );
					Image_Plugin::setImageBPG( *r_Img, 4 );
					Image_Plugin::setImageBPB( *r_Img, 4 );
					Image_Plugin::setImageBPA( *r_Img, 4 );
					break;

				case SDL_PIXELFORMAT_ARGB1555:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Alpha );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_RGBA5551:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Alpha );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_ABGR1555:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Alpha );
					}
					Image_Plugin::setImageBPR( *r_Img, 5 );
					Image_Plugin::setImageBPG( *r_Img, 5 );
					Image_Plugin::setImageBPB( *r_Img, 5 );
					Image_Plugin::setImageBPA( *r_Img, 1 );
					break;

				case SDL_PIXELFORMAT_RGB565:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_BGR565:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
					}
					Image_Plugin::setImageBPR( *r_Img, 5 );
					Image_Plugin::setImageBPG( *r_Img, 6 );
					Image_Plugin::setImageBPB( *r_Img, 5 );
					Image_Plugin::setImageBPA( *r_Img, 0 );
					break;

				case SDL_PIXELFORMAT_RGB24:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_BGR24:
					Image_Plugin::setImageBPR( *r_Img, 6 );
					Image_Plugin::setImageBPG( *r_Img, 6 );
					Image_Plugin::setImageBPB( *r_Img, 6 );
					Image_Plugin::setImageBPA( *r_Img, 6 );
					break;

				case SDL_PIXELFORMAT_ARGB8888:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Alpha );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_RGBA8888:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Alpha );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_ABGR8888:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Alpha );
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
					}
					// Purposeful fall-through
				case SDL_PIXELFORMAT_BGRA8888:
					if ( BitOrder.empty() )
					{
						BitOrder.push_back( Color_Blue  );
						BitOrder.push_back( Color_Green );
						BitOrder.push_back( Color_Red   );
						BitOrder.push_back( Color_Alpha );
					}
					Image_Plugin::setImageBPR( *r_Img, 8 );
					Image_Plugin::setImageBPG( *r_Img, 8 );
					Image_Plugin::setImageBPB( *r_Img, 8 );
					Image_Plugin::setImageBPA( *r_Img, 8 );
					break;

				case SDL_PIXELFORMAT_UNKNOWN:
				default:
					throw std::runtime_error( std::string(dbg_CurFunc) + ": Unhandled/unknown image format." );
					break;
			}

			// Copy the image data to our return Image
			Image_Plugin::setImageWidth( *r_Img, img->w );
			Image_Plugin::setImageHeight( *r_Img, img->h );
			Image_Plugin::setImagePitch( *r_Img, img->pitch );
			Image_Plugin::setImageBitOrder( *r_Img, BitOrder );
			Image_Plugin::setImageData( *r_Img, std::string((const char *)img->pixels, (size_t)(img->pitch * img->h)) );

			SDL_FreeSurface( img );
		}
				
		virtual void Save( const std::string &fileName, const Image &img )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image_Plugin(SDL2_Image)::Save(const std::string &, const Image &)";
			throw std::runtime_error( std::string(dbg_CurFunc) + ": Not implemented!" );
		}

	END_TOOLBOX_PLUGIN_DEF


	extern "C"
	{
		DEFINE_TOOLBOX_PLUGIN_INFO( "SDL2 Image",
									"0.1",
									"Image_Plugin" )

		DEFINE_TOOLBOX_PLUGIN_FACTORY( Image_Plugin, SDL2_Image )

		// Optional plugin event functions
		void onLoad()
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Image_Plugin(SDL2_Image)::onLoad()";

			int flags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF;
			int status = IMG_Init( flags );

			if ( (status & flags) != flags )
				throw std::runtime_error( std::string(dbg_CurFunc) + ": Failed to initialize SDL2_Image." );
		}

		void onUnload()
		{
			IMG_Quit();
		}
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


