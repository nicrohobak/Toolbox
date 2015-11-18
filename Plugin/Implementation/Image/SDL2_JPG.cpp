/*
 * SDL2 Image Plugin
 * Implements many different Image_Loader plugins
 *
 * Link plugin with: -fPIC -shared -lSDL2_Image
 * Link main app with: -ldl
 */

#include <iostream>

#include <SDL2/SDL_image.h>

#include <Toolbox/Plugin/Interface/Image.hpp>

#include "SDL2_Image.h"


namespace Toolbox
{
	//
	// Primary Image_Plugin implementation (SDL2_Image) defined in "SDL2_Image.h"
	//
	Image_Plugin::tExtensionList SDL2_Image::Extensions() const
	{
		Image_Plugin::tExtensionList List;
		List.push_back( std::string("JPG") );
		List.push_back( std::string("JPEG") );
		return List;
	}


	extern "C"
	{
		DEFINE_TOOLBOX_PLUGIN_INFO( "SDL2 JPG",
									"0.1",
									"Image_Plugin" )

		DEFINE_TOOLBOX_PLUGIN_FACTORY( Image_Plugin, SDL2_Image )

		// Optional plugin event functions
		void onLoad()
		{
			constexpr const char *dbg_CurFunc = "SDL2_JPG::onLoad()";

			int status = IMG_Init( IMG_INIT_JPG );

			if ( (status & IMG_INIT_JPG) != IMG_INIT_JPG )
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


