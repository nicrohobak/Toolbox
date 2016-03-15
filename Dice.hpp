#ifndef TOOLBOX_DICE_HPP
#define TOOLBOX_DICE_HPP

/*
 * Toolbox/Dice.hpp
 *
 * A simple dice roller library.
 */

/*****************************************************************************
 * Example Program:
	int main( int argc, char *argv[] )
	{
		int ReturnCode = 0;

		try
		{
			Toolbox::Dice<>		SixSiders;	// Without any specification, we get 6-siders
			Toolbox::Dice< 10 > TenSiders;	// Or we can specify the number of sides

			std::cout << "Rolling 3D6:" << std::endl;
			Toolbox::DiceResults SixSiderResults	= SixSiders.Roll( 3 );

			for ( auto r = SixSiderResults.begin(), r_end = SixSiderResults.end(); r != r_end; ++r )
				std::cout << "\t" << *r << std::endl;

			std::cout << "Rolling 2D10 (Percentile):" << std::endl;
			Toolbox::DiceResults PercentileResults	= TenSiders.Roll( 2 );	// Roll 2D10

			std::cout << "\t";
			bool First = true;
			for ( auto r = PercentileResults.begin(), r_end = PercentileResults.end(); r != r_end; ++r )
			{
				// Some special checking to make sure our second die prints properly
				if ( First || (!First && *r != 10) )
					std::cout << *r;
				else
					std::cout << "0";

				First = false;
			}
			std::cout << "%" << std::endl;
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

#include <random>

#include <Toolbox/Defines.h>


namespace Toolbox
{
	typedef std::list< size_t >	DiceResults;


	template <size_t _NumSides = 6>
	class Dice
	{
	public:
		TOOLBOX_POINTERS( Dice<_NumSides> )

	public:
		DiceResults Roll( size_t numDice )
		{
			DiceResults Results;

			for ( size_t die = 0; die < numDice; ++die )
				Results.push_back( _d(_e) );

			return Results;
		}

	protected:
		static std::uniform_int_distribution< size_t >	_d;
		static std::random_device						_rd;
		static std::default_random_engine 				_e;
	};

	template <size_t _NumSides>
	std::uniform_int_distribution< size_t >	Dice<_NumSides>::_d{ 1, _NumSides };	// Generate within this range

	template <size_t _NumSides>
	std::random_device						Dice<_NumSides>::_rd;

	template <size_t _NumSides>
	std::default_random_engine				Dice<_NumSides>::_e( _rd() );
}


#endif // TOOLBOX_DICE_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


