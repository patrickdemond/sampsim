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
    // find the nearest building
    b = tree.find_nearest( test_coords[i] );
    double distance = b->get_position().distance( test_coords[i] );
    cout << "Nearest point to (" << test_coords[i].x << ", " << test_coords[i].y << ") "
         << "is (" << b->get_position().x << ", " << b->get_position().y << ") "
         << "which is " << ( distance * 1000 ) << " meters away" << endl;

    // now check by looping through all buildings whether this is correct
    for( auto building_it = building_list.begin(); building_it != building_list.end(); ++building_it )
    {
      sampsim::building* btemp = *building_it;
      double test = btemp->get_position().distance( test_coords[i] );
      cout << "Testing " << btemp
           << " (" << btemp->get_position().x << ", " << btemp->get_position().y << ") "
           << "which is " << ( test * 1000 ) << " meters away" << endl;
      CHECK( ( *building_it )->get_position().distance( test_coords[i] ) >= distance );
    }
    cout << endl;

    // now remove that building
    sampsim::coordinate bcoord = b->get_position();
    cout << "Removing building at (" << bcoord.x << ", " << bcoord.y << ")" << endl;

    tree.remove( b );
    for( auto building_it = building_list.begin(); building_it != building_list.end(); ++building_it )
    {
      if( *building_it == b )
      {
        building_list.erase( building_it );
        break;
      }
    }

    // and find the next nearest building
    b = tree.find_nearest( bcoord );
    if( NULL == b )
    {
      cout << "Tree is empty" << endl;
    }
    else
    {
      distance = b->get_position().distance( bcoord );
      cout << "Nearest point to removed building is ("
           << b->get_position().x << ", " << b->get_position().y << ") "
           << "which is " << ( distance * 1000 ) << " meters away" << endl;

      // check by looping through all buildings whether this is correct
      for( auto building_it = building_list.begin(); building_it != building_list.end(); ++building_it )
      {
        sampsim::building* btemp = *building_it;
        double test = btemp->get_position().distance( bcoord );
        cout << "Testing " << btemp
             << " (" << btemp->get_position().x << ", " << btemp->get_position().y << ") "
             << "which is " << ( test * 1000 ) << " meters away" << endl;
        CHECK( ( *building_it )->get_position().distance( bcoord ) >= distance );
      }
    }

    cout << endl;
  }

  // clean up
  sampsim::utilities::safe_delete( population );
}
