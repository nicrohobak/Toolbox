#ifndef TOOLBOX_GENETICS_ORGANISM_HPP
#define TOOLBOX_GENETICS_ORGANISM_HPP

/*
 * Organism.hpp
 *
 * Organisms contain genomes and can mate with (combine genomes with) other
 * organisms via their gametes (sperm/eggs).
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
				MutationRate( Default::MutationRate ),
				_numParents( 1 )
			{
			}

			Organism( Genome::Ptr genome, const tMutationRate &rate = Default::MutationRate ):
				MutationRate( rate ),
				_genome( genome )
			{
				_numParents = genome->HaploidNumber();

				if ( _numParents < 1 )
					_numParents = 1;
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

				size_t Divisor = _numParents;

				if ( Divisor < 1 )
					Divisor = 1;

				static std::uniform_real_distribution< tMutationRate >	d{ tMutationRate(0.0), tMutationRate(1.0) };   // Generate within this range
				static std::random_device								rd;
				static std::default_random_engine						e( rd() );

				// Add as many allosomes as we need, with some randomization
				size_t NumNeededAllosomes = _genome->Allosomes().size() / Divisor;

				for ( auto begin = _genome->Allosomes().begin(), end = _genome->Allosomes().end(), c = begin; c != end && NumNeededAllosomes > 0; )
				{
					// Use our number of parents to determine how many sex-linked genes we can skip over safely in order to randomize which is actually passed down
					static std::uniform_int_distribution< size_t >		dWhichAllosome{ 1, _numParents };
					size_t SkipCount = _numParents - dWhichAllosome(e);
					for ( size_t i = 0; i < SkipCount; ++i )
						++c;

					auto NewChromosome = std::make_shared< Chromosome >( Chromosome(*(c->second)) );

					if ( tMutationRate(1.0) - d(e) < MutationRate )
						NewChromosome->Mutate();

					NewGamete->AddChromosome( c->first, NewChromosome );
					--NumNeededAllosomes;

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

					static std::uniform_int_distribution< size_t >	dWhichAutosome{ 1, NumElements };   // Generate within this range
					size_t Index = 0, Which = dWhichAutosome(e) - 1; // -1 to account for the 0-based index
					for ( auto h = c; h != end && h != LastKey; ++h, ++Index )
					{
						if ( Index == Which )
						{
							auto NewChromosome = std::make_shared< Chromosome >( Chromosome(*(h->second)) );
							if ( tMutationRate(1.0) - d(e) < MutationRate )
								NewChromosome->Mutate();

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


