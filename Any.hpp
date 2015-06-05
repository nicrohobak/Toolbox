#ifndef TOOLBOX_ANY_HPP
#define TOOLBOX_ANY_HPP

/*
 * Toolbox/Any.hpp
 *
 * A simple "anything" data type.
 */

/*****************************************************************************
 * Example Program:
    void Print( const Toolbox::Any &value )
	{
		std::cout << "Type: " << value.TypeName() << "  Value: " << value << std::endl;
	}

	int main( int argc, char *argv[] )
	{
		int ReturnCode = 0;

		try
		{
			Toolbox::Any Test;
			Print( Test );

			Test = false;
			Print( Test );
	
			Test = (unsigned)12345;
			Print( Test );

			Test = 123.456;
			Print( Test );

			std::cout << "\tAs Int: " << Test.AsInt() << std::endl;

			Test = "This is a test.";
			Print( Test );

			Test = std::string( "This is another test." );
			Print( Test );

			Test.Unassign();
			Print( Test );
		}
		catch ( std::exception &ex )
		{
			std::cerr << "Fatal error: " << ex.what() << std::endl;
			ReturnCode = 1;
		}

		return ReturnCode;
	}

// Build with:
// g++ -std=c++14 -Wall -pedantic -o example
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <iostream>
#include <string>
#include <sstream>


namespace Toolbox
{
	class Any
	{
	public:
		enum DataType
		{
			Any_NULL,

			Any_Bool,
			Any_Int,
			Any_Uint,
			Any_Float,
			Any_Str,

			Any_MAX,

			Any_FIRST	= Any_NULL,
			Any_DEFAULT = Any_NULL
		};

		static const char *DataTypeNames[ Any_MAX ];

	public:
		Any()
		{
			Unassign();
		}

		~Any()
		{
		}

		DataType Type() const
		{
			return _type;
		}

		std::string TypeName() const
		{
			return std::string( DataTypeNames[_type] );
		}

		bool AsBool() const
		{
			return _data.Bool;
		}

		long AsInt() const
		{
			return AsLong();
		}

		long AsLong() const
		{
			switch ( _type )
			{
				case Any_NULL:
					return 0;

				case Any_Float:
					return (long)_data.Float;

				default:
					return _data.Int;
			}

		}

		unsigned long AsUint() const
		{
			return AsUlong();
		}

		unsigned long AsUlong() const
		{
			switch ( _type )
			{
				case Any_NULL:
					return 0;

				case Any_Float:
					return (unsigned long)_data.Float;

				default:
					return _data.Uint;
			}
		}

		double AsFloat() const
		{
			return AsDouble();
		}

		double AsDouble() const
		{
			switch ( _type )
			{
				case Any_NULL:
					return 0;

				case Any_Int:
					return (double)_data.Int;

				case Any_Uint:
					return (double)_data.Uint;

				default:
					return _data.Float;
			}

		}

		const std::string &AsStr() const
		{
			switch ( _type )
			{
				case Any_NULL:
					return std::string( "NULL" );

				case Any_Bool:
				{
					if ( _data.Bool )
						return std::string( "true" );
					else
						return std::string( "false" );
				}

				case Any_Int:
				{
					std::stringstream Formatter;
					Formatter << _data.Int;
					return std::string( Formatter.str() );
				}

				case Any_Uint:
				{
					std::stringstream Formatter;
					Formatter << _data.Uint;
					return std::string( Formatter.str() );
				}

				case Any_Float:
				{
					std::stringstream Formatter;
					Formatter << _data.Float;
					return std::string( Formatter.str() );
				}

				default:
					return _str;
			}
		}

		// Yay for templates, auto-type matching and the typing it saves!
		template <typename tDataType>
		Any &operator=( const tDataType &value )
		{
			this->Assign( value );
			return *this;
		}

		void Unassign()
		{
			_type = Any_NULL;
			_data.Int = 0;
			_str.clear();
		}

		void Assign( const bool &value )
		{
			_type = Any_Bool;
			_str.clear();
			_data.Bool = value;
		}

		void Assign( const int &value )
		{
			Assign( (long)value );
		}

		void Assign( const long &value )
		{
			_type = Any_Int;
			_str.clear();
			_data.Int = value;
		}

		void Assign( const unsigned int &value )
		{
			Assign( (unsigned long)value );
		}

		void Assign( const unsigned long &value )
		{
			_type = Any_Uint;
			_str.clear();
			_data.Uint = value;
		}

		void Assign( const double &value )
		{
			_type = Any_Float;
			_str.clear();
			_data.Float = value;
		}

		void Assign( const std::string &str )
		{
			_type = Any_Str;
			_data.Int = 0;
			_str = str;
		}

	protected:
		DataType		_type;

		union
		{
			bool			Bool;
			long			Int;
			unsigned long	Uint;
			double			Float;
		} _data;

		std::string		_str;
	};


	const char *Any::DataTypeNames[ Any::DataType::Any_MAX ] =
	{
		"NULL",
		"Bool",
		"Int",
		"Uint",
		"Float",
		"String"
	};
}


std::ostream &operator<<( std::ostream &out, const Toolbox::Any &value )
{
	switch ( value.Type() )
	{
		case Toolbox::Any::DataType::Any_Bool:
		{
			if ( value.AsBool() )
				out << "true";
			else
				out << "false";
			break;
		}

		case Toolbox::Any::DataType::Any_Int:
			out << value.AsInt();
			break;

		case Toolbox::Any::DataType::Any_Uint:
			out << value.AsUint();
			break;

		case Toolbox::Any::DataType::Any_Float:
			out << value.AsFloat();
			break;

		case Toolbox::Any::DataType::Any_Str:
			out << value.AsStr();
			break;

		case Toolbox::Any::DataType::Any_NULL:
		default:
			out << "NULL";
			break;
	}

	return out;
}


#endif // TOOLBOX_ANY_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


