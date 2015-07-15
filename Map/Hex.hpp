#ifndef TOOLBOX_MAP_HEX_HPP
#define TOOLBOX_MAP_HEX_HPP

/*
 * Toolbox/Map/Hex.hpp
 */

/*****************************************************************************
 * See documentation in Toolbox/Map/Common.hpp
 *
 * Special Thanks:
 *
 * - http://www.redblobgames.com/grids/hexagons/
 *   Fantastic resource, and an excellent primer to understanding this code.
 ****************************************************************************/

/****************************************************************************/
/****************************************************************************/

#include <Toolbox/Map/Common.hpp>


namespace Toolbox
{
	namespace Map
	{
		template <typename _tTileData = Default::tTile, typename _tCoordType = Default::tCoordType>
		class tHex : public tMap< _tTileData, _tCoordType >
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
				typedef tHex< tTokenData, tCoordType >	tData;

				TOOLBOX_MEMORY_POINTERS( tToken<tTokenData> )

			public:
				tHex< tTokenData, tCoordType >		Data;		// Token data is of the same map type, but with a potentially different tileset

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
				// Convert to hex "cube" coordinates
				Coordinate Origin = _MapToHex( origin );
				Coordinate Target = _MapToHex( target );

				// Find the x/y distance in hexes
				tCoordType Dist = (abs(Target.X - Origin.X)
								 + abs(Target.Y - Origin.Y)
								 + abs(Target.Z - Origin.Z)) / 2;

				// Then factor in the original map height difference
				return (Dist + abs(target.Z - origin.Z));
			}

		protected:
			// Converts the map coordinates (axial) to a hex-based coordinate (cube) system for easier hex-map math
			Coordinate _MapToHex( const Coordinate &mapCoords )
			{
				/*
					# convert axial to cube
					x = q
					z = r
					y = -x-z
				*/
				return Coordinate( mapCoords.X,
							 	  -mapCoords.X - mapCoords.Y,
							 	   mapCoords.Y );
			}

			Coordinate _HexToMap( const Coordinate &hexCoords, tCoordType mapZ = 0 )
			{
				/*
					# convert cube to axial
					q = x
					r = z
				*/
				return Coordinate( hexCoords.X,
								   hexCoords.Z,
								   mapZ );
			}

		};

		typedef tHex<>				Hex;
	}
}


#endif // TOOLBOX_MAP_HEX_HPP


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper



