#ifndef TOOLBOX_PLUGIN_HPP
#define TOOLBOX_PLUGIN_HPP

/*
 * Toolbox/Plugin.hpp
 *
 * A simple C++ plugin implementation
 */

/*****************************************************************************
 * Plugin Requirements:
 *
 * - Main application MUST link with: -ldl (Toolbox/SharedLibrary.hpp
 *     dependency)
 * - Plugins MUST link with: -fPIC -shared
 *****************************************************************************
 * Plugin Information:
 *
 * - All plugins MUST define the following items:
 *   - The Toolbox Plugin Class
 *     - MUST be defined before the Toolbox Plugin Interface
 *     - Start definition with:
 *       DEFINE_TOOLBOX_PLUGIN( InterfaceName, PluginName )
 *     - End definition with: END_TOOLBOX_PLUGIN_DEF
 *     - Custom public class definitions can be declared/defined between the
 *       start and end
 *
 *   - The Toolbox Plugin Interface
 *     - NOTE: Everything here is enclosed within an extern "C" {} block
 *     - A Toolbox Plugin Factory
 *       - DEFINE_TOOLBOX_PLUGIN_FACTORY( InterfaceName, PluginName )
 *     - Optionally, all plugins MAY define the following event functions:
 *       - All have prototype:	   void onEvent( void );
 *       - onLoad				-- Called when the plugin is initially loaded
 *       - onUnload				-- Called when the plugin is destroyed
 *****************************************************************************
 * Interface Information:
 *
 *  - All interfaces MUST define the following items:
 *    - The Toolbox Plugin Interface Class
 *      - Start definition with:
 *        DEFINE_TOOLBOX_PLUGIN_INTERFACE( InterfaceName, VersionStr )
 *      - End definition with: END_TOOLBOX_PLUGIN_DEF
 *      - Custom public class definitions can be declared/defined between the
 *        start and end
 *****************************************************************************
 * Example code available in <Toolbox/Plugin/example>
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <iostream>

#include <Toolbox/SharedLibrary.hpp>


namespace Toolbox
{
	/*
	 * Macros to assist with plugin/interface source creation
	 */
	// MUST be closed with END_TOOLBOX_PLUGIN_DEF!
	#define DEFINE_TOOLBOX_PLUGIN_INTERFACE( tInterface, tVersion )			\
		class tInterface													\
		{																	\
		public:																\
			constexpr static const char *InterfaceName = #tInterface;		\
			constexpr static const char *APIVersion = #tVersion;			\
			typedef std::shared_ptr< tInterface >	Ptr;					\
			typedef std::weak_ptr< tInterface >		wPtr;


	// MUST be closed with END_TOOLBOX_PLUGIN_DEF!
	#define DEFINE_TOOLBOX_PLUGIN( tInterface, tImplementation )			\
		class tImplementation : public tInterface							\
		{																	\
		public:																\
			TOOLBOX_MEMORY_POINTERS( tImplementation )


	// Terminates DEFINE_TOOLBOX_PLUGIN*() macros
	#define END_TOOLBOX_PLUGIN_DEF											\
		};


	#define DEFINE_TOOLBOX_PLUGIN_FACTORY( tInterface, tImplementation )	\
		const char *_Name						= #tImplementation;			\
		const char *_APIVersion					= tInterface::APIVersion;	\
																			\
		tInterface::Ptr Create##tInterface()								\
		{																	\
			return std::make_shared< tImplementation >();					\
		}																	\


	/*
	 * Our simple plugin class
	 */
	class Plugin 
	{
	public:
		TOOLBOX_MEMORY_POINTERS_AND_LISTS( Plugin )

	public:
		typedef void (*tEventFunc)( void );

	public:
		Plugin( const std::string &fileName ):
			_Library( fileName ),
			_APIVersion( "!INVALID!" )
		{
			// Mandatory items
			try
			{
				_Name = _Library.GetValue< const char * >( "_Name" );
				_APIVersion = _Library.GetValue< const char * >( "_APIVersion" );
			}
			catch ( std::exception &ex )
			{
				throw std::runtime_error( std::string("Plugin::Plugin(): '") + fileName + std::string("' is not a valid plugin.") );
			}

			// Suppressing errors for an optional function
			tEventFunc onLoad = NULL;
			try
			{
				onLoad = _Library.GetSymbol< tEventFunc >( "onLoad" );
			}
			catch ( std::exception &ex )
			{
			}

			if ( onLoad )
				(*onLoad)();
		}

		virtual ~Plugin()
		{
			// Suppressing errors for an optional function 
			tEventFunc onUnload = NULL;
			try
			{
				onUnload = _Library.GetSymbol< tEventFunc >( "onUnload" );
			}
			catch ( std::exception &ex )
			{
			}

			if ( onUnload )
				(*onUnload)();
		}

		const std::string &Name() const
		{
			return _Name;
		}

		const std::string &APIVersion() const
		{
			return _APIVersion;
		}

		template <typename tInterface, typename ... tArgs>
		typename tInterface::Ptr Create( const std::string &type, tArgs ... arguments  )
		{
			typedef typename tInterface::Ptr (*tCreateFunc)( tArgs ... arguments );

			typename tInterface::Ptr Instance;
			tCreateFunc CreateFunc = NULL;

			try
			{
				CreateFunc = _Library.GetSymbol< tCreateFunc >( std::string("Create") + type );
				Instance = (*CreateFunc)( &arguments... );
			}
			catch ( std::exception &ex )
			{
				throw std::runtime_error( "Toolbox::Plugin::Create(): " + Name() + " doesn't provide a 'Create" + type + "' function." );
			}

			return Instance;
		}

	protected:
		SharedLibrary	_Library;

		std::string		_Name;
		std::string		_APIVersion;

	};

	
	/*
	 * Generic plugin manager
	 */
	class PluginManager
	{
	public:
		TOOLBOX_MEMORY_POINTERS( PluginManager );

	public:
		PluginManager()
		{
		}

		virtual ~PluginManager()
		{
		}

		const Plugin::List Plugins() const
		{
			return _Plugins;
		}

		void Load( const std::string &fileName )
		{
			_Plugins.push_back( std::make_shared< Plugin >(fileName) );
		}

		template <typename tInterface, typename ... tArgs>
		typename tInterface::Ptr Create( const std::string &type, tArgs ... arguments )
		{
			typename tInterface::Ptr Instance;

			for ( auto p = _Plugins.begin(), p_end = _Plugins.end(); p != p_end; ++p )
			{
				if ( !type.compare((*p)->Name()) )
				{
					Instance = (*p)->Create< tInterface >( tInterface::InterfaceName, &arguments ... );
					break;
				}
			}

			if ( !Instance )
				throw std::runtime_error( std::string("Toolbox::Plugin::Manager::Create(): Failed to create new ") + tInterface::InterfaceName + ": Create" + type + " not found." );

			return Instance;
		}


	protected:
		Plugin::List	_Plugins;
	};
}


#endif // TOOLBOX_PLUGIN_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


