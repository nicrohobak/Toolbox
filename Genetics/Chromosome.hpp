#ifndef TOOLBOX_GENETICS_CHROMOSOME_H
#define TOOLBOX_GENETICS_CHROMOSOME_H

/*
 * chromosome.h
 *
 *
 */


#include <map>
#include <memory>
#include <sstream>
#include <string>

namespace Toolbox
{
	namespace Genetics
	{
		class tAllele : public std::enable_shared_from_this< tAllele >
		{
		public:
			typedef std::shared_ptr< tAllele >	Ptr;

		public:
			tAllele()
			{
			}

			virtual ~tAllele()
			{
			}

			virtual void Mutate( unsigned int rate = 0 ) = 0;	// Re-arrange the data in some way that is meaningful to the underlying data type

			template <typename tDataType>
			tDataType Get();

		protected:
		};


		template <typename tAlleleType>
		class Allele : public tAllele
		{
		public:
			typedef std::shared_ptr< Allele<tAlleleType> >	Ptr;

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

			virtual void Mutate( unsigned int rate = 0 );		// Each Allele type used will need to have a Mutate() function defined

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
			typedef std::shared_ptr< Chromosome >	Ptr;
			typedef unsigned int					tDominance;
			typedef char							tGender;
			typedef unsigned int					tMutationRate;
			typedef tMutationRate					tMutationFactor;

			static const tGender					Autosome = 0;

		public:
			tDominance				Dominance;
			tGender					Gender;
			std::map< std::string, tAllele::Ptr >	Alleles;

		public:
			Chromosome():
				Dominance( tDominance() ),
				Gender( tGender() ),
				_mutationRate( tMutationRate() ),
				_mutationFactor( tMutationFactor() )
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

			Chromosome( tDominance dominance, tGender gender = tGender(), tMutationRate rate = tMutationRate(), tMutationRate factor = tMutationFactor() ):
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

				if ( _mutationRate >= 100 )
					_mutationRate = 100;
			}

			tMutationFactor MutationFactor() const
			{
				return _mutationFactor;
			}

			void SetMutationFactor( tMutationFactor factor = tMutationFactor() )
			{
				_mutationFactor = factor;

				if ( _mutationFactor >= 100 )
					_mutationFactor = 100;
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
					if ( !(rand() % (100 - _mutationRate)) )
					{
						std::cout << "Mutating chromosome... (rate: " << _mutationRate << ", factor: " << _mutationFactor << ")" << std::endl;
						a->second->Mutate( _mutationFactor );
					}
				}
			}

		protected:
			tMutationRate			_mutationRate;		// Determines mutation rate of chromosomes on gametes produced
			tMutationFactor			_mutationFactor;	// Determines mutation rate of the data within each allele that gets mutated
		};
	}
}

#endif // TOOLBOX_GENETICS_CHROMOSOME_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


