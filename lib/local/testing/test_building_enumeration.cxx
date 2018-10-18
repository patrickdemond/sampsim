/*=========================================================================

  Program:  sampsim
  Module:   test_building_enumeration.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the building_enumeration class
//

#include "UnitTest++.h"

#include "building.h"
#include "common.h"
#include "household.h"
#include "individual.h"
#include "building_enumeration.h"
#include "population.h"
#include "tile.h"
#include "town.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_building_enumeration )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  building_list_type building_list;
  for( auto town_it = population->get_town_list_begin();
       town_it != population->get_town_list_end();
       ++town_it )
  {
    sampsim::town *town = *town_it;
    for( auto tile_it = town->get_tile_list_begin();
         tile_it != town->get_tile_list_end();
         ++tile_it )
    {
      sampsim::tile *tile = tile_it->second;
      building_list.insert(
        building_list.begin(),
        tile->get_building_list_begin(),
        tile->get_building_list_end() );

    }
  }

  sampsim::building *b;
  //sampsim::building_enumeration building_enumeration = sampsim::building_enumeration( building_list );

  sampsim::coordinate test_coords[] =
  {
    sampsim::coordinate( 7.5, 0.0 ),
    sampsim::coordinate( 0.0, 7.5 ),
    sampsim::coordinate( 2.5, 0.0 ),
    sampsim::coordinate( 0.0, 2.5 ),
    sampsim::coordinate( 8.0, 8.0 ),
    sampsim::coordinate( 2.0, 8.0 ),
    sampsim::coordinate( 2.0, 2.0 ),
    sampsim::coordinate( 8.0, 2.0 ),
    sampsim::coordinate( 5.0, 5.0 ),
  };

  for( int i = 0; i < 9; i++ )
  {
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
