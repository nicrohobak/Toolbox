#ifndef TOOLBOX_GENETICS_GENOME_H
#define TOOLBOX_GENETICS_GENOME_H

/*
 * genome.h
 *
 *
 */


#include <list>

#include <Toolbox/Genetics/Chromosome.hpp>


namespace Toolbox
{
	namespace Genetics
	{
		class Genome : public std::enable_shared_from_this< Genome >
		{
		public:
			typedef std::shared_ptr< Genome >		Ptr;
			typedef Chromosome::tDominance			tDominance;
			typedef Chromosome::tGender				tGender;
			typedef std::list< Chromosome::Ptr >	tChromosomeList;
			typedef Chromosome::tMutationRate		tMutationRate;
			typedef Chromosome::tMutationFactor		tMutationFactor;

		public:
			Genome()
			{
			}

			virtual ~Genome()
			{
			}

			const std::multimap< std::string, Chromosome::Ptr > &Allosomes() const
			{
				return _allosomes;
			}

			const std::multimap< std::string, Chromosome::Ptr > &Autosomes() const
			{
				return _autosomes;
			}

			size_t HaploidNumber() const							// How many copies of each chromosome does this genome have?
			{
				size_t HaploidNum = 0;

				if ( !_autosomes.empty() )
				{
					auto LastKey = _autosomes.upper_bound( _autosomes.begin()->first );
					HaploidNum = std::distance( _autosomes.begin(), LastKey );
				}
				else if ( !_allosomes.empty() )
				{
					auto LastKey = _allosomes.upper_bound( _allosomes.begin()->first );
					HaploidNum = std::distance( _allosomes.begin(), LastKey );
				}

				return HaploidNum;
			}

			tChromosomeList GetAllosome( const std::string &name ) const
			{
				tChromosomeList RetList;

				if ( name.empty() )
					return RetList;

				for ( auto c = _allosomes.find(name), upper = _allosomes.upper_bound(name), end = _allosomes.end(); c != end && c != upper; ++c )
					RetList.push_back( c->second );

				return RetList;
			}

			tChromosomeList GetAutosome( const std::string &name ) const
			{
				tChromosomeList RetList;

				if ( name.empty() )
					return RetList;

				for ( auto c = _autosomes.find(name), upper = _autosomes.upper_bound(name), end = _autosomes.end(); c != end && c != upper; ++c )
					RetList.push_back( c->second );

				return RetList;
			}

			tChromosomeList GetChromosome( const std::string &name ) const
			{
				tChromosomeList RetList;

				if ( name.empty() )
					return RetList;

				tChromosomeList Temp;

				RetList = this->GetAllosome( name );
				Temp = this->GetAutosome( name );

				RetList.splice( RetList.end(), Temp );

				return RetList;
			}

			Chromosome::Ptr AddChromosome( const std::string &name, tDominance dominance = tDominance(), tGender gender = tGender(), tMutationRate rate = tMutationRate(), tMutationFactor factor = tMutationFactor() )
			{
				Chromosome::Ptr NewChromosome;

				if ( name.empty() )
					return NewChromosome;

				NewChromosome = std::make_shared< Chromosome >( dominance, gender, rate, factor );

				if ( !gender )
					_autosomes.emplace( name, NewChromosome );
				else
					_allosomes.emplace( name, NewChromosome );

				return NewChromosome;
			}

			void AddChromosome( const std::string &name, Chromosome::Ptr chromosome )
			{
				if ( name.empty() || !chromosome )
					return;

				if ( !chromosome->Gender )
					_autosomes.emplace( name, chromosome );
				else
					_allosomes.emplace( name, chromosome );
			}

		protected:
			std::multimap< std::string, Chromosome::Ptr >	_allosomes;
			std::multimap< std::string, Chromosome::Ptr >	_autosomes;
		};
	}
}

#endif // TOOLBOX_GENETICS_GENOME_H


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


