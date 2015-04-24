/*
 * SDL-based App Window Plugin
 *
 * NOTE: This plugin requires -lSDL2
 */

#include <Toolbox/Plugin.hpp>
#include <Toolbox/Plugin/Interface/AppWindow.hpp>

#include <SDL2/SDL.h>


DEFINE_TOOLBOX_PLUGIN( AppWindow, SDL )

	virtual void Create( const std::string &title, int width, int height, long int flags )
	{
		if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) )
			throw std::runtime_error( std::string("AppWindow(SDL): Failed to initialize SDL: ") + SDL_GetError() );

		_Window = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags );

		if ( !_Window )
			throw std::runtime_error( std::string("AppWindow(SDL): Failed to create the window: ") + SDL_GetError() );

	}

	virtual void Destroy() 
	{
		if ( _Window )
			SDL_DestroyWindow( _Window );
	}

protected:
    SDL_Window *	_Window;

END_TOOLBOX_PLUGIN_DEF


extern "C"
{
	DEFINE_TOOLBOX_PLUGIN_FACTORY( AppWindow, SDL )

	// Optional plugin event functions
	// void onLoad();
	// void onUnload();
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


