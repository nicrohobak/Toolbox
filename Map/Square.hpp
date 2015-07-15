#ifndef TOOLBOX_MAP_SQUARE_HPP
#define TOOLBOX_MAP_SQUARE_HPP

/*
 * Toolbox/Map/Square.hpp
 */

/*****************************************************************************
 * See documentation in Toolbox/Map/Common.hpp
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <Toolbox/Map/Common.hpp>


namespace Toolbox
{
	namespace Map
	{
		template <typename _tTileData = Default::tTile, typename _tCoordType = Default::tCoordType>
		class tSquare : public tMap< _tTileData, _tCoordType >
		{
		public:
			typedef _tCoordType						tCoordType;
			typedef _tTileData						tTileData;
			typedef tMap< tTileData, tCoordType >	tParent;

			///////////////////////////////////////
			/*
			 * The only "problem" with defining tokens this way, is that each map
			 * type now MUST define its associated token type as well.
			 */
			template <typename _tTokenData = Default::tToken>
			class tToken : public tParent::Token
			{
			public:
				typedef _tTokenData						tTokenData;
				typedef tSquare< tTokenData, tCoordType >	tData;

				TOOLBOX_MEMORY_POINTERS( tToken<tTokenData> )

			public:
				tSquare< tTokenData, tCoordType >		Data;		// Token data is of the same map type, but with a potentially different tileset

			public:
				virtual ~tToken()
				{
				}

				tCoordType Distance( const Coordinate &origin, const Coordinate &target = Coordinate() )
				{
					return Data.Distance( origin, target );
				}

				Coordinate Size()
				{
					return Data.Size();
				}

				const tTokenData &At( const Coordinate &target )
				{
					return Data.At( target );
				}

				// The tToken<> version places token tiles on the token
				void PlaceToken( const tTokenData &token, const Coordinate &target )
				{
					Data.PlaceTile( token, target );
				}

				// The tToken<> version removes token tiles from the token
				void RemoveToken( const Coordinate &target )
				{
					Data.RemoveTile( target );
				}
			};

			typedef tToken<>		Token;
			///////////////////////////////////////

		public:
			virtual tCoordType Distance( const Coordinate &origin, const Coordinate &target = Coordinate() )
			{
               return ( abs(target.X - origin.X)
					  + abs(target.Y - origin.Y)
					  + abs(target.Z - origin.Z) );
			}
		};

		typedef tSquare<>				Square;
	}
}


#endif // TOOLBOX_MAP_SQUARE_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper



