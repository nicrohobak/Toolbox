#ifndef TOOLBOX_DEFINES_H
#define TOOLBOX_DEFINES_H

/*
 * Toolbox/Defines.h
 *
 * Special macro definitions for commonly used smart pointer boilerplate
 */

#include <list>
#include <memory>


namespace Toolbox
{
	//
	// Declare shared/weak pointers
	//
	#define TOOLBOX_POINTERS( tType )				\
		typedef std::shared_ptr< tType >	Ptr;	\
		typedef std::weak_ptr< tType >		wPtr;

	//
	// Declare shared/weak pointers for a std::list<> of our type too
	//
	#define TOOLBOX_POINTERS_AND_LISTS( tType )		\
		TOOLBOX_POINTERS( tType )					\
		typedef std::list< Ptr >			List;	\
		typedef std::list< wPtr >			wList;

	//
	// Declare a parent object
	//
	#define TOOLBOX_PARENT( tType )					\
		typedef tType						tParent;
}


#endif // TOOLBOX_DEFINES_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


