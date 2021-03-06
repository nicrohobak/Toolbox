#ifndef SIMPLETASK_H
#define SIMPLETASK_H

/*
 * SimpleTask plugin interface example
 */

#include <Toolbox/Plugin.hpp>


TOOLBOX_DEFINE_PLUGIN_INTERFACE( SimpleTask, 0.1 )

	virtual const std::string &Operation() = 0;
	virtual int Task( int input ) = 0;

TOOLBOX_END_PLUGIN_DEF


#endif // SIMPLETASK_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


