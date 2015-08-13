#ifndef TOOLBOX_GENETICS_SHEPHERD_HPP
#define TOOLBOX_GENETICS_SHEPHERD_HPP

/*
 * Shepherd.hpp
 *
 * A fitness/breeding manager for Organisms.
 */

#include <Toolbox/Genetics/Organism.hpp>


namespace Toolbox
{
	namespace Genetics
	{
		template <typename tOrganism = Organism>
		class Shepherd : public std::enable_shared_from_this< Shepherd<tOrganism> >
		{
		public:
			TOOLBOX_MEMORY_POINTERS( Shepherd<tOrganism> )

			typedef std::list< typename tOrganism::Ptr >	tFlock;

		public:
			tFlock			Flock;

		public:
			Shepherd()
			{
			}

			virtual ~Shepherd()
			{
			}

			void AddToFlock( const typename tOrganism::Ptr organism )
			{
				if ( !organism )
					throw std::runtime_error( "Toolbox::Genetics::Shepherd::AddToFlock(): No organism provided." );

				Flock.push_back( organism );
			}

			void AddToFlock( const tFlock &flock )
			{
				Flock.insert( Flock.end(), flock.begin(), flock.end() );
			}

			// The fitness function
			virtual double Rate( const Organism::Ptr organism ) const = 0;

			// Iterates the flock generation
			virtual void BreedFlock( float topPercent = 0.20 )
			{
				size_t FlockSize = Flock.size();

				if ( FlockSize <= 0 )
					throw std::runtime_error( "Toolbox::Genetics::Shepherd::BreedFlock(): There must be at least one organism in the flock in order to breed." );

				tFlock BreedFlock, NewFlock;
				std::map< typename tOrganism::Ptr, double > FlockRatings;
				size_t NumToBreed = FlockSize * topPercent;		// topPercent of the "best" of the flock

				if ( NumToBreed == 0 )
					throw std::runtime_error( "Toolbox::Genetics::Shepherd::BreedFlock(): Your flock has died off.  (NumToBreed == 0)" );

				// Rate everyone
				for ( auto f = Flock.begin(), f_end = Flock.end(); f != f_end; ++f )
					FlockRatings[ *f ] = this->Rate( *f );

				// Find the best of the best
				for ( size_t n = 0; n < NumToBreed; ++n )
				{
					if ( FlockRatings.empty() )
						break;

					auto CurBestOrganism = FlockRatings.end();
					for ( auto r = FlockRatings.begin(), r_end = FlockRatings.end(); r != r_end; ++r )
					{
						if ( r->second > CurBestOrganism->second || CurBestOrganism == FlockRatings.end() )
							CurBestOrganism = r;
					}

					if ( CurBestOrganism != FlockRatings.end() )
					{
						BreedFlock.push_back( CurBestOrganism->first );
						FlockRatings.erase( CurBestOrganism );
					}
				}

				// Breed the best of the best
				if ( !BreedFlock.empty() )
				{
					const size_t NumParents = (*BreedFlock.begin())->Genetics()->HaploidNumber();

					// Pull parents from the breed flock and breed until we have enough for our new flock
					for ( size_t n = 0; n < FlockSize; ++n )
					{
						// Select a few at random, rerolling if we get a repeat value
						static std::uniform_int_distribution< int >	d{ 1, BreedFlock.size() };	// Generate within this range
						static std::random_device					rd;
						static std::default_random_engine			e( rd() );

						tFlock Parents;
						std::list< size_t > ParentIDs;

						// Determine who our parents are
						for ( size_t p = 0; p < NumParents; ++p )
						{
							size_t NewParent = 0;

							bool Reroll = true;		// Make sure we roll at least once
							while ( Reroll )		//   and keep going until we've got unique parents
							{
								Reroll = false;		// But only reroll if we have to, of course

								NewParent = d(e);

								if ( ParentIDs.empty() )
								{
									ParentIDs.push_back( NewParent );
									break;
								}

								// Check for duplicates
								for ( auto i = ParentIDs.begin(), i_end = ParentIDs.end(); i != i_end; ++i )
								{
									if ( NewParent == *i )
									{
										Reroll = true;
										break;
									}
								}
							}

							ParentIDs.push_back( NewParent );
						}

						// Then pull them from the flock
						size_t Count = 0;
						for ( auto f = Flock.begin(), f_end = Flock.end(); f != f_end; ++f, ++Count )
						{
							for ( auto p = ParentIDs.begin(), p_end = ParentIDs.end(); p != p_end; ++p )
							{
								if ( *p == Count )
								{
									Parents.push_back( *f );
									ParentIDs.erase( p );
									break;
								}
							}
						}

						// Then breed our parents
						Embryo::Ptr NewEmbryo;
						for ( auto p = Parents.begin(), p_end = Parents.end(); p != p_end; ++p )
						{
							if ( !NewEmbryo )
								NewEmbryo = std::make_shared< Embryo >( (*p)->ProduceGamete() );
							else
								NewEmbryo->FertilizeWith( (*p)->ProduceGamete() );
						}

						// And birth the new organism into the new flock
						NewFlock.push_back( NewEmbryo->Gestate< tOrganism >() );
					}
				}
				else
					throw std::runtime_error( "Toolbox::Genetics::Shepherd::BreedFlock(): Breed flock empty!" );

				// Update our Flock
				Flock = NewFlock;
			}
		};
	}
}


#endif // TOOLBOX_GENETICS_SHEPHERD_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-namespaces --indent-cases --pad-oper


