#ifndef TOOLBOX_GENETICS_CHROMOSOME_HPP
#define TOOLBOX_GENETICS_CHROMOSOME_HPP

/*
 * Chromosome.hpp
 *
 * The "core" of the genetic algorithm.  Gene information (alleles) grouped into
 * chromosomes (logical groupings of different alleles).
 */

#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>

#include <Toolbox/Memory.hpp>


namespace Toolbox
{
	namespace Genetics
	{
		namespace Default
		{
			typedef float			tMutationRate;			// How often something mutates
			typedef tMutationRate	tMutationFactor;		// To what extent something mutates

			const tMutationRate		MutationRate	= 0.2f;	// 20%
			const tMutationFactor	MutationFactor	= 1.0f;	// Often a multiplication factor to the newly generated allele value
		}


		class tAllele : public std::enable_shared_from_this< tAllele >
		{
		public:
			TOOLBOX_MEMORY_POINTERS( tAllele )

			typedef Default::tMutationFactor		tMutationFactor;

		public:
			tAllele()
			{
			}

			virtual ~tAllele()
			{
			}

			virtual void Mutate( const tMutationFactor &rate = tMutationFactor() ) = 0;	// Re-arrange the data in some way that is meaningful to the underlying data type

			template <typename tDataType>
			tDataType Get();

		protected:
		};


		template <typename tAlleleType>
		class Allele : public tAllele
		{
		public:
			TOOLBOX_MEMORY_POINTERS( Allele<tAlleleType> )

			typedef Default::tMutationFactor		tMutationFactor;

		public:
			Allele()
			{
			}

			Allele( const tAlleleType &value ):
				_data( value )
			{
			}

			virtual ~Allele()
			{
			}

			virtual void Mutate( const tMutationFactor &rate = tMutationFactor() );		// Each Allele type used will need to have a Mutate() function defined

			tAlleleType Get() const
			{
				return _data;
			}

		protected:
			tAlleleType		_data;

			friend class tAllele;
		};


		template <typename tDataType>
		tDataType tAllele::Get()
		{
			typename Allele< tDataType >::Ptr RealAllele = std::dynamic_pointer_cast< Allele<tDataType> >( shared_from_this() );

			if ( RealAllele )
				return RealAllele->_data;

			return tDataType();
		}


		class Chromosome
		{
		public:
			TOOLBOX_MEMORY_POINTERS( Chromosome )

			typedef unsigned int					tDominance;
			typedef char							tGender;
			typedef Default::tMutationRate			tMutationRate;
			typedef tMutationRate					tMutationFactor;

			static const tGender					Autosome = 0;

		public:
			tDominance								Dominance;
			tGender									Gender;
			std::map< std::string, tAllele::Ptr >	Alleles;

		public:
			Chromosome():
				Dominance( tDominance() ),
				Gender( tGender() ),
				_mutationRate( Default::MutationRate ),
				_mutationFactor( Default::MutationFactor )
			{
			}

			Chromosome( const Chromosome &copy ):
				Dominance( copy.Dominance ),
				Gender( copy.Gender ),
				_mutationRate( copy._mutationRate ),
				_mutationFactor( copy._mutationFactor )
			{
				Alleles = copy.Alleles;
			}

			Chromosome( tDominance dominance, tGender gender = tGender(), tMutationRate rate = Default::MutationRate, tMutationRate factor = Default::MutationFactor ):
				Dominance( dominance ),
				Gender( gender ),
				_mutationRate( rate ),
				_mutationFactor( factor )
			{
			}

			~Chromosome()
			{
			}

			tMutationRate MutationRate() const
			{
				return _mutationRate;
			}

			void SetMutationRate( tMutationRate rate = tMutationRate() )
			{
				_mutationRate = rate;
			}

			tMutationFactor MutationFactor() const
			{
				return _mutationFactor;
			}

			void SetMutationFactor( tMutationFactor factor = tMutationFactor() )
			{
				_mutationFactor = factor;
			}

			template <typename tAlleleType>
			tAlleleType GetAllele( const std::string &name ) const
			{
				if ( name.empty() )
					return tAlleleType();

				auto Allele = this->Alleles.find( name );

				if ( Allele == this->Alleles.end() )
					return tAlleleType();

				return Allele->second->Get< tAlleleType >();
			}

			void Mutate()
			{
				if ( !_mutationRate )
					return;

				for ( auto a = Alleles.begin(), end = Alleles.end(); a != end; ++a )
				{
	                static std::uniform_real_distribution< tMutationRate >  d{ tMutationRate(0.0), tMutationRate(1.0) };   // Generate within this range
	                static std::random_device                       		rd;
	                static std::default_random_engine               		e( rd() );
												                
	                if (  tMutationRate(1.0) - d(e) < _mutationRate )
						a->second->Mutate( _mutationFactor );
				}
			}

		protected:
			tMutationRate			_mutationRate;		// Determines mutation rate of chromosomes on gametes produced (how often)
			tMutationFactor			_mutationFactor;	// Determines mutation rate of the data within each allele that gets mutated (to what extent)
		};
	}
}

#endif // TOOLBOX_GENETICS_CHROMOSOME_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


