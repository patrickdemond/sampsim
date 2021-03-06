/*=========================================================================

  Program:  sampsim
  Module:   test_building_catalogue.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the building_catalogue class
//

#include "UnitTest++.h"

#include "building.h"
#include "building_catalogue.h"
#include "common.h"
#include "enumeration.h"
#include "household.h"
#include "individual.h"
#include "population.h"
#include "tile.h"
#include "town.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_building_catalogue )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population, 8, 5000, 100000, 1, 1 );

  unsigned int count = 1;
  for( auto town_it = population->get_town_list_begin();
       town_it != population->get_town_list_end();
       ++town_it )
  {
    cout << "Testing town #" << count++ << endl;

    // create a list of all buildings in the town
    building_list_type building_list;
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

    // create a building catalogue using the building list
    sampsim::building_catalogue *be = new building_catalogue( building_list, 100 );

    cout << "Checking that no enumeration has surpassed the threshold" << endl;
    unsigned int total = 0;
    for( auto it = be->get_enumeration_list_cbegin(); it != be->get_enumeration_list_cend(); ++it )
    {
      sampsim::enumeration *e = *it;
      unsigned int count = e->get_number_of_buildings();
      CHECK( 100 > count );
      total += count;
    }

    cout << "Making sure that the sum of buildings in all enumerations is equal to the number of buildings in the town" << endl;
    CHECK_EQUAL( building_list.size(), total );

    sampsim::utilities::safe_delete( be );
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
