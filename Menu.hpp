#ifndef TOOLBOX_MENU_HPP
#define TOOLBOX_MENU_HPP

/*
 * Toolbox/Menu.hpp
 *
 * A simple menu system.
 */

/*****************************************************************************
 * Example Program:

	//
	// Define some menu return types
	//
	enum tMenuReturn
	{
		Menu_FIRST,

		Menu_Invalid = Menu_FIRST,

		Menu_OptionOne,
		Menu_OptionTwo,
		Menu_OptionThree,

		Menu_Red,
		Menu_Green,
		Menu_Blue,

		Menu_Bird,
		Menu_Cat,
		Menu_Dog,
		Menu_Fish,

		Menu_MAX,
		Menu_DEFAULT = Menu_FIRST
	};

	typedef Toolbox::Menu< tMenuReturn >	tMenu;


	void DisplayMenu( tMenu::Ptr menu )
	{
		if ( !menu )
		{
			std::cout << "(Error: Invalid menu pointer.)" << std::endl;
			return;
		}

		std::cout << "-----[ " << menu->Title << " ]------" << std::endl;

		for ( auto o = menu->Options.begin(), o_end = menu->Options.end(); o != o_end; ++o )
			std::cout << "      " << (*o)->Display << std::endl;

		std::cout << std::endl;
		std::cout << menu->Prompt << std::endl;
	}


	int main( int argc, char *argv[] )
	{
		int ReturnCode = 0;

		try
		{
			// Setup our return codes for readability throughout the example
			std::map< tMenuReturn, std::string >		ReturnCodeStrings;
			ReturnCodeStrings[ Menu_Invalid		] =		"!INVALID!";
			ReturnCodeStrings[ Menu_OptionOne	] =		"Option One";
			ReturnCodeStrings[ Menu_OptionTwo	] =		"Option Two";
			ReturnCodeStrings[ Menu_OptionThree	] =		"Option Three";
			ReturnCodeStrings[ Menu_Red			] =		"Red";
			ReturnCodeStrings[ Menu_Green		] =		"Green";
			ReturnCodeStrings[ Menu_Blue		] =		"Blue";
			ReturnCodeStrings[ Menu_Bird		] =		"Bird";
			ReturnCodeStrings[ Menu_Cat			] =		"Cat";
			ReturnCodeStrings[ Menu_Dog			] =		"Dog";
			ReturnCodeStrings[ Menu_Fish		] =		"Fish";

			const std::string DefaultPrompt( "> " );

			tMenu::Ptr CurMenu = std::make_shared< tMenu >( "Test Menu", DefaultPrompt );
			CurMenu->AddOption( "1 one",   Menu_OptionOne,   "1) Menu Option One"	);
			CurMenu->AddOption( "2 two",   Menu_OptionTwo,   "2) Menu Option Two"	);
			CurMenu->AddOption( "3 three", Menu_OptionThree, "3) Menu Option Three"	);
			DisplayMenu( CurMenu );

			std::cout << "Selecting '1'...  Selection: " << ReturnCodeStrings[ CurMenu->Select("1") ] << std::endl;

			CurMenu = std::make_shared< tMenu >( CurMenu, "Sub Menu 1", DefaultPrompt );
			CurMenu->AddOption( "1 red",	Menu_Red,	"1) Red"	);
			CurMenu->AddOption( "2 green",	Menu_Green,	"2) Green"	);
			CurMenu->AddOption( "3 blue",	Menu_Blue,	"3) Blue"	);
			DisplayMenu( CurMenu );

			std::cout << "Selecting 'green'...  Selection: " << ReturnCodeStrings[ CurMenu->Select("green") ] << std::endl;
			std::cout << "Going 'back'..." << std::endl;

			CurMenu = CurMenu->Back();
			DisplayMenu( CurMenu );

			std::cout << "Selecting 'two'...  Selection: " << ReturnCodeStrings[ CurMenu->Select("two") ] << std::endl;

			CurMenu = std::make_shared< tMenu >( CurMenu, "Sub Menu 2", DefaultPrompt );
			CurMenu->AddOption( "1 bird",	Menu_Bird,	"1) Bird"	);
			CurMenu->AddOption( "2 cat",	Menu_Cat,	"2) Cat"	);
			CurMenu->AddOption( "3 dog",	Menu_Dog,	"3) Dog"	);
			CurMenu->AddOption( "4 fish",	Menu_Fish,	"3) Fish"	);
			DisplayMenu( CurMenu );

			std::cout << "Selecting 'cat'...  Selection: " << ReturnCodeStrings[ CurMenu->Select("cat") ] << std::endl;

			std::cout << "Going 'back'..." << std::endl;
			CurMenu = CurMenu->Back();
			DisplayMenu( CurMenu );

			std::cout << "Selecting 'mistake'...  Selection: " << ReturnCodeStrings[ CurMenu->Select("mistake") ] << std::endl;
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


#include <Toolbox/Arguments.hpp>


namespace Toolbox
{
	enum tDefaultOptionRetVal
	{
		OptionReturn_FIRST,

		OptionReturn_Invalid = OptionReturn_FIRST,

		OptionReturn_Confirm,
		OptionReturn_Cancel,

		OptionReturn_MAX,
		OptionReturn_DEFAULT = OptionReturn_FIRST
	};


	template <typename tOptionRetVal = tDefaultOptionRetVal, typename tOptionDisplay = std::string, typename tTitle = std::string, typename tPrompt = std::string>
	class Menu : public std::enable_shared_from_this< Menu<tOptionRetVal,tOptionDisplay,tTitle,tPrompt> >
	{
	public:
		typedef Menu< tOptionRetVal, tOptionDisplay, tTitle, tPrompt >		tSelf;
		TOOLBOX_POINTERS_AND_LISTS( tSelf )

	public:
		class Option : public std::enable_shared_from_this< Option >
		{
		public:
			TOOLBOX_POINTERS_AND_LISTS( Option )

		public:
			std::string		Keyword;
			tOptionRetVal	Return;
			tOptionDisplay	Display;

		public:
			Option( const std::string &keyword, const tOptionRetVal &retVal = tOptionRetVal(), const tOptionDisplay &display = tOptionDisplay() ):
				Keyword( keyword ),
				Return( retVal ),
				Display( display )
			{
			}
		};

	public:
		tTitle					Title;
		typename Option::List	Options;
		tPrompt					Prompt;

		Ptr						Parent;

	public:
		Menu( const tTitle &title, const tPrompt &prompt ):
			Title( title ),
			Prompt( prompt )
		{
		}

		Menu( Ptr parent, const tPrompt &title, const tPrompt &prompt ):
			Title( title ),
			Prompt( prompt ),
			Parent( parent )
		{
		}

		virtual ~Menu()
		{
		}

		Ptr Back()
		{
			return Parent;
		}

		virtual bool AddOption( const std::string &keyword, const tOptionRetVal &retVal = tOptionRetVal(), const tOptionDisplay &display = tOptionDisplay() )
		{
			if ( !keyword.empty() )
			{
				Options.emplace_back( std::make_shared< Option >(keyword, retVal, display) );
				return true;
			}

			return false;
		}

		virtual tOptionRetVal Select( const std::string &keyword )
		{
			for ( auto o = Options.begin(), o_end = Options.end(); o != o_end; ++o )
			{
				Toolbox::Arguments Args( (*o)->Keyword );

				for ( auto k = Args.begin(), k_end = Args.end(); k != k_end; ++k )
				{
					if ( !keyword.compare((*k)->Str()) )
						return (*o)->Return;
				}
			}

			return tOptionRetVal();
		}
	};
}


#endif // TOOLBOX_MENU_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


