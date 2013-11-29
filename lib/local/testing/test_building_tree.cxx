/*=========================================================================

  Program:  sampsim
  Module:   test_building_tree.cxx
  Language: C++

=========================================================================*/
//
// .SECTION Description
// Unit tests for the building_tree class
//

#include "UnitTest++.h"

#include "building.h"
#include "common.h"
#include "coordinate.h"
#include "household.h"
#include "individual.h"
#include "building_tree.h"
#include "population.h"
#include "tile.h"
#include "utilities.h"

using namespace std;

int main( const int argc, const char** argv ) { return UnitTest::RunAllTests(); }

TEST( test_building_tree )
{
  // create a population
  sampsim::population *population = new sampsim::population;
  create_test_population( population );

  vector< sampsim::building* > building_list;
  for( auto tile_it = population->get_tile_list_begin();
       tile_it != population->get_tile_list_end();
       ++tile_it )
  {
    sampsim::tile *tile = tile_it->second;
    building_list.insert(
      building_list.begin(),
      tile->get_building_list_begin(),
      tile->get_building_list_end() );

  }

  sampsim::building *b;
  sampsim::building_tree tree = sampsim::building_tree( building_list );
  b = tree.find_nearest( sampsim::coordinate( 5, 5 ) );
  b->get_position().to_csv( cout, cout );
  cout << endl;
  b = tree.find_nearest( sampsim::coordinate( 5, 5 ) );
  b->get_position().to_csv( cout, cout );
  cout << endl;
  b = tree.find_nearest( sampsim::coordinate( 5, 5 ) );
  b->get_position().to_csv( cout, cout );
  cout << endl;

  // clean up
  sampsim::utilities::safe_delete( population );
}
