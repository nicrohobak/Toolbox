#ifndef TOOLBOX_ANY_HPP
#define TOOLBOX_ANY_HPP

/*
 * Toolbox/Any.hpp
 *
 * A simple "anything" data type.
 */

/*****************************************************************************
 * Example Program:

	#include <Toolbox/Any.hpp>

	void Print( const Toolbox::Any &value )
	{
		std::cout << "Type: " << value.TypeName() << "\tValue: " << value << std::endl;

		// For this demo, we happen to know that our pointer type is a std::string
		if ( !value.TypeName().compare("Pointer") )
			std::cout << "\t\tPointer contents: " << *(value.AsPtr< std::string >()) << std::endl;
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

			Test = 987.654;
			Print( Test );

			std::cout << "\t\tSame value, as Int: " << Test.AsInt() << std::endl;

			Test = "This is a test.";
			Print( Test );

			Test = std::string( "This is another test." );
			Print( Test );

			std::string PointerTest( "Pointer test!" );
			Test.AssignPtr( &PointerTest );
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
			Any_Char,
			Any_UChar,
			Any_Short,
			Any_UShort,
			Any_Int,
			Any_UInt,
			Any_Long,
			Any_ULong,
			Any_Float,
			Any_Double,
			Any_Str,
			Any_Ptr,

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

		char AsChar() const
		{
			return _data.Char;
		}

		unsigned char AsUChar() const
		{
			return _data.UChar;
		}

		short int AsShort() const
		{
			return _data.Short;
		}

		unsigned short int AsUShort() const
		{
			return _data.UShort;
		}

		int AsInt() const
		{
			return _data.Int;
		}

		unsigned int AsUInt() const
		{
			return _data.UInt;
		}

		long AsLong() const
		{
			return _data.Long;
		}

		unsigned long AsULong() const
		{
			return _data.ULong;
		}

		float AsFloat() const
		{
			return _data.Float;
		}

		double AsDouble() const
		{
			return _data.Double;
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

				case Any_Char:
				case Any_UChar:
				{
					std::string CharStr("");
					CharStr.push_back( _data.Char );
					return CharStr;
				}

				case Any_Short:
				{
					std::stringstream Formatter;
					Formatter << AsShort();
					return std::string( Formatter.str() );
				}

				case Any_UShort:
				{
					std::stringstream Formatter;
					Formatter << AsUShort();
					return std::string( Formatter.str() );
				}

				case Any_Int:
				{
					std::stringstream Formatter;
					Formatter << AsInt();
					return std::string( Formatter.str() );
				}

				case Any_UInt:
				{
					std::stringstream Formatter;
					Formatter << AsUInt();
					return std::string( Formatter.str() );
				}

				case Any_Long:
				{
					std::stringstream Formatter;
					Formatter << AsLong();
					return std::string( Formatter.str() );
				}

				case Any_ULong:
				{
					std::stringstream Formatter;
					Formatter << AsULong();
					return std::string( Formatter.str() );
				}

				case Any_Float:
				case Any_Double:
				{
					std::stringstream Formatter;
					Formatter << _data.Double;
					return std::string( Formatter.str() );
				}

				case Any_Ptr:
				{
					std::stringstream Formatter;
					Formatter << _data.Ptr;
					return std::string( Formatter.str() );
				}

				default:
					return _str;
			}
		}

		template <typename tPtr>
		tPtr *AsPtr() const
		{
			return reinterpret_cast< tPtr * >( _data.Ptr );
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
			_data.Long = 0;
			_str.clear();
		}

		void Assign( const bool &value )
		{
			_type = Any_Bool;
			_str.clear();
			_data.Bool = value;
		}

		void Assign( const char &value )
		{
			_type = Any_Char;
			_str.clear();
			_data.Char = value;
		}

		void Assign( const unsigned char &value )
		{
			_type = Any_UChar;
			_str.clear();
			_data.UChar = value;
		}

		void Assign( const short &value )
		{
			_type = Any_Short;
			_str.clear();
			_data.Short = value;
		}

		void Assign( const unsigned short &value )
		{
			_type = Any_UShort;
			_str.clear();
			_data.UShort = value;
		}

		void Assign( const int &value )
		{
			_type = Any_Int;
			_str.clear();
			_data.Int = value;
		}

		void Assign( const unsigned int &value )
		{
			_type = Any_UInt;
			_str.clear();
			_data.UInt = value;
		}

		void Assign( const long &value )
		{
			_type = Any_Long;
			_str.clear();
			_data.Long = value;
		}

		void Assign( const unsigned long &value )
		{
			_type = Any_ULong;
			_str.clear();
			_data.ULong = value;
		}

		void Assign( const float &value )
		{
			_type = Any_Float;
			_str.clear();
			_data.Float = value;
		}

		void Assign( const double &value )
		{
			_type = Any_Double;
			_str.clear();
			_data.Double = value;
		}

		void Assign( const char *str )
		{
			_type = Any_Str;
			_data.Long = 0;
			_str = str;
		}

		void Assign( const std::string &str )
		{
			_type = Any_Str;
			_data.Int = 0;
			_str = str;
		}

		template <typename tPtr>
		void AssignPtr( tPtr ptr )
		{
			_type = Any_Ptr;
			_str.clear();
			_data.Ptr = reinterpret_cast< void * >( ptr );
		}

	protected:
		DataType		_type;

		union
		{
			bool			Bool;
			char			Char;
			unsigned char	UChar;
			short			Short;
			unsigned short	UShort;
			int				Int;
			unsigned int	UInt;
			long			Long;
			unsigned long	ULong;
			float			Float;
			double			Double;
			void *			Ptr;
		} _data;

		std::string		_str;
	};


	const char *Any::DataTypeNames[ Any::DataType::Any_MAX ] =
	{
		"NULL",
		"Bool",
		"Char",
		"UChar",
		"Short",
		"UShort",
		"Int",
		"UInt",
		"Long",
		"ULong",
		"Float",
		"Double",
		"String",
		"Pointer"
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

		case Toolbox::Any::DataType::Any_Char:
			out << value.AsChar();
			break;

		case Toolbox::Any::DataType::Any_UChar:
			out << value.AsUChar();
			break;

		case Toolbox::Any::DataType::Any_Short:
			out << value.AsShort();
			break;

		case Toolbox::Any::DataType::Any_UShort:
			out << value.AsUShort();
			break;

		case Toolbox::Any::DataType::Any_Int:
			out << value.AsInt();
			break;

		case Toolbox::Any::DataType::Any_UInt:
			out << value.AsUInt();
			break;

		case Toolbox::Any::DataType::Any_Long:
			out << value.AsLong();
			break;

		case Toolbox::Any::DataType::Any_ULong:
			out << value.AsULong();
			break;

		case Toolbox::Any::DataType::Any_Float:
			out << value.AsFloat();
			break;

		case Toolbox::Any::DataType::Any_Double:
			out << value.AsDouble();
			break;

		case Toolbox::Any::DataType::Any_Str:
			out << value.AsStr();
			break;

		case Toolbox::Any::DataType::Any_Ptr:
			out << value.AsPtr< void * >();
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


