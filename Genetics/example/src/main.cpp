/*
 * main.cpp
 *
 * An working example for Toolbox::Genetics
 */


#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <Toolbox/Genetics.hpp>


/*
 * Mutation functions MUST be defined for any data types used in our genome
 *
 * - The "rate" parameter is typically a percentage chance for mutation and/or
 *   extremity of mutation
 */
namespace Toolbox
{
	namespace Genetics
	{
		//
		// Mutation function for alleles of type bool
		//
		template <>
		void Allele< bool >::Mutate( const tMutationFactor &factor )
		{
			static std::uniform_int_distribution< int >		d{ 0, 1 };   // Coin toss
			static std::random_device						rd;
			static std::default_random_engine				e( rd() );

			std::cout << "Mutating bool (factor: " << factor << ")" << std::endl;

			std::cout << " -- Before: " << _data << std::endl;
			_data = (bool)(d(e) * factor);
			std::cout << " -- After: " << _data << std::endl;
		}


		//
		// Mutation function for alleles of type char
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

			const float CharMutationRate								= 0.5;				// 50%

			// Factor doesn't really matter here...at least in this current implementation
			std::cout << "Mutating char (factor: " << factor << ")" << std::endl;

			// Only alter alphanumeric glyphs
			if ( !isalnum(_data) )
				return;

			// Only mutate some of the time
			if ( Default::tMutationFactor(1.0) - d(e) > (CharMutationRate * factor) )
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
		// Mutation function for alleles of type int
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
		// Mutation function for alleles of type float
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
		// Mutation function for alleles of type string
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

			const float StringMutationRate								= 0.5;				// 50%

			std::cout << "Mutating string (factor: " << factor << ")" << std::endl;

			std::cout << " -- Before: " << _data << std::endl;
			for ( auto c = _data.begin(), end = _data.end(); c != end; ++c )
			{
				// Only alter alphanumeric glyphs
				if ( !isalnum(*c) )
					continue;

				// Only mutate some of the time
				if ( Default::tMutationFactor(1.0) - d(e) > (StringMutationRate * factor) )
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
	}
}



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


int main( int argc, char *argv[] )
{
	int ReturnCode = 0;

	try
	{
		srand( time(NULL) );

		Toolbox::Genetics::Genome::Ptr AdamGenome, EveGenome;
		Toolbox::Genetics::Organism::Ptr Adam, Eve;

		//
		// Create the "Adam" genome
		//
		AdamGenome = std::make_shared< Toolbox::Genetics::Genome >();

		// Add a new chromosome (autosome) for "physical traits", and add alleles height and weight
		auto NewChromosome = AdamGenome->AddChromosome( "Physical Traits", 100 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.65 );
		NewChromosome->Alleles[ "Weight" ] = std::make_shared< Toolbox::Genetics::Allele<int> >( 75 );

		// Then add another new chromosome for "physical traits" (to simulate having one from a mother and father both)
		NewChromosome = AdamGenome->AddChromosome( "Physical Traits", 80 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.45 );
		NewChromosome->Alleles[ "Weight" ] = std::make_shared< Toolbox::Genetics::Allele<int> >( 65 );

		// Now, we add new chromosome for "behavioral traits", and add alleles to denote if this organism is helpful and/or aggressive
		NewChromosome = AdamGenome->AddChromosome( "Behavioral Traits", 100 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );

		// Then add another similar "behaviorial traits", as before (again simulating two parents)
		// This particular chromosome has a non-standard mutation rate of 80%, meaning this chromosome will mutate 80% of the time that this organism mutates during procreation, and twice as extreme as normal (2.0 factor instead of 1.0)
		NewChromosome = AdamGenome->AddChromosome( "Behavioral Traits", 75, Toolbox::Genetics::Chromosome::Autosome, 0.8, 2.0 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( true );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( false );

		// Finally, we add a an allosome (sex-linked) gene for some special traits
		// Adding a chromosome with any gender value (except Toolbox::Genetics::Chromosome::Autosome) turns this into an allosome
		NewChromosome = AdamGenome->AddChromosome( "Sex-Linked Traits", 100, 'X' );
		NewChromosome->Alleles[ "Hemophilia" ] = std::make_shared< Toolbox::Genetics::Allele<char> >( 'A' );

		// And another representing the opposite sex
		NewChromosome = AdamGenome->AddChromosome( "Sex-Linked Traits", 60, 'Y' );
		NewChromosome->Alleles[ "Hair Length" ] = std::make_shared< Toolbox::Genetics::Allele< std::string > >( std::string("Bald") );
		NewChromosome->Alleles[ "Color Blind" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );

		//
		// Create the "Eve" genome, in a similar style to above
		//i
		EveGenome = std::make_shared< Toolbox::Genetics::Genome >();

		// Physical traits from parent #1
		NewChromosome = EveGenome->AddChromosome( "Physical Traits", 90 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.60 );
		NewChromosome->Alleles[ "Weight" ] = std::make_shared< Toolbox::Genetics::Allele<int> >( 63 );

		// Physical traits from parent #2
		NewChromosome = EveGenome->AddChromosome( "Physical Traits", 0.5 );
		NewChromosome->Alleles[ "Height" ] = std::make_shared< Toolbox::Genetics::Allele<float> >( 1.85 );
		NewChromosome->Alleles[ "Weight" ] =  std::make_shared< Toolbox::Genetics::Allele<int> >( 80 );

		// Behavorial traits from parent #1
		NewChromosome = EveGenome->AddChromosome( "Behavioral Traits", 85 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( true );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( false );

		// Behavorial traits from parent #2
		NewChromosome = EveGenome->AddChromosome( "Behavioral Traits", 65 );
		NewChromosome->Alleles[ "Helpful" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( false );
		NewChromosome->Alleles[ "Aggressive" ] = std::make_shared< Toolbox::Genetics::Allele<bool> >( false );

		// Sex-linked traits from mom
		NewChromosome = EveGenome->AddChromosome( "Sex-Linked Traits", 120, 'X' );
		NewChromosome->Alleles[ "Hemophilia" ] = std::make_shared< Toolbox::Genetics::Allele<char> >( '-' );

		// Sex-linked traits from dad
		NewChromosome = EveGenome->AddChromosome( "Sex-Linked Traits", 60, 'X' );
		NewChromosome->Alleles[ "Hemophilia" ] = std::make_shared< Toolbox::Genetics::Allele<char> >( '-' );


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
		Adam = std::make_shared< Toolbox::Genetics::Organism >( AdamGenome, 1.0 );
		Eve = std::make_shared< Toolbox::Genetics::Organism >( EveGenome, 1.0 );

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


