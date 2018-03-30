#ifndef TOOLBOX_SQL_HPP
#define TOOLBOX_SQL_HPP

/*
 * Generic SQL
 *
 * A generic, plugin-ready SQL interface.
 */


/* NOTE: Plugin Required! ****************************************************

	 * Default plugin implementation(s) available at:
		<Toolbox/Plugin/SQL>

	* For more information, please refer to <Toolbox/Plugin/README>

 ****************************************************************************/


/*****************************************************************************

	// This example uses the libmysqlclient Toolbox::SQL plugin for its SQL implementation
	Toolbox::SQL::LoadPlugin( "/path/to/Toolbox/Plugin/SQL/libmysqlclient.so" );
	Toolbox::SQL::SetPlugin( "libmysqlclient" );

	// Generic Toolbox::SQL code starts here
	const std::string Host( "hostname:port" );
	const std::string Database( "database" );
	const std::string Username( "user" );
	const std::string Password( "pass" );
	const std::string Table( "table" );

	// Initialize and connect to the server
	Toolbox::SQL SqlServer( Host, Database, Username, Password );

	// Run a SQL query
	auto Result = SqlServer.Query( "SELECT * FROM `" + Database + "`.`" + Table + "`;" );

	// Iterate over all query results
	std::cout << "Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;

	size_t CurRecord = 0;
	for ( auto r = Result.Record.begin(), r_end = Result.Record.end(); r != r_end; ++r )
	{   
		std::cout << "CurRecord: " << CurRecord << "  NumFields: " << (*r)->Field.size() << std::endl;
		
		size_t CurField = 0;
		for ( auto f = (*r)->Field.begin(), f_end = (*r)->Field.end(); f != f_end; ++f )
		{   
			std::cout << "    CurField: " << CurField << " -- " << Result.Header[CurField] << " : " << *f << std::endl;
			++CurField;
		}
		
		++CurRecord;
	}
	
	// Or read a specific record
	const size_t TestRecord = 0;
	const size_t TestField = 1;

	// Long-hand notation access
	std::cout << Result.Header[TestField] << " : " << Result.Record[TestRecord]->Field[TestField] << std::endl;

	// Short-hand notation access (typically preferred and functionally identical to the longer version)
	std::cout << Result.Header[TestField] << " : " << Result[TestRecord][TestField] << std::endl;


	const std::string TestDB( "__TEST_DB__" );
	std::cout << "Running a \"create database\" query..." << std::endl;
	Result = SqlServer.Query( "create database `" + TestDB + "`;" );

	std::cout << "  Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;
	if ( !Result.Successful() )
		std::cout << "    -- Error: " << Result.Error() << std::endl;


	std::cout << "Showing all databases..." << std::endl;
	Result = SqlServer.Query( "show databases;" );

	std::cout << "  Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;
	if ( !Result.Successful() )
		std::cout << "    -- Error: " << Result.Error() << std::endl;
	for ( auto r = Result.Record.begin(), r_end = Result.Record.end(); r != r_end; ++r )
	{   
		size_t CurField = 0;
		for ( auto f = (*r)->Field.begin(), f_end = (*r)->Field.end(); f != f_end; ++f )
		{   
			std::cout << "      -- " << Result.Header[CurField] << " : " << *f << std::endl;
			++CurField;
		}
	}
	
	
	std::cout << "Running a duplicate \"create database\" query..." << std::endl;
	Result = SqlServer.Query( "create database `" + TestDB + "`;" );

	std::cout << "  Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;
	if ( !Result.Successful() )
		std::cout << "    -- Error: " << Result.Error() << std::endl;


	std::cout << "Showing all databases again..." << std::endl;
	Result = SqlServer.Query( "show databases;" );

	std::cout << "  Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;
	if ( !Result.Successful() )
		std::cout << "    -- Error: " << Result.Error() << std::endl;
	for ( auto r = Result.Record.begin(), r_end = Result.Record.end(); r != r_end; ++r )
	{   
		size_t CurField = 0;
		for ( auto f = (*r)->Field.begin(), f_end = (*r)->Field.end(); f != f_end; ++f )
		{   
			std::cout << "      -- " << Result.Header[CurField] << " : " << *f << std::endl;
			++CurField;
		}
	}
	
	
	std::cout << "Deleting the test database..." << std::endl;
	Result = SqlServer.Query( "drop database `" + TestDB + "`;" );

	std::cout << "  Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;
	if ( !Result.Successful() )
		std::cout << "    -- Error: " << Result.Error() << std::endl;


	std::cout << "Showing all databases, one last time..." << std::endl;
	Result = SqlServer.Query( "show databases;" );

	std::cout << "  Result - Success: " << Result.Successful() << "  NumRows: " << Result.NumRows() << "  NumFields: " << Result.NumFields() << std::endl;
	if ( !Result.Successful() )
		std::cout << "    -- Error: " << Result.Error() << std::endl;
	for ( auto r = Result.Record.begin(), r_end = Result.Record.end(); r != r_end; ++r )
	{   
		size_t CurField = 0;
		for ( auto f = (*r)->Field.begin(), f_end = (*r)->Field.end(); f != f_end; ++f )
		{   
			std::cout << "      -- " << Result.Header[CurField] << " : " << *f << std::endl;
			++CurField;
		}
	}


	// Conversion tests:
	std::cout << "Bool:   " << SqlServer.ToBool("true") << " - " << SqlServer.ToBool("false") << std::endl;
	std::cout << "Long:   " << SqlServer.ToLong("8675309") << " - " << SqlServer.ToLong("-1") << std::endl;
	std::cout << "ULong:  " << SqlServer.ToULong("8675309") << " - " << SqlServer.ToULong("-1") << std::endl;
	std::cout << "Double: " << std::setprecision(20) << SqlServer.ToDouble("867.5309") << " - " << SqlServer.ToDouble("1234.56789") << " - " << SqlServer.ToDouble("3.141592653589793239") << std::endl;

 ****************************************************************************/


#include <deque>
#include <vector>

#include <Toolbox/Any.hpp>
#include <Toolbox/Defines.h>
#include <Toolbox/Plugin.hpp>


namespace Toolbox
{
	//
	// SQL class helpers
	//
	// A single record of an SQL query result
	class SQLRecord
	{
	public:
		TOOLBOX_POINTERS_AND_LISTS( SQLRecord )
		typedef std::deque< Ptr >				Deque;

	public:
		std::vector< std::string >				Field;

	public:
		SQLRecord( size_t numFields = 0 )
		{
			if ( numFields )
				Field.reserve( numFields );
		}

		std::string &operator[]( size_t pos )
		{
			return Field[pos];
		}

		const std::string &operator[]( size_t pos ) const
		{
			return Field[pos];
		}

		void Clear()
		{
			Field.clear();
		}
	};


	// A result of an SQL query
	class SQLResult
	{
	public:
		TOOLBOX_POINTERS_AND_LISTS( SQLResult )

	public:
		std::vector< std::string >				Header;
		SQLRecord::Deque						Record;

	public:
		SQLResult( const std::string &error ):
			_Successful( false ),
			_Error( error ),
			_NumRows( 0 ),
			_NumFields( 0 )
		{
		}

		SQLResult( size_t numRows = 0, size_t numFields = 0 ):
			_Successful( true ),
			_Error( "" ),
			_NumRows( numRows ),
			_NumFields( numFields )
		{
		}

		SQLRecord &operator[]( size_t pos )
		{
			static SQLRecord EmptyRecord;
			EmptyRecord.Clear();
			auto CurRecord = Record[pos];

			if ( CurRecord )
				return *CurRecord;

			return EmptyRecord;
		}

		const SQLRecord &operator[]( size_t pos ) const
		{
			auto CurRecord = Record[pos];

			if ( CurRecord )
				return *CurRecord;

			return SQLRecord();
		}

		bool Successful() const
		{
			return _Successful;
		}

		size_t NumRows() const
		{
			return _NumRows;
		}

		size_t NumFields() const
		{
			return _NumFields;
		}

		const std::string &Error() const
		{
			return _Error;
		}

		void Clear()
		{
			Header.clear();
			Record.clear();
		}

	protected:
		bool									_Successful;
		std::string								_Error;
		size_t									_NumRows;
		size_t									_NumFields;
	};


	//
	// Forward declaration so our SQL_Plugin can accept an SQL as a parameter
	//
	class SQL;


	//
	// SQL_Plugin interface
	//
	TOOLBOX_DEFINE_PLUGIN_INTERFACE( SQL_Plugin, "0.1" )

		virtual void Connect( const std::string &hostname = "", const std::string &database = "", const std::string &username = "", const std::string &password = "" ) = 0;
		virtual SQLResult Query( const std::string &query = "" ) = 0;

		// Result conversions
		virtual bool ToBool( const std::string &dbstr ) const = 0;
		virtual long ToLong( const std::string &dbstr ) const = 0;
		virtual unsigned long ToULong( const std::string &dbstr ) const = 0;
		virtual double ToDouble( const std::string &dbstr ) const = 0;
		
	protected:
		//
		// Let plugins set select private variables when loading via our "friend" status
		//

	TOOLBOX_END_PLUGIN_DEF


	//
	// Our generic, plugin-based SQL class
	//
	class SQL
	{
	public:
		TOOLBOX_POINTERS( SQL )
		TOOLBOX_DEFINE_STATIC_PLUGIN_MGR( SQL, SQL_Plugin )

		static SQL_Plugin::Ptr				Current;

		static void SetPlugin( const std::string &pluginName )
		{
			if ( pluginName.empty() )
			{
				Current.reset();
				return;
			}

			auto PluginData = _PluginMgr.Find( pluginName );
			Current = _PluginMgr.Create< SQL_Plugin >( pluginName );

			if ( !Current )
				throw std::runtime_error( "Toolbox::SQL::SetPlugin(): Failed to create an instance of '" + pluginName + "'" );
		}

	public:
		std::string				Host;
		std::string				Database;
		std::string				Username;
		std::string				Password;

	public:
		SQL( const std::string &host = "",
			 const std::string &database = "",
			 const std::string &username = "",
			 const std::string &password = "" ):
			Host(host),
			Database(database),
			Username(username),
			Password(password)
		{
			if ( !Host.empty() && Current )
				Connect();
		}

		void Connect( const std::string &host = "",
					  const std::string &database = "",
					  const std::string &username = "",
					  const std::string &password = "" )
		{
			// If all input parameters are empty, we just skip this part...otherwise we set our local variables according to input
			if ( !(host.empty() && database.empty() && username.empty() && password.empty()) )
			{
				if ( !host.empty() || !Host.empty() )
					Host = host;

				if ( !database.empty() || !Database.empty() )
					Database = database;

				if ( !username.empty() || !Username.empty() )
					Username = username;

				if ( !password.empty() || !Password.empty() )
					Password = password;
			}

			if ( Current )
				Current->Connect( Host, Database, Username, Password );
		}

		SQLResult Query( const std::string &query )
		{
			if ( !Current )
				Connect();

			if ( Current )
				return Current->Query( query );

			return SQLResult();
		}

		bool ToBool( const std::string &dbStr )
		{
			if ( Current )
				return Current->ToBool( dbStr );

			return false;
		}

		long ToLong( const std::string &dbStr )
		{
			if ( Current )
				return Current->ToLong( dbStr );

			return long(0);
		}

		unsigned long ToULong( const std::string &dbStr )
		{
			if ( Current )
				return Current->ToULong( dbStr );

			return (unsigned long)(0);
		}

		double ToDouble( const std::string &dbStr )
		{
			if ( Current )
				return Current->ToDouble( dbStr );

			return double(0.0);
		}

	protected:
		friend class Toolbox::SQL_Plugin;
	};

	TOOLBOX_DECLARE_STATIC_PLUGIN_MGR( SQL )
	SQL_Plugin::Ptr SQL::Current;


	//
	// SQL_Plugin helper functions
	//
}


#endif // TOOLBOX_SQL_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


