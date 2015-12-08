/*
 * SimpleTask plugin implementation called SimpleImple1
 */

#include <Toolbox/Plugin.hpp>

#include "../SimpleTask_interface.h"


TOOLBOX_DEFINE_PLUGIN( SimpleTask, SimpleImple1 )

	virtual const std::string &Operation()
	{
		static std::string Op( "* 5" );
		return Op;
	}

	virtual int Task( int input )
	{
		return input * 5;
	}

TOOLBOX_END_PLUGIN_DEF


extern "C"
{
	TOOLBOX_DEFINE_PLUGIN_INFO( "Simple1",
								"1.0",
								"SimpleTask" )

	TOOLBOX_DEFINE_PLUGIN_FACTORY( SimpleTask, SimpleImple1 )

	// Optional plugin event functions
	// void onLoad();
	// void onUnload();
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


