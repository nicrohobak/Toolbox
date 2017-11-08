#ifndef TOOLBOX_MAP_COMMON_HPP
#define TOOLBOX_MAP_COMMON_HPP

/*
 * Toolbox/Map/Common.hpp
 *
 * A generic Map class, suitable for creating game worlds, etc.
 */

/*****************************************************************************
 * Map Information:
 *
 * - The Map class is a flat x/y grid (of varying types: square, hex, etc.)
 *   with an added Z coordinate for height (increasing Z moves downward).
 *   This is effectively a top-down view of a 3D grid.
 *
 * - Maps are stored volumetrically (think voxels) with RLE encoding on the Z
 *   axis.  Any map tiles placed will automatically change all types below it
 *   into that same type.
 *
 * - Each Map has its own Map::Token type that represents objects (or more
 *   formally, tokens) for that map type.  Tokens themselves have a nested
 *   map to represent their physical "size" in the map.
 *
 * - Maps can be given custom tile types through tMap<> template parameters.
 *   Tokens can also be given separate custom tile types through the
 *   tMap<>::tToken<> template parameters.  Tokens take up the same space in
 *   map units as map tiles, but tokens will not be restricted to only one
 *   tile in size (they can be "painted" like voxels in a map too).
 *
 * - Map coordinate types can also be modified through tMap<> template
 *   parameters.  A signed type is expected with (0,0,0) as the origin.
 *
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <cstdlib>
#include <map>

#include <Toolbox/Defines.h>


namespace Toolbox
{
	namespace Map
	{
		namespace Default
		{
			typedef int		tCoordType;


			enum tTile
			{
				Tile_Empty,
				Tile_Solid,
				Tile_Other,

				Tile_MAX,

				Tile_FIRST =	Tile_Empty,
				Tile_DEFAULT =	Tile_FIRST,
			};


			enum tToken
			{
				Token_Empty,
				Token_Solid,
				Token_Other,

				Token_MAX,

				Token_FIRST =	Token_Empty,
				Token_DEFAULT =	Token_FIRST,
			};
		};


		template <typename _tCoordType = Default::tCoordType>
		class tCoordinate : public std::enable_shared_from_this< tCoordinate<_tCoordType> >
		{
		public:
			typedef _tCoordType		tCoordType;

			TOOLBOX_POINTERS( tCoordinate<tCoordType> )

		public:
			tCoordType				X, Y, Z;

		public:
			tCoordinate():
				X( tCoordType() ),
				Y( tCoordType() ),
				Z( tCoordType() )
			{
			}

			tCoordinate( const tCoordType &x, const tCoordType &y, const tCoordType &z ):
				X( x ), Y( y ), Z( z )
			{
			}

			virtual ~tCoordinate()
			{
			}
		};

		typedef tCoordinate<>		Coordinate;


		template <typename _tTileData = Default::tTile>
		class tTile : public std::enable_shared_from_this< tTile<_tTileData> >
		{
		public:
			typedef _tTileData		tTileData;

			TOOLBOX_POINTERS( tTile<tTileData> )

		public:
			tTileData		Data;

		public:
			tTile()
			{
			}

			tTile( const tTileData &data ):
				Data( data )
			{
			}

			virtual ~tTile()
			{
			}

			// A* algorithm needs go here, etc.
		};


		template <typename _tTileData = Default::tTile, typename _tCoordType = Default::tCoordType>
		class tMap : public std::enable_shared_from_this< tMap<_tTileData,_tCoordType> >
		{
		public:
			typedef std::shared_ptr< tMap<_tTileData, _tCoordType> >	Ptr;
			typedef _tCoordType											tCoordType;
			typedef _tTileData											tTileData;
			typedef tCoordinate< tCoordType >							Coordinate;
			typedef tTile< tTileData >									Tile;
			typedef std::map< tCoordType, Tile >						Column;		// Each x,y map coordinate is a column of map positions

		public:
			///////////////////////////////////////
			/*
			 * The Token class depends a bit on its associate map type.
			 *
			 * Defining the Token within the map type yields not only a better
			 *   association in the code, but better resulting library syntax
			 *   too...ugly as it may seem...
			 */
			class Token : public std::enable_shared_from_this< Token >
			{
			public:
				TOOLBOX_POINTERS( Token )

				virtual ~Token()
				{
				}

				virtual Coordinate Size() = 0;
			};
			///////////////////////////////////////

		public:
			std::map< tCoordType,
				std::map< tCoordType, Column >>				Data;		// 3D array constructed of std::map<>

			std::map< typename Token::Ptr, Coordinate >		Tokens;

		public:
			tMap()
			{
			}

			virtual ~tMap()
			{
			}

			// Somewhat redundant because direct access is available...
			void PlaceToken( typename Token::Ptr token, const Coordinate &target = Coordinate() )
			{
				if ( !token )
					throw std::runtime_error( "Toolbox::Map::tMap<>::PlaceToken(): No token provided." );

				Tokens[ token ] = target;
			}

			void RemoveToken( typename Token::Ptr token )
			{
				if ( !token )
					throw std::runtime_error( "Toolbox::Map::tMap<>::RemoveToken(): No token provided." );

				auto Found = Tokens.find( token );

				if ( Found != Tokens.end() )
					Tokens.erase( Found );
			}

			virtual tCoordType Distance( const Coordinate &orign, const Coordinate &target = Coordinate() ) = 0;

			Coordinate Size()
			{
				Coordinate ReturnSize( 1, 1, 1 );				// A single map unit by default

				Coordinate Min, Max;

				// The X minimum is easy since the std::map<> takes care of it for us
				auto XMinCheck = Data.begin();
				if ( XMinCheck != Data.end() )
					Min.X = XMinCheck->first;

				// Same with the X maximum
				auto XMaxCheck = Data.rbegin();
				if ( XMaxCheck != Data.rend() )
					Max.X = XMaxCheck->first;

				// But for the other dimensions, we have to iterate a bit to be sure to cover everything
				for ( auto x = Data.begin(), x_end = Data.end(); x != x_end; ++x )
				{
					// Check our minimum first
					auto YMinCheck = x->second.begin();
					if ( YMinCheck != x->second.end() && YMinCheck->first <= Min.Y )
						Min.Y = YMinCheck->first;

					// Then check the maximum
					auto YMaxCheck = x->second.rbegin();
					if ( YMaxCheck != x->second.rend() && YMaxCheck->first >= Max.Y )
						Max.Y = YMaxCheck->first;

					for ( auto y = x->second.begin(), y_end = x->second.end(); y != y_end; ++y )
					{
						// Check our minimum first
						auto ZMinCheck = y->second.begin();
						if ( ZMinCheck != y->second.end() && ZMinCheck->first <= Min.Z )
							Min.Z = ZMinCheck->first;

						// Then check the maximum
						auto ZMaxCheck = y->second.rbegin();
						if ( ZMaxCheck != y->second.rend() && ZMaxCheck->first >= Max.Z )
							Max.Z = ZMaxCheck->first;
					}
				}	

				ReturnSize.X = this->Distance( Coordinate(Min.X, 0,     0    ),
											   Coordinate(Max.X, 0,     0    ) );
				ReturnSize.Y = this->Distance( Coordinate(0,     Min.Y, 0    ),
											   Coordinate(0,     Max.Y, 0    ) );
				ReturnSize.Z = this->Distance( Coordinate(0,     0,     Min.Z),
											   Coordinate(0,     0,     Max.Z) );

				// Add one to our distance calculations to also include the origin spot
				ReturnSize.X++;
				ReturnSize.Y++;
				ReturnSize.Z++;

				return ReturnSize;

			}

			const tTileData &At( const tCoordinate<tCoordType> &target )
			{
				Column *GridSpot = &Data[target.X][target.Y];

				auto t_end = GridSpot->end();
				auto t_last = t_end;								// Keeps track of the last checked tile
				for ( auto t = GridSpot->begin(); t != t_end; ++t )
				{
					t_last = t;

					if ( t->first == target.Z )
						return t->second.Data;
					else if ( t->first > target.Z )
					{
						auto t_prev = t;
						--t;

						if ( t != t_end )
							return t->second.Data;
						else
							return tTileData();
					}
				}

				// Not found...but if we had anything at all prior, that's our type too
				if ( t_last != t_end )
					return t_last->second.Data;

				// Not found at all
				return tTileData();
			}

			void PlaceTile( const Tile &tile, const tCoordinate<tCoordType> &target )
			{
				Data[target.X][target.Y][target.Z] = tile;
			}

			void RemoveTile( const tCoordinate<tCoordType> &target )
			{
				Column *GridSpot = &Data[target.X][target.Y];

				if ( GridSpot->empty() )
					return;

				auto Found = GridSpot->find( target.Z );

				if ( Found != GridSpot->end() )
					GridSpot->erase( Found );
			}
		};

		typedef tMap<>					Map;
	}
}


#endif // TOOLBOX_MAP_COMMON_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper



