#ifndef TOOLBOX_PLUGIN_INTERFACE_RENDERER_HPP
#define TOOLBOX_PLUGIN_INTERFACE_RENDERER_HPP

/*
 * Generic GL Rendering Interface
 *
 * A simple, generic interface for creating a graphics library rendering context.
 */

/*****************************************************************************
 ****************************************************************************/


#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	DEFINE_TOOLBOX_PLUGIN_INTERFACE( Renderer, 0.1 )

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

	END_TOOLBOX_PLUGIN_DEF
}


#endif // TOOLBOX_PLUGIN_INTERFACE_RENDERER_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


