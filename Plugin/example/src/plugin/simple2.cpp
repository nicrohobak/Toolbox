/*
 * SimpleTask plugin implementation called SimpleImple2
 */

#include <Toolbox/Plugin.hpp>

#include "../SimpleTask_interface.h"


DEFINE_TOOLBOX_PLUGIN( SimpleTask, SimpleImple2 )

	virtual const std::string &Operation()
	{
		static std::string Op( "* 10" );
		return Op;
	}

	virtual int Task( int input )
	{
		return input * 10;
	}

END_TOOLBOX_PLUGIN_DEF


extern "C"
{
	DEFINE_TOOLBOX_PLUGIN_FACTORY( SimpleTask, SimpleImple2 )

	// Optional plugin event functions
	// void onLoad();
	// void onUnload();
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


