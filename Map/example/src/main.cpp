/*
 * main.cpp
 *
 * A working example for Toolbox::Map
 */

#include <iostream>
#include <stdexcept>

#include <Toolbox/Map.hpp>



enum tCustomToken
{
	Token_Empty,
	Token_Solid,
	Token_Custom,

	Token_MAX,

	Token_FIRST =	Token_Empty,
	Token_DEFAULT =	Token_FIRST,
};


typedef Toolbox::Map::Coordinate				Coordinate;
//typedef Toolbox::Map::Hex						Map;
typedef Toolbox::Map::Square					Map;
typedef Map::Token								Token;			// Default token type


class CustomToken : public Map::tToken< tCustomToken >
{
public:
	TOOLBOX_MEMORY_POINTERS( CustomToken )

public:
	char Facing;			// Which direction is this token facing?

	// Other game-specific items go here

public:
	CustomToken():
		Facing(0)
	{
	}

	CustomToken( char facing ):
		Facing( facing )
	{
	}

	virtual ~CustomToken()
	{
	}
};


typedef Toolbox::Map::Default::tTile			MapTile;


int main( int argc, char *argv[] )
{
    try
    {
		//
		// Map editing
		//
		Map MyMap;

		// Create a solid 2x2 cube floating in space
		// Fill in the squares that make up the top of the floating cube and let the RLE encoding fill in the rest
		auto MapBrush = Map::Tile( MapTile::Tile_Solid );
		MyMap.PlaceTile( MapBrush, Coordinate(0, 0, -1) );
		MyMap.PlaceTile( MapBrush, Coordinate(1, 0, -1) );
		MyMap.PlaceTile( MapBrush, Coordinate(1, 1, -1) );
		MyMap.PlaceTile( MapBrush, Coordinate(0, 1, -1) );

		// But, we'll need to fill the space below the cube with empty tiles again so it will be just a floating cube instead of a tower that goes to the bottom of the map (RLE encoding again)
		MapBrush = Map::Tile( MapTile::Tile_Empty );
		MyMap.PlaceTile( MapBrush, Coordinate(0, 0, 1) );
		MyMap.PlaceTile( MapBrush, Coordinate(1, 0, 1) );
		MyMap.PlaceTile( MapBrush, Coordinate(1, 1, 1) );
		MyMap.PlaceTile( MapBrush, Coordinate(0, 1, 1) );

		// MyMap.RemoveTile( Coordinate ) can be used to remove placed tiles

		std::cout << "Map tile at (0, 0, 0): " << (int)MyMap.At(Coordinate(0,0,0)) << std::endl;

		auto Size = MyMap.Size();	// Will include specifically placed empty tiles
		std::cout << "MyMap size -- X: " << Size.X << " Y: " << Size.Y << "  Z: " << Size.Z << std::endl;

		//
		// Tokens
		//
		Token::Ptr Object = std::make_shared< Token >();				// Multiple token types can be
		CustomToken::Ptr Custom = std::make_shared< CustomToken >( 2 );	//   placed on the same map

		auto TokenBrush = Token::tTokenData( Token_Custom );
		Object->PlaceToken( TokenBrush, Coordinate(0, 0, 0) );
		Object->PlaceToken( TokenBrush, Coordinate(0, 1, 0) );
		Size = Object->Size();
		std::cout << "Object size -- X: " << Size.X << " Y: " << Size.Y << "  Z: " << Size.Z << std::endl;

		std::cout << "At Object::(0, 0, 0): " << (int)Object->At(Coordinate(0,0,0)) << std::endl;

		Custom->PlaceToken( Token_Solid, Coordinate(0, 0, 0) );
		Size = Custom->Size();
		std::cout << "Custom size -- X: " << Size.X << " Y: " << Size.Y << "  Z: " << Size.Z << std::endl;

		std::cout << "At Custom::(0, 0, 0): " << (int)Custom->At(Coordinate(0,0,0)) << std::endl;

		//
		// Tokens on the Map
		//
		// Can be manipulated via the helper functions...
		MyMap.PlaceToken( Object, Coordinate(1, 2, -3) );

		// ...or by direct access
		auto Loc = MyMap.Tokens[ Object ];
		std::cout << "MyObject -- X: " << Loc.X << "  Y: " << Loc.Y << "  Z: " << Loc.Z << std::endl;

		// With any token type (that is based on our map type's token)
		MyMap.Tokens[ Custom ] = Coordinate( 8, 9, 0 );

		Loc = MyMap.Tokens[ Custom ];
		std::cout << "Custom -- X: " << Loc.X << "  Y: " << Loc.Y << "  Z: " << Loc.Z << "  (Facing: " << (int)Custom->Facing << ")" << std::endl;

		// MyMap.RemoveToken( Token::Ptr ) can be used to remove placed tokens from the map

		//
		// Other generic map helper functions are available too (more to come in this area especially)
		//
		std::cout << "Distance between Object and Custom: " << MyMap.Distance( MyMap.Tokens[Object], MyMap.Tokens[Custom] ) << std::endl;
    }
    catch ( std::exception &ex )
    {
		std::cerr << "Fatal error: " << ex.what() << std::endl;
		return 1;
    }

    return 0;
}


// vim: tabstop=4 shiftwidth=4
// astyle: --indent=tab=4 --style=ansi --indent-switches --indent-namespaces --pad-oper


