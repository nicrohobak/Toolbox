/*
 * main.cpp
 *
 * An couple of working examples for Toolbox::Genetics
 */


#include <bitset>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <Toolbox/Genetics.hpp>



//////////////////////////////////////////////////////////////////////////////
// Setting this provides an example of how this library can be used to
// implement a "traditional" genetic algorithm.  Otherwise, a non-standard
// (and somewhat video game oriented) example will be used.
//
// Both types are interesting, have potential, and the concepts are not at all
// mutually exclusive.
//////////////////////////////////////////////////////////////////////////////

#define TRADITIONAL_GENETICS	1

//////////////////////////////////////////////////////////////////////////////


#if TRADITIONAL_GENETICS
	//////////////////////////////////////////////////////////////////////////
	// Define our custom data type for our traditional genetic algorithm
	// example.  It implements something similar to this example:
	//   http://www.ai-junkie.com/ga/intro/gat1.html
	//
	// - The genetic variation between this example and the tutorial are
	//   significantly different.  Mutation happens on the allele as a unit
	//   instead of on each bit individually.  Crossover is replaced in
	//   principle with the "recessive" genetic information.
	//////////////////////////////////////////////////////////////////////////
	class MathBotAllele
	{
	public:
		// Four bits are required to represent the range of characters used:
		// 0:         0000
		// 1:         0001
		// 2:         0010
		// 3:         0011
		// 4:         0100
		// 5:         0101
		// 6:         0110
		// 7:         0111
		// 8:         1000
		// 9:         1001
		// +:         1010
		// -:         1011
		// *:         1100
		// /:         1101
		enum tSymbol
		{
			Sym_0,
			Sym_1,
			Sym_2,
			Sym_3,
			Sym_4,
			Sym_5,
			Sym_6,
			Sym_7,
			Sym_8,
			Sym_9,
			Sym_Add,
			Sym_Sub,
			Sym_Mult,
			Sym_Div,

			Sym_MAX,
			Sym_FIRST = Sym_0,
		};

	public:
		std::bitset< 4 >	Bits;

	public:
		MathBotAllele()
		{
			static std::uniform_int_distribution< int >		d{ 0, 1 };   // Coin toss
			static std::random_device						rd;
			static std::default_random_engine				e( rd() );

			// Randomize initial values
			for ( size_t b = Bits.size(); b > 0; --b )
				Bits.set( b - 1, d(e) );
		}

		MathBotAllele( bool bit1, bool bit2, bool bit3, bool bit4 )
		{
			Bits.set( 0, bit1 );
			Bits.set( 1, bit2 );
			Bits.set( 2, bit3 );
			Bits.set( 3, bit4 );
		}

		// Calculates our current value (decodes the raw bits into decimal)
		// Basically borrowed from the tutorial code
		int Decimal() const
		{
			int Value		= 0;
			int ValueToAdd	= 1;

			for ( int b = Bits.size(); b > 0; --b )
			{
				// Add up the bits, only counting them if they're significant
				if ( Bits.test(b - 1) )
					Value += ValueToAdd;

				// Moving to the next bit inceases the "power of 2"
				ValueToAdd *= 2;
			}

			return Value;
		}

		char Symbol() const
		{
			char Value = ' ';

			switch ( Decimal() )
			{
				case Sym_0:
					Value = '0';
					break;

				case Sym_1:
					Value = '1';
					break;

				case Sym_2:
					Value = '2';
					break;

				case Sym_3:
					Value = '3';
					break;

				case Sym_4:
					Value = '4';
					break;

				case Sym_5:
					Value = '5';
					break;

				case Sym_6:
					Value = '6';
					break;

				case Sym_7:
					Value = '7';
					break;

				case Sym_8:
					Value = '8';
					break;

				case Sym_9:
					Value = '9';
					break;

				case Sym_Add:
					Value = '+';
					break;

				case Sym_Sub:
					Value = '-';
					break;

				case Sym_Mult:
					Value = '*';
					break;

				case Sym_Div:
					Value = '/';
					break;

				default:
					Value = ' ';
					break;
			}

			return Value;
		}

		void Mutate( double factor )
		{
			static std::uniform_int_distribution< int >		d{ 0, 1 };   // Coin toss
			static std::random_device						rd;
			static std::default_random_engine				e( rd() );

			const float NormalMutationRate					= 0.5;				// 50%

			for ( size_t b = Bits.size(); b > 0; --b )
			{
				// Only mutate some of the time
				if ( Toolbox::Genetics::Default::tMutationFactor(1.0) - d(e) < (NormalMutationRate * factor) )
					Bits.flip( b - 1 );
			}
		}
	};


	// The organism we'll be working with
	class MathBot : public Toolbox::Genetics::Organism
	{
	public:
		TOOLBOX_MEMORY_POINTERS( MathBot )

		typedef Toolbox::Genetics::Organism					tParent;
		typedef Toolbox::Genetics::Chromosome::tDominance	tDominance;

		constexpr static size_t NumMathBotAlleles = 9;

	public:
		MathBot( size_t numSimulatedParents = 2 ):
			tParent( numSimulatedParents )
		{
			static std::uniform_int_distribution< tDominance >	d{ tDominance(), tDominance(100) };	// Random dominance ratings between 0 and 100
			static std::random_device							rd;
			static std::default_random_engine					e( rd() );
			std::stringstream CurAlleleLabel("");

			for ( size_t p = 0; p < numSimulatedParents; ++p )
			{
				auto NewChromosome = _genome->AddChromosome( "MathBot", d(e) );

				for ( size_t a = 0; a < NumMathBotAlleles; ++a )
				{
					CurAlleleLabel.str("");
					CurAlleleLabel << a;
					NewChromosome->Alleles[ CurAlleleLabel.str() ] = std::make_shared< Toolbox::Genetics::Allele<MathBotAllele> >();
				}
			}
		}

		// This constructor is required so our shepherd can breed these organisms
		MathBot( Toolbox::Genetics::Genome::Ptr genome, const tMutationRate &rate = Toolbox::Genetics::Default::MutationRate ):
			tParent( genome, rate )
		{
		}

		virtual ~MathBot()
		{
		}

		std::string Formula() const
		{
			return _Formula.str();
		}

		// NOTE: This does NOT follow order of operations during calculation (nor does the tutorial example)
		int Value()
		{
			std::stringstream CurAlleleLabel("");
			std::list< unsigned int > SymbolValues;
			int TotalValue = 0;

			_Formula.str("");

			bool IsNumber = true;

			// Read through the alleles and store them in temp list
			for ( size_t a = 0; a < NumMathBotAlleles; ++a )
			{
				CurAlleleLabel.str("");
				CurAlleleLabel << a;
				auto CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", CurAlleleLabel.str() );
				unsigned int CurSymbolValue = CurAllele.Decimal();

				// Skip any invalid symbols completely
				if ( CurSymbolValue > 13 )
					continue;

				// Only add those that match the sequence: number -> symbol -> number -> symbol -> etc.
				if ( (!IsNumber && CurSymbolValue > 9)
				   || (IsNumber && CurSymbolValue < 9) )
				{
					SymbolValues.push_back( CurSymbolValue );
					IsNumber = !IsNumber;
				}
			}

			int CurSymbol = 999;	// Set to something "invalid" first

			// Now, go through the list and total the result
			for ( auto s = SymbolValues.begin(), s_end = SymbolValues.end(); s != s_end; ++s )
			{
				switch ( *s )
				{
					case 1: case 2: case 3:
					case 4: case 5: case 6:
					case 7: case 8: case 9:
					case 0:
					{
						if ( CurSymbol < 9 || CurSymbol > 13 )
						{
							TotalValue = *s;
							_Formula << TotalValue << " ";
						}
						else
						{
							switch ( CurSymbol )
							{
								case 10:	// Add
									_Formula << "+ " << *s << " ";
									TotalValue += *s;
									break;

								case 11:	// Sub
									_Formula << "- " << *s << " ";
									TotalValue -= *s;
									break;

								case 12:	// Mult
									_Formula << "* " << *s << " ";
									TotalValue *= *s;
									break;

								case 13:	// Div
									_Formula << "/ " << *s << " ";
									if ( *s != 0 )
										TotalValue /= *s;
									break;
							}
						}

						break;
					}

					case 10:	// Add
					case 11:	// Sub
					case 12:	// Mult
					case 13:	// Div
						CurSymbol = *s;
						break;

					default:
						break;
				}
			}

			return TotalValue;
		}

		// Print this MathBot's genome, etc. to stdout
		void Print( const std::string &label = std::string("MathBot") )
		{
			std::stringstream CurAlleleLabel("");
			auto ChromosomeList = _genome->GetChromosome( "MathBot" );

			std::cout << "Genome: " << label << std::endl;

			for ( auto c = ChromosomeList.begin(), end = ChromosomeList.end(); c != end; ++c )
			{
				if ( c != ChromosomeList.begin() )
					std::cout << "    ----------" << std::endl;

				for ( size_t a = 0; a < NumMathBotAlleles; ++a )
				{
					CurAlleleLabel.str("");
					CurAlleleLabel << a;
					auto CurAllele = (*c)->GetAllele< MathBotAllele >( CurAlleleLabel.str() );

					std::cout << "    Allele " << CurAlleleLabel.str() << ": " << CurAllele.Symbol() << std::endl;
				}
			}

			std::cout << "Phenotype: " << label << std::endl;
			auto CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "0" );
			std::cout << "  Allele 0: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "1" );
			std::cout << "  Allele 1: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "2" );
			std::cout << "  Allele 2: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "3" );
			std::cout << "  Allele 3: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "4" );
			std::cout << "  Allele 4: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "5" );
			std::cout << "  Allele 5: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "6" );
			std::cout << "  Allele 6: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "7" );
			std::cout << "  Allele 7: " << CurAllele.Symbol() << std::endl;
			CurAllele = this->GetPhenotype< MathBotAllele >( "MathBot", "8" );
			std::cout << "  Allele 8: " << CurAllele.Symbol() << std::endl;

			std::cout << "    Formula:     " << Formula() << std::endl;
			std::cout << "    Total value: " << Value() << std::endl;
		}

	protected:
		std::stringstream		_Formula;
	};


	// Our shepherd trains and breeds our flock of organisms for our "traditional"
	// example.
	class Shepherd : public Toolbox::Genetics::Shepherd< MathBot >
	{
	public:
		int				Target;
		size_t			FlockSize;

	public:
		Shepherd():
			Target( 42 ),
			FlockSize( 100 )
		{
			_initFlock();
		}

		Shepherd( int target, size_t flockSize = 100 ):
			Target( target ),
			FlockSize( flockSize )
		{
			_initFlock();
		}

		// How do we rate this organism?
		virtual double Rate( const Toolbox::Genetics::Organism::Ptr organism ) const
		{
			double Rating = 0.0;

			if ( !organism )
				throw std::runtime_error( "Shepherd::Rate(): No organism provided." );

			MathBot::Ptr Organism = std::dynamic_pointer_cast< MathBot >( organism );
			int Value = Organism->Value();

			if ( Value == Target )
				Rating = 99999.99;
			else
				Rating = 1 / std::abs((double)Target - (double)Value);

			return Rating;
		}

		// A simple function to contain our 'main' logic (so the example code is closer together for easier understanding)
		void FindTargetNumber( size_t maxGenerations = 20000 )
		{
			bool Found = false;

			size_t Generation = 0;
			for ( ; !Found && Generation < maxGenerations; ++Generation )
			{
				// Search through the flock for a member with the answer
				size_t FlockCount = 0;
				for ( auto f = Flock.begin(), f_end = Flock.end(); f != f_end; ++f, ++FlockCount )
				{
					if ( (*f)->Value() == Target )
					{
						std::cout << "Solution found!  (" << Generation << " generations)" << std::endl;
						(*f)->Print();
						Found = true;
						break;
					}
				}

				if ( Found )
					break;

				// Produce a new generation based on the top X percent of the current flock
				BreedFlock();
			}

			if ( !Found )
				std::cout << "No solution found after " << maxGenerations << " generations." << std::endl;
		}

		void Print()
		{
			for ( auto f = Flock.begin(), f_end = Flock.end(); f != f_end; ++f )
			{
				auto Member = std::dynamic_pointer_cast< MathBot >( *f );
				Member->Print();
			}
		}

	protected:
		void _initFlock()
		{
			for ( size_t f = 0; f < FlockSize; ++f )
				this->Flock.push_back( std::make_shared< MathBot >() );
		}
	};
	//////////////////////////////////////////////////////////////////////////
#endif // TRADITIONAL_GENETICS



//////////////////////////////////////////////////////////////////////////////
// Mutation functions MUST be defined for any data types used in our genome
//
// - The "rate" parameter is typically a percentage chance for mutation and/or
//   extremity of mutation
//////////////////////////////////////////////////////////////////////////////
namespace Toolbox
{
	namespace Genetics
	{
#if TRADITIONAL_GENETICS
		//
		// Mutation function for our "traditional" genetic algorithm example
		//
		template <>
		void Allele< MathBotAllele >::Mutate( const tMutationFactor &factor )
		{
			//std::cout << "Mutating MathBotAllele (factor: " << factor << ")" << std::endl;

			//std::cout << " -- Before: " << _data.Symbol() << std::endl;
			_data.Mutate( factor );										// In this case, we'll simply pass the buck...
			//std::cout << " -- After: " << _data.Symbol() << std::endl;
		}

#else	// TRADITIONAL_GENETICS

		//
		// Mutation function for our "non-traditional" genetic algorithm for alleles of type bool
		//
		template <>
		void Allele< bool >::Mutate( const tMutationFactor &factor )
		{
			static std::uniform_int_distribution< int >		d{ 0, 1 };	// Coin toss
			static std::random_device						rd;
			static std::default_random_engine				e( rd() );

			std::cout << "Mutating bool (factor: " << factor << ")" << std::endl;

			std::cout << " -- Before: " << _data << std::endl;
			_data = (bool)(d(e) * factor);
			std::cout << " -- After: " << _data << std::endl;
		}


		//
		// Mutation function for our "non-traditional" genetic algorithm for alleles of type char
		//
		template <>
		void Allele< char >::Mutate( const tMutationFactor &factor )
		{
			static std::uniform_real_distribution< tMutationFactor >	d{ tMutationFactor(0.0), tMutationFactor(1.0) };	// Generate within this range
			static std::random_device									rd;
			static std::default_random_engine							e( rd() );

			static std::uniform_int_distribution< char >				dDigit{ '0', '9' };	// Generate a random digit
			static std::uniform_int_distribution< char >				dLower{ 'a', 'z' };	// Generate a random lowercase character
			static std::uniform_int_distribution< char >				dUpper{ 'A', 'Z' };	// Generate a random uppercase character

			const float NormalMutationRate								= 0.5;				// 50%

			// Factor doesn't really matter here...at least in this current implementation
			std::cout << "Mutating char (factor: " << factor << ")" << std::endl;

			// Only alter alphanumeric glyphs
			if ( !isalnum(_data) )
				return;

			// Only mutate some of the time
			if ( Default::tMutationFactor(1.0) - d(e) > (NormalMutationRate * factor) )
				return;

			std::cout << " -- Before: " << _data << std::endl;
			// Make sure we keep the same kind of glyph, only modify which one it is
			if ( isdigit(_data) )
				_data = (char)dDigit(e);
			else if ( isupper(_data) )
				_data = (char)dUpper(e);
			else
				_data = (char)dLower(e);
			std::cout << " -- After: " << _data << std::endl;
		}


		//
		// Mutation function for our "non-traditional" genetic algorithm for alleles of type int
		//
		template <>
		void Allele< int >::Mutate( const tMutationFactor &factor )
		{
			static std::random_device									rd;
			static std::default_random_engine							e( rd() );

			const int MutationRange										= 50;											// Mutate to +/- this value
			static std::uniform_int_distribution< int >					dInt{ -MutationRange, MutationRange };

			std::cout << "Mutating int (factor: " << factor << ")" << std::endl;

			std::cout << " -- Before: " << _data << std::endl;
			_data += (int)(dInt(e) * factor);
			std::cout << " -- After: " << _data << std::endl;
		}


		//
		// Mutation function for our "non-traditional" genetic algorithm for alleles of type float
		//
		template <>
		void Allele< float >::Mutate( const tMutationFactor &factor )
		{
			static std::random_device									rd;
			static std::default_random_engine							e( rd() );

			const float MutationRange									= 25.0f;
			static std::uniform_real_distribution< float >				dFloat{ -MutationRange, MutationRange };   // Mutation to +/- this value

			std::cout << "Mutating float (factor: " << factor << ")" << std::endl;

			std::cout << " -- Before: " << _data << std::endl;
			_data *= dFloat(e) * factor;
			std::cout << " -- After: " << _data << std::endl;
		}


		//
		// Mutation function for our "non-traditional" genetic algorithm for alleles of type string
		//
		template <>
		void Allele< std::string >::Mutate( const tMutationFactor &factor )
		{
			static std::uniform_real_distribution< tMutationFactor >	d{ tMutationFactor(0.0), tMutationFactor(1.0) };	// Generate within this range
			static std::random_device									rd;
			static std::default_random_engine							e( rd() );

			static std::uniform_int_distribution< char >				dDigit{ '0', '9' };	// Generate a random digit
			static std::uniform_int_distribution< char >				dLower{ 'a', 'z' };	// Generate a random lowercase character
			static std::uniform_int_distribution< char >				dUpper{ 'A', 'Z' };	// Generate a random uppercase character

			const float NormalMutationRate								= 0.5;				// 50%

			std::cout << "Mutating string (factor: " << factor << ")" << std::endl;

			std::cout << " -- Before: " << _data << std::endl;
			for ( auto c = _data.begin(), end = _data.end(); c != end; ++c )
			{
				// Only alter alphanumeric glyphs
				if ( !isalnum(*c) )
					continue;

				// Only mutate some of the time
				if ( Default::tMutationFactor(1.0) - d(e) > (NormalMutationRate * factor) )
					continue;

				// Make sure we keep the same kind of glyph, only modify which one it is
				if ( isdigit(*c) )
					*c = (char)dDigit(e);
				else if ( isupper(*c) )
					*c = (char)dUpper(e);
				else
					*c = (char)dLower(e);
			}
			std::cout << " -- After: " << _data << std::endl;
		}
#endif // TRADITIONAL_GENETICS
	}
}
//////////////////////////////////////////////////////////////////////////////


#if !TRADITIONAL_GENETICS
	//////////////////////////////////////////////////////////////////////////
	// Give ourselves a way to visualize our non-traditional genomes
	//////////////////////////////////////////////////////////////////////////
	void PrintGenome( Toolbox::Genetics::Genome::Ptr genome, const std::string &name = std::string("") )
	{
		if ( !genome )
			return;

		std::string CurChromosome("");

		std::cout << "Genome: " << name << std::endl;

		CurChromosome = std::string( "Physical Traits" );
		auto ChromosomeList = genome->GetChromosome( CurChromosome );
		std::cout << "  " << CurChromosome << ":" << std::endl;

		for ( auto c = ChromosomeList.begin(), end = ChromosomeList.end(); c != end; ++c )
		{
			std::cout << "    Dominance: " << (*c)->Dominance << "  Gender: " << (*c)->Gender << std::endl;
			std::cout << "      Height:      " << (*c)->GetAllele< float >( "Height" ) << std::endl;
			std::cout << "      Weight:      " << (*c)->GetAllele< int >( "Weight" ) << std::endl;
		}

		CurChromosome = std::string( "Behavioral Traits" );
		ChromosomeList = genome->GetChromosome( CurChromosome );
		std::cout << "  " << CurChromosome << ":" << std::endl;

		for ( auto c = ChromosomeList.begin(), end = ChromosomeList.end(); c != end; ++c )
		{
			std::cout << "    Dominance: " << (*c)->Dominance << "  Gender: " << (*c)->Gender << std::endl;
			std::cout << "      Helpful:     " << (*c)->GetAllele< bool >( "Helpful" ) << std::endl;
			std::cout << "      Aggressive:  " << (*c)->GetAllele< bool >( "Aggressive" ) << std::endl;
		}

		CurChromosome = std::string( "Sex-Linked Traits" );
		ChromosomeList = genome->GetChromosome( CurChromosome );
		std::cout << "  " << CurChromosome << ":" << std::endl;

		for ( auto c = ChromosomeList.begin(), end = ChromosomeList.end(); c != end; ++c )
		{
			std::cout << "    Dominance: " << (*c)->Dominance << "  Gender: " << (*c)->Gender << std::endl;

			if ( (*c)->Gender == 'X' )
			{
				std::cout << "      Hemophilia:  " << (*c)->GetAllele< char >( "Hemophilia" ) << std::endl;
			}
			else if ( (*c)->Gender == 'Y' )
			{
				std::cout << "      Hair Length: " << (*c)->GetAllele< std::string >( "Hair Length" ) << std::endl;
				std::cout << "      Color Blind: " << (*c)->GetAllele< bool >( "Color Blind" ) << std::endl;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
#endif // !TRADITIONAL_GENETICS


//
// And finally, the actual program of interest
//
int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

	try
	{
#if TRADITIONAL_GENETICS
		//////////////////////////////////////////////////////////////////////////////
		// This is an example of a "traditional" genetic algorithm that can "solve" a
		// specific problem.
		//////////////////////////////////////////////////////////////////////////////
		int TargetNumber = 42;
		int FlockSize = 100;
		int MaxGenerations = 20000;

		std::cout << "Please enter the flock size.  This represents the number of organisms that will be used to search for a solution to this problem.  A healthy place to start is a flock size of 100 looking for a target number of 42." << std::endl;

		for ( ; ; )
		{
			std::cout << "Flock size: ";
			std::cin >> FlockSize;
			std::cout << "Flock size set to: " << FlockSize << std::endl;

			std::cout << "Please enter a target number: ";
			std::cin >> TargetNumber;
			std::cout << "Searching for a solution with an answer of " << TargetNumber << "..." << std::endl;

			Shepherd StandardExample( TargetNumber, FlockSize );	// Our shepherd trains/breeds our flock

			StandardExample.FindTargetNumber( MaxGenerations );
			std::cout << std::endl;
		}
		//////////////////////////////////////////////////////////////////////////////

#else // TRADITIONAL_GENETICS

		//////////////////////////////////////////////////////////////////////////////
		// This is an example of a "non-traditional" genetic algorithm that could be
		// used to generate NPC traits in a game, or someting similar.
		//////////////////////////////////////////////////////////////////////////////
		//
		// Create the "Adam" genome
		//
		Toolbox::Genetics::Genome::Ptr AdamGenome = std::make_shared< Toolbox::Genetics::Genome >();

		// Physical traits from (simulated) parent #1
		// Add a new chromosome (autosome) for "physical traits", and add alleles height and weight
		auto NewChromosome = AdamGenome->AddChromosome( "Physical Traits", 100 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.65 );
		NewChromosome->Alleles[ "Weight" ] = std::make_shared< Toolbox::Genetics::Allele<int> >( 75 );

		// Physical traits from (simulated) parent #2
		// Then add another new chromosome for "physical traits" (to simulate having one from a mother and father both)
		NewChromosome = AdamGenome->AddChromosome( "Physical Traits", 80 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.45 );
		NewChromosome->Alleles[ "Weight" ] = std::make_shared< Toolbox::Genetics::Allele<int> >( 65 );

		// Behavorial traits from (simulated) parent #1
		// Now, we add new chromosome for "behavioral traits", and add alleles to denote if this organism is helpful and/or aggressive
		NewChromosome = AdamGenome->AddChromosome( "Behavioral Traits", 100 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );

		// Behavorial traits from (simulated) parent #2
		// Then add another similar "behaviorial traits", as before (again simulating two parents)
		// This particular chromosome has a non-standard mutation rate of 80%, meaning this chromosome will mutate 80% of the time that this organism mutates during procreation, and twice as extreme as normal (2.0 factor instead of 1.0)
		NewChromosome = AdamGenome->AddChromosome( "Behavioral Traits", 75, Toolbox::Genetics::Chromosome::Autosome, 0.8, 2.0 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( true );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( false );

		// Sex-linked traits from (simulated) mom
		// Finally, we add a an allosome (sex-linked) gene for some special traits
		// Adding a chromosome with any gender value (except Toolbox::Genetics::Chromosome::Autosome) turns this into an allosome
		NewChromosome = AdamGenome->AddChromosome( "Sex-Linked Traits", 100, 'X' );
		NewChromosome->Alleles[ "Hemophilia" ] = std::make_shared< Toolbox::Genetics::Allele<char> >( 'A' );

		// Sex-linked traits from (simulated) dad
		// And another representing the opposite sex
		NewChromosome = AdamGenome->AddChromosome( "Sex-Linked Traits", 60, 'Y' );
		NewChromosome->Alleles[ "Hair Length" ] = std::make_shared< Toolbox::Genetics::Allele< std::string > >( std::string("Bald") );
		NewChromosome->Alleles[ "Color Blind" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );

		//
		// Create the "Eve" genome, in a similar style to above
		//
		Toolbox::Genetics::Genome::Ptr EveGenome = std::make_shared< Toolbox::Genetics::Genome >();

		// Physical traits from (simulated) parent #1
		NewChromosome = EveGenome->AddChromosome( "Physical Traits", 90 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.60 );
		NewChromosome->Alleles[ "Weight" ] = std::make_shared< Toolbox::Genetics::Allele<int> >( 63 );

		// Physical traits from (simulated) parent #2
		NewChromosome = EveGenome->AddChromosome( "Physical Traits", 0.5 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.85 );
		NewChromosome->Alleles[ "Weight" ] =  std::make_shared< Toolbox::Genetics::Allele<int> >( 80 );

		// Behavorial traits from (simulated) parent #1
		NewChromosome = EveGenome->AddChromosome( "Behavioral Traits", 85 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( false );

		// Behavorial traits from (simulated) parent #2
		NewChromosome = EveGenome->AddChromosome( "Behavioral Traits", 65 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( false );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( false );

		// Sex-linked traits from (simulated) mom
		NewChromosome = EveGenome->AddChromosome( "Sex-Linked Traits", 120, 'X' );
		NewChromosome->Alleles[ "Hemophilia" ] = std::make_shared< Toolbox::Genetics::Allele<char> >( '-' );

		// Sex-linked traits from (simulated) dad
		NewChromosome = EveGenome->AddChromosome( "Sex-Linked Traits", 60, 'X' );
		NewChromosome->Alleles[ "Hemophilia" ] = std::make_shared< Toolbox::Genetics::Allele<char> >( '-' );

		//
		// Print the genomes that we've just created
		//
		std::cout << "------------------------------------------------------------" << std::endl;
		PrintGenome( AdamGenome, "Adam" );
		std::cout << "------------------------------------------------------------" << std::endl;
		PrintGenome( EveGenome, "Eve" );
		std::cout << "------------------------------------------------------------" << std::endl;

		//
		// Create our organisms
		//
		// Create the original Adam and Eve of our population
		// NOTE: Mutation rates here decide if the organism as a whole will mutate at all or not (could be affected by environment or something)
		//       Here, the rate is currently set so the organism will mutate 80% of the time
		Toolbox::Genetics::Organism::Ptr Adam	= std::make_shared< Toolbox::Genetics::Organism >( AdamGenome, 1.0 ),
										 Eve	= std::make_shared< Toolbox::Genetics::Organism >( EveGenome, 1.0 );

		// Generate gametes from each parent
		auto Sperm = Adam->ProduceGamete();
		auto Egg = Eve->ProduceGamete();

		std::cout << "------------------------------------------------------------" << std::endl;
		PrintGenome( Sperm, "Adam Gamete" );
		std::cout << "------------------------------------------------------------" << std::endl;
		PrintGenome( Egg, "Eve Gamete" );
		std::cout << "------------------------------------------------------------" << std::endl;

		// Generate an embryo from these gametes
		auto Embryo = Egg->FertilizeWith( Sperm );

		PrintGenome( Embryo, "Embryo" );
		std::cout << "------------------------------------------------------------" << std::endl;

		// And once fertilization is complete, produce an offspring organism
		auto Offspring = Embryo->Gestate();

		PrintGenome( Offspring->Genetics(), "Offspring" );
		std::cout << "------------------------------------------------------------" << std::endl;
		//////////////////////////////////////////////////////////////////////////////
#endif // TRADITIONAL_GENETICS
	}
	catch ( std::exception &ex )
	{
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		ReturnCode = 1;
	}

	return ReturnCode;
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


