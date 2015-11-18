/*
 * SDL2 Image TGA Plugin
 *
 * Link plugin with: -fPIC -shared -lSDL2_Image
 * Link main app with: -ldl
 */

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
		List.push_back( std::string("TGA") );
		List.push_back( std::string("TARGA") );
		return List;
	}


	extern "C"
	{
		DEFINE_TOOLBOX_PLUGIN_INFO( "SDL2 TGA",
									"0.1",
									"Image_Plugin" )

		DEFINE_TOOLBOX_PLUGIN_FACTORY( Image_Plugin, SDL2_Image )

		// Optional plugin event functions
		//void onLoad();
		//void onUnload();
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


