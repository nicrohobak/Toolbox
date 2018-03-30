/*
 * libmysqlclient-based SQL_Plugin
 *
 * Link plugin with: -fPIC -shared -lmysqlclient
 * Link main app with: -ldl
 */

#include <iostream>
#include <cstdlib>		// For database type conversions

#include <Toolbox/SQL.hpp>

#include <mysql/mysql.h>


namespace Toolbox
{
	TOOLBOX_DEFINE_PLUGIN_D( SQL_Plugin, SQL_libmysqlclient )

		SQL_libmysqlclient()
		{
			_Connection = mysql_init( NULL );

			if ( !_Connection )
				throw std::runtime_error( "SQL_libmysqlclient: MySQL initialization failed!" );
		}

		virtual ~SQL_libmysqlclient()
		{
			if ( _Connection )
				mysql_close( _Connection );
		}

		virtual void Connect( const std::string &hostname, const std::string &database, const std::string &username, const std::string &password )
		{
			_Connection = mysql_real_connect( _Connection, hostname.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, 0 );

			if ( !_Connection )
				throw std::runtime_error( std::string("SQL_libmysqlclient::Connect(): Failed to connect to the database!  (") + mysql_error(_Connection) + std::string(")") );
		}

		virtual SQLResult Query( const std::string &query )
		{
			int Error = mysql_query( _Connection, query.c_str() );

			if ( Error )
				return SQLResult( mysql_error(_Connection) );

			MYSQL_RES *Result = mysql_store_result( _Connection );

			// If there's no result but no error, then return success...otherwise pass along the error
			if ( !Result && !mysql_errno(_Connection) )
				return SQLResult( 0, 0 );
			else if ( !Result )
				return SQLResult( mysql_error(_Connection) );

			size_t NumRows = mysql_affected_rows( _Connection );
			size_t NumFields = mysql_field_count( _Connection );

			SQLResult Results( NumRows, NumFields );	// The return value container

			// If we don't have any fields at all, don't even bother with this
			if ( NumFields )
			{
				// But if we do, read the headers first
				MYSQL_FIELD *Field;
				while ( (Field = mysql_fetch_field(Result)) != NULL )
					Results.Header.emplace_back( std::string(Field->name) );

				// Then get the actual table data if we have any
				if ( NumRows )
				{
					MYSQL_ROW CurRow;
					unsigned int CurRecord = 0;
					while ( (CurRow = mysql_fetch_row(Result)) != NULL )
					{
						//unsigned long *FieldLengths;
						//FieldLengths = mysql_fetch_lengths(Result);

						Results.Record.emplace_back( std::make_shared<SQLRecord>(NumFields) );

						for ( size_t CurField = 0; CurField < NumFields; ++CurField )
							Results.Record[CurRecord]->Field.emplace_back( std::string(CurRow[CurField]) );

						++CurRecord;
					}
				}
			}

			mysql_free_result( Result );

			return Results;
		}

		virtual bool ToBool( const std::string &dbStr ) const
		{
			if ( !dbStr.compare("1")
			  || !dbStr.compare("true")
			  || !dbStr.compare("TRUE")
			  || !dbStr.compare("True") )
				return true;

			return false;
		}

		virtual long ToLong( const std::string &dbStr ) const
		{
			return strtol( dbStr.c_str(), NULL, 0 );
		}

		virtual unsigned long ToULong( const std::string &dbStr ) const
		{
			return strtoul( dbStr.c_str(), NULL, 0 );
		}

		virtual double ToDouble( const std::string &dbStr ) const
		{
			return strtod( dbStr.c_str(), NULL );
		}

	protected:
		MYSQL *		_Connection;

	TOOLBOX_END_PLUGIN_DEF


	extern "C"
	{
		TOOLBOX_DEFINE_PLUGIN_INFO( "libmysqlclient",
									"0.1",
									"SQL_Plugin" )

		TOOLBOX_DEFINE_PLUGIN_FACTORY( SQL_Plugin, SQL_libmysqlclient )

		// Optional plugin event functions
		//void onLoad()
		//void onUnload()
	}
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


