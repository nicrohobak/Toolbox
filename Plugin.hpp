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
 * Interface Information:
 *
 *  - All interfaces MUST define the following items:
 *    - The Toolbox Plugin Interface Class
 *      - Start definition with:
 *        TOOLBOX_DEFINE_PLUGIN_INTERFACE( InterfaceName, VersionStr )
 *      - End definition with: TOOLBOX_END_PLUGIN_DEF
 *      - Custom public class definitions can be declared/defined between the
 *        start and end
 *****************************************************************************
 * Plugin Information:
 *
 * - All plugins MUST define the following items:
 *   - The Toolbox Plugin Class
 *     - MUST be defined before the Toolbox Plugin Interface extern "C" {}
 *       block;
 *     - Start definition with:
 *       TOOLBOX_DEFINE_PLUGIN( InterfaceName, PluginName )
 *     - End definition with: TOOLBOX_END_PLUGIN_DEF
 *     - Custom public class member definitions can be declared/defined
 *       between the start and end.
 *
 *   - The Toolbox Plugin Interface
 *     - NOTE: Everything here is enclosed within an extern "C" {} block
 *     - The Core Plugin Information
 *       - TOOLBOX_DEFINE_PLUGIN_INFO( name, version, provides )
 *     - Toolbox Plugin Factor(y/ies)
 *       - TOOLBOX_DEFINE_PLUGIN_FACTORY( InterfaceName, ImplementationName )
 *       - The Implementation itself does not need to be contained within the
 *         extern "C" {} block, allowing for "safe" use of C++ in the
 *         plugin's implementation.
 *     - Optionally, all plugins MAY define the following event functions:
 *       - All have prototype:	   void onEvent( void );
 *       - onLoad				-- Called when the plugin is initially loaded
 *       - onUnload				-- Called when the plugin is destroyed
 *****************************************************************************
 * Example code available in <Toolbox/Plugin/example>
 ****************************************************************************/


#include <algorithm>
#include <iostream>
#include <map>

#include <cctype>

#include <Toolbox/SharedLibrary.hpp>


namespace Toolbox
{
	/*
	 * Macros to assist with plugin/interface source creation
	 */
	// MUST be closed with TOOLBOX_END_PLUGIN_DEF!
	#define TOOLBOX_DEFINE_PLUGIN_INTERFACE( tInterface, tVersion )			\
		class tInterface : public std::enable_shared_from_this< tInterface >\
		{																	\
		public:																\
			constexpr static const char *Name = #tInterface;				\
			constexpr static const char *APIVersion = #tVersion;			\
			TOOLBOX_POINTERS( tInterface )							\
																			\
			virtual ~tInterface() { }


	// MUST be closed with TOOLBOX_END_PLUGIN_DEF!
	// Same as above, but with no destructor automatically defined, the user
	//   MUST provide one
	#define TOOLBOX_DEFINE_PLUGIN_INTERFACE_D( tInterface, tVersion )		\
		class tInterface : public std::enable_shared_from_this< tInterface >\
		{																	\
		public:																\
			constexpr static const char *Name = #tInterface;				\
			constexpr static const char *APIVersion = #tVersion;			\
			TOOLBOX_POINTERS( tInterface )



	// MUST be closed with TOOLBOX_END_PLUGIN_DEF!
	#define TOOLBOX_DEFINE_PLUGIN( tInterface, tImplementation )			\
		class tImplementation : public tInterface							\
		{																	\
		public:																\
			TOOLBOX_POINTERS( tImplementation )						\
																			\
			virtual ~tImplementation() { }


	// MUST be closed with TOOLBOX_END_PLUGIN_DEF!
	// Same as above, but with no destructor automatically defined, the user
	//   MUST provide one
	#define TOOLBOX_DEFINE_PLUGIN_D( tInterface, tImplementation )			\
		class tImplementation : public tInterface							\
		{																	\
		public:																\
			TOOLBOX_POINTERS( tImplementation )


	// Terminates TOOLBOX_DEFINE_PLUGIN*() macros
	#define TOOLBOX_END_PLUGIN_DEF											\
		};


	//
	// Main Plugin Implementation Definition -- Without this, your plugin
	// 											isn't valid
	//
	// MUST be the first thing defined in your extern "C" block.
	//
	// name     - Whatever creative name you would like to give this plugin
	// version  - A version string, typically in the format of:
	// 			  (Major.minor.revision)
	// provides - A whitespace and/or comma separated list of interfaces
	// 			  provided by this plugin
	//
	#define TOOLBOX_DEFINE_PLUGIN_INFO( name, version, provides )			\
		const char *_Name		= name;										\
		const char *_Version	= version;									\
		const char *_Provides	= provides;


	//
	// Plugin Interface Definitions
	//
	#define TOOLBOX_DEFINE_PLUGIN_FACTORY( tInterface, tImplementation )	\
		const char *_ ## ##tInterface ## _APIVersion	= tInterface::APIVersion;	\
																			\
		tInterface::Ptr Create##tInterface()								\
		{																	\
			return std::make_shared< tImplementation >();					\
		}																	\


	//
	// Plugin Manager static class integration
	// Defines a protected static PluginManager object and some static members to manipulate it
	// Assumes a singular plugin type and provides a single PluginManager
	// MUST also use TOOLBOX_DECLARE_STATIC_PLUGIN_MGR (below)
	//
	// tParent	- Containing class
	// tPlugin	- The plugin name
	//
	#define TOOLBOX_DEFINE_STATIC_PLUGIN_MGR( tParent, tPlugin )			\
		protected:															\
			static PluginManager		_PluginMgr;							\
																			\
		public:																\
			static const PluginManager &PluginMgr()							\
			{																\
				return _PluginMgr;											\
			}																\
																			\
			static void LoadPlugin( const std::string &fileName )			\
			{																\
				constexpr const char *dbg_CurFunc = "Toolbox::" #tParent "::LoadPlugin(const std::string &)";	\
				auto NewPlugin = _PluginMgr.Load( fileName );				\
				/* Make sure this plugin implements the interface we need (any valid version is fine for now */	\
				if ( !NewPlugin->Version( #tPlugin ).compare(Plugin::Invalid) )	\
				{															\
					_PluginMgr.Unload( NewPlugin->Name() );					\
					throw std::runtime_error( std::string(dbg_CurFunc) + ": Invalid " #tParent " plugin." );	\
				}															\
			}																\
																			\
			static void UnloadPlugin( const std::string &name )				\
			{																\
				_PluginMgr.Unload( name );									\
			}

	//
	// Plugin Manager static class integration
	// Declared outside of the tParent class definition.  MUST be used if TOOLBOX_DEFINE_STATIC_PLUGIN_MGR is used.
	//
	// tParent	- Containing class
	//
	#define TOOLBOX_DECLARE_STATIC_PLUGIN_MGR( tParent )					\
		PluginManager tParent::_PluginMgr;


	/*
	 * Our simple plugin class
	 */
	class Plugin 
	{
	public:
		TOOLBOX_POINTERS_AND_LISTS( Plugin )

	public:
		constexpr static char *Invalid = "!INVALID!";
		typedef std::map< std::string, std::string >	tInterfaceMap;
		typedef void (*tEventFunc)( void );

	public:
		static std::string GetExtFromFilename( const std::string &fileName )
		{
			constexpr const char *dbg_CurFunc = "Toolbox::Plugin::GetExtFromFilename(const std::string &)";

			std::string Ext("");

			if ( fileName.empty() )
				return Ext;

			auto LastDot = fileName.find_last_of( '.' );
			Ext = fileName.substr( ++LastDot );

			if ( !Ext.empty() )
				std::transform( Ext.begin(), Ext.end(), Ext.begin(), (int(*)(int))std::toupper );

			return Ext;
		}

	public:
		Plugin( const std::string &fileName ):
			_Library( fileName ),
			_Version( "!INVALID!" )
		{
			// Mandatory items
			try
			{
				_Name = _Library.GetValue< const char * >( "_Name" );
				_Version = _Library.GetValue< const char * >( "_Version" );
				std::string Provides = _Library.GetValue< const char * >( "_Provides" );

				std::string Temp("");

				// The _Provides from the library can be whitespace and/or comma separated
				for ( auto p = Provides.begin(), p_end = Provides.end(); p != p_end; ++p )
				{
					if ( *p == ',' || isspace(*p) )
					{
						if ( Temp.empty() )
							continue;
						else
						{
							_Provides[ Temp ] = Invalid;
							Temp.clear();
						}
					}
					else
						Temp.push_back( *p );
				}

				// Make sure we get the last one too
				if ( !Temp.empty() )
					_Provides[ Temp ] = Invalid;
	
				// Now, find the Interface versions for each interface we provide
				for ( auto p = _Provides.begin(), p_end = _Provides.end(); p != p_end; ++p )
				{
					std::string APIVersionString( "_" );
					APIVersionString.append( p->first );
					APIVersionString.append( "_APIVersion" );
					p->second = _Library.GetValue< const char * >( APIVersionString );
				}
			}
			catch ( std::exception &ex )
			{
				throw std::runtime_error( std::string("Plugin::Plugin(): Invalid plugin: ") + ex.what() );
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

		const std::string &Version() const
		{
			return _Version;
		}

		const tInterfaceMap &Provides() const
		{
			return _Provides;
		}

		const std::string &Version( const std::string &interface ) const
		{
			for ( auto p = _Provides.begin(), p_end = _Provides.end(); p != p_end; ++p )
			{
				if ( !p->first.compare(interface) )
					return p->second;
			}

			return std::string( Invalid );
		}

		template <typename tInterface, typename ... tArgs>
		typename tInterface::Ptr Create( tArgs ... arguments  )
		{
			typedef typename tInterface::Ptr (*tCreateFunc)( tArgs ... arguments );

			typename tInterface::Ptr Instance;
			tCreateFunc CreateFunc = NULL;

			try
			{
				CreateFunc = _Library.GetSymbol< tCreateFunc >( std::string("Create") + tInterface::Name );
				Instance = (*CreateFunc)( &arguments... );
			}
			catch ( std::exception &ex )
			{
				throw std::runtime_error( "Toolbox::Plugin::Create(): '" + Name() + "' doesn't provide a 'Create" + tInterface::Name + "' function." );
			}

			return Instance;
		}

	protected:
		SharedLibrary	_Library;

		std::string		_Name;		// Plugin Name
		std::string		_Version;	// Plugin Version
		tInterfaceMap	_Provides;	// Interface Name, Interface Version
	};

	
	/*
	 * Generic plugin manager
	 */
	class PluginManager
	{
	public:
		TOOLBOX_POINTERS( PluginManager )

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

		Plugin::Ptr Load( const std::string &fileName )
		{
			Plugin::Ptr NewPlugin = std::make_shared< Plugin >( fileName );
			_Plugins.push_back( NewPlugin );
			return NewPlugin;
		}

		void Unload( const std::string &name )
		{
			static const std::string dbg_CurFunc( "Toolbox::PluginManager::Unload(const std::string &)" );

			for ( auto p = _Plugins.begin(), p_end = _Plugins.end(); p != p_end; ++p )
			{
				if ( !name.compare((*p)->Name()) )
				{
					_Plugins.erase( p );
					return;
				}
			}
		}

		Plugin::Ptr Find( const std::string &plugin ) const
		{
			for ( auto p = _Plugins.begin(), p_end = _Plugins.end(); p != p_end; ++p )
			{
				if ( !(*p)->Name().compare(plugin) )
					return *p;
			}

			return Plugin::Ptr();
		}

		template <typename tInterface, typename ... tArgs>
		typename tInterface::Ptr Create( const std::string &plugin, tArgs ... arguments ) const
		{
			typename tInterface::Ptr Instance;
			static const std::string dbg_CurFunc( "Toolbox::PluginManager::Create(const std::string &, ...)" );

			// Find our desired plugin
			for ( auto p = _Plugins.begin(), p_end = _Plugins.end(); p != p_end; ++p )
			{
				// If it's not the desired plugin, move on
				if ( !plugin.compare((*p)->Name()) )
				{
					// Check all of the provided interfaces from this plugin
					for ( auto pl = (*p)->Provides().begin(), pl_end = (*p)->Provides().end(); pl != pl_end; ++pl )
					{
						// If we've got the interface, create one
						if ( !pl->first.compare(tInterface::Name) )
						{
							Instance = (*p)->Create< tInterface >( &arguments ... );
							break;
						}
					}

					break;
				}
			}

			if ( !Instance )
				throw std::runtime_error( dbg_CurFunc + ": Failed to create new " + tInterface::Name + " ('" + plugin + "' does not have this interface)." );

			return Instance;
		}


	protected:
		Plugin::List	_Plugins;
	};
}


#endif // TOOLBOX_PLUGIN_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


