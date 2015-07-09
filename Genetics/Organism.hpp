#ifndef TOOLBOX_GENETICS_ORGANISM_H
#define TOOLBOX_GENETICS_ORGANISM_H

/*
 * organism.h
 *
 *
 */


#include <Toolbox/Genetics/Genome.hpp>


namespace Toolbox
{
	namespace Genetics
	{
		// Forward declarations
		class Embryo;
		class Organism;


		class Gamete : public Genome
		{
		public:
			TOOLBOX_MEMORY_POINTERS( Gamete )

		public:
			Gamete()
			{
			}

			virtual ~Gamete()
			{
			}

			std::shared_ptr< Embryo > FertilizeWith( Gamete::Ptr other );

			friend class Embryo;
			friend class Organism;
		};


		class Embryo : public Gamete
		{
		public:
			TOOLBOX_MEMORY_POINTERS( Embryo )

		public:
			Embryo():
				_numParents( 0 )
			{
			}

			Embryo( Gamete::Ptr parent1, Gamete::Ptr parent2 ):
				_numParents( 0 )
			{
				if ( parent1 )
				{
					this->_allosomes = parent1->_allosomes;
					this->_autosomes = parent1->_autosomes;
				}

				if ( parent2 )
				{
					this->_allosomes.insert( parent2->_allosomes.begin(), parent2->_allosomes.end() );
					this->_autosomes.insert( parent2->_autosomes.begin(), parent2->_autosomes.end() );
				}

				_numParents = 2;
			}

			virtual ~Embryo()
			{
			}

			void FertilizeWith( Gamete::Ptr other )
			{
				if ( !other )
					return;

				_allosomes.insert( other->_allosomes.begin(), other->_allosomes.end() );
				_autosomes.insert( other->_autosomes.begin(), other->_autosomes.end() );

				++_numParents;
			}

			std::shared_ptr< Organism > Gestate();

		protected:
			size_t		_numParents;
		};


		std::shared_ptr< Embryo > Gamete::FertilizeWith( Gamete::Ptr other )
		{
			return std::make_shared< Embryo >( std::dynamic_pointer_cast< Gamete >(this->shared_from_this()), other );
		}


		class Organism
		{
		public:
			TOOLBOX_MEMORY_POINTERS( Organism )

			typedef Chromosome::tMutationRate	tMutationRate;
			typedef Genome::tChromosomeList		tChromosomeList;

		public:
			tMutationRate						MutationRate;		// Typically between 0 (off) and 100 -- determines mutation rate of chromosomes on gametes produced

		public:
			Organism():
				MutationRate( 0 ),
				_numParents( 0 )
			{
			}

			Organism( Genome::Ptr genome, size_t numParents = 0, tMutationRate rate = tMutationRate() ):
				MutationRate( rate ),
				_genome( genome ),
				_numParents( numParents )
			{
			}

			~Organism()
			{
			}

			inline const Genome::Ptr Genetics() const
			{
				return _genome;
			}

			// TODO: For dominance, if two alleles both happen to have the same dominance rating, they should be merged together so that both are expressed...this may require a new function like Mutate() though...
			// TODO: Mutate() should have default function definitions in their own .hpp file that can be easily included if desired.  Probably one .hpp per function and one .hpp that includes all of them at once
			template <typename tAlleleType>
			tAlleleType GetPhenotype( const std::string &trait, const std::string &chromosome = std::string() ) const
			{
				tAlleleType RetVal = tAlleleType();

				if ( trait.empty() )
					return RetVal;

				if ( chromosome )
				{
				}
				else
				{
				}

				return RetVal;
			}

			Gamete::Ptr ProduceGamete() const
			{
				// TODO: Add chromosomal swapping -- On a random chance, allow chromosomes to swap alleles with their counterpart...in part, or in whole
				// TODO: Add some method to introduce control to this process...number of needed allosomes and of what types, etc.
				// TODO: Possibly add a more distinct gender variable to the objects, along with some method to determine gender from a genome

				Gamete::Ptr NewGamete;

				if ( !_genome )
					return NewGamete;

				// Put stuff into the gamete
				NewGamete = std::make_shared< Gamete >();

				//size_t HaploidNumber = _genome->HaploidNumber();

				unsigned int Divisor = _numParents;

				if ( Divisor < 1 )
					Divisor = 1;

				// Add as many allosomes as we need, with some randomization
				unsigned int NumNeededAllosomes = _genome->Allosomes().size() / Divisor;
				for ( auto begin = _genome->Allosomes().begin(), end = _genome->Allosomes().end(), c = begin; c != end && NumNeededAllosomes > 0; )
				{
					if ( !(rand() % 2) )
					{
						auto NewChromosome = std::make_shared< Chromosome >( Chromosome(*(c->second)) );

						if ( !MutationRate && (rand() % (100 - MutationRate)) && (rand() % (100 - MutationRate)) )
						{
							std::cout << "Checking for gamete allosome mutation..." << std::endl;
							NewChromosome->Mutate();
						}

						NewGamete->AddChromosome( c->first, NewChromosome );
						--NumNeededAllosomes;
					}

					auto Next = c;
					if ( ++Next == end )
						c = begin;
					else
						++c;
				}

				// One of each type of autosome, with some randomization
				for ( auto c = _genome->Autosomes().begin(), end = _genome->Autosomes().end(); c != end; )
				{
					auto LastKey = _genome->Autosomes().upper_bound( c->first );

					size_t NumElements = std::distance( c, LastKey );

					unsigned int Index = 0, Which = rand() % NumElements;
					for ( auto h = c; h != end && h != LastKey; ++h, ++Index )
					{
						if ( Index == Which )
						{
							auto NewChromosome = std::make_shared< Chromosome >( Chromosome(*(h->second)) );

							if ( !MutationRate && (rand() % (100 - MutationRate)) && (rand() % (100 - MutationRate)) )
							{
								std::cout << "Checking for gamete autosome mutation..." << std::endl;
								NewChromosome->Mutate();
							}

							NewGamete->AddChromosome( h->first, NewChromosome );
							break;
						}
					}

					c = LastKey;
				}

				return NewGamete;
			}

		protected:
			Genome::Ptr		_genome;
			size_t			_numParents;
		};


		Organism::Ptr Embryo::Gestate()
		{
			Organism::Ptr NewOrganism = std::make_shared< Organism >( shared_from_this(), _numParents );
			return NewOrganism;
		}
	}
}


#endif // TOOLBOX_GENETICS_ORGANISM_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


