#ifndef TOOLBOX_MEMORY_HPP
#define TOOLBOX_MEMORY_HPP

/*
 * Toolbox/Memory.hpp
 *
 * Special macro definitions for commonly used smart pointer boilerplate
 */

#include <list>
#include <memory>


namespace Toolbox
{
	namespace Memory
	{
		#define TOOLBOX_MEMORY_POINTERS( tType )			\
			typedef std::shared_ptr< tType >	Ptr;		\
			typedef std::weak_ptr< tType >		wPtr;

		#define TOOLBOX_MEMORY_POINTERS_AND_LISTS( tType )	\
			TOOLBOX_MEMORY_POINTERS( tType )				\
			typedef std::list< Ptr >			List;		\
			typedef std::list< wPtr >			wList;
	}
}


#endif // TOOLBOX_MEMORY_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


